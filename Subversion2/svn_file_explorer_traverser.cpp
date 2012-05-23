#include "svn_file_explorer_traverser.h"
#include "virtualdirtreectrl.h"
#include "wx/wxsqlite3.h"

SvnFileExplorerTraverser::SvnFileExplorerTraverser(wxTreeCtrl *tree, const Map_t &files, size_t imgCount, const wxString &repoPath)
	: wxTreeTraverser(tree)
	, m_files(files)
	, m_imgCount(imgCount)
	, m_repoPath(repoPath)
{
}

SvnFileExplorerTraverser::~SvnFileExplorerTraverser()
{
}

void SvnFileExplorerTraverser::OnItem(const wxTreeItemId& item)
{
	VdtcTreeItemBase * itemData = dynamic_cast<VdtcTreeItemBase*>(m_tree->GetItemData(item));
	if ( itemData ) {

		wxFileName filename;
		if(itemData->IsDir())
			filename = wxFileName(itemData->GetFullpath(), wxT(""));
		else
			filename = itemData->GetFullpath();
			
		if(!IsPathUnderSvn ( filename.GetFullPath(), itemData->IsDir() ) ) {

			if(itemData->IsDir() && filename.GetDirCount()) {

				// this directory is not under version control, however if the parent directory is under version control
				// force it to use the 'Unversioned' icon
				filename.RemoveLastDir();
				if(IsPathUnderSvn(filename.GetFullPath(), true)) {
					int itemIndx = itemData->GetIconId() == -1 ? -1 : m_imgCount + itemData->GetIconId() + (m_imgCount * SvnFileExplorerTraverser::Unversioned);
					m_tree->SetItemImage(item, itemIndx);
					m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_Selected);
					m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_SelectedExpanded);
				}
			}
			return;
		}

		Map_t::const_iterator iter = m_files.find(itemData->GetFullpath());
		if(iter != m_files.end()) {
			SvnFileInfo fi = iter->second;
			switch(fi.type) {
			case Modified:
			case Locked:
			case Conflicted:
			case Deleted: {
				// Keep the parents up to be marked as modified
				wxArrayTreeItemIds parents;
				DoGetParentsUpToRoot(item, parents);

				for(size_t i=0; i<parents.GetCount(); i++)
					m_dirs.insert(parents.Item(i));
				// fall through
			}

			case Unversioned:
			case New: {
				// Example:
				// Modified - Folder:
				// Modified type => 1
				// m_imgCount  => 18
				// Folder icon => 1
				// Our goal is to get icon idx => 37
				// Modified range: 36 - 53 (18 icons) 36 => Root/Mod, 37 => Folder/Mod, 20 => ...
				// The target icon is: 18 + (1) + (1 * 18)
				int itemIndx = itemData->GetIconId() == -1 ? -1 : m_imgCount + itemData->GetIconId() + (m_imgCount * fi.type);
				m_tree->SetItemImage(item, itemIndx);
				m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_Selected);
				m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_SelectedExpanded);
				break;
			}
			case Ignored:
				// do nothing ... keep the current image
				break;
			case Ok:
			default: {
				int itemIndx = itemData->GetIconId() == -1 ? -1 : m_imgCount + itemData->GetIconId() + (m_imgCount * SvnFileExplorerTraverser::Ok);
				m_tree->SetItemImage(item, itemIndx);
				m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_Selected);
				m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_SelectedExpanded);
				break;

			}
			}
		} else {
			int itemIndx = itemData->GetIconId() == -1 ? -1 : m_imgCount + itemData->GetIconId() + (m_imgCount * SvnFileExplorerTraverser::Ok);
			m_tree->SetItemImage(item, itemIndx);
			m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_Selected);
			m_tree->SetItemImage(item, itemIndx, wxTreeItemIcon_SelectedExpanded);
		}
	}
}

bool SvnFileExplorerTraverser::IsPathUnderSvn(const wxString& path, bool isDir)
{
	wxFileName fn;

	if(isDir) {
		fn = wxFileName(path, wxT(""));

	} else {
		fn = path;
	}

	fn.AppendDir(wxT(".svn"));
	MapPaths_t::const_iterator iter = m_svnPaths.find(fn.GetPath());
	if(iter == m_svnPaths.end()) {
		bool exists = ::wxDirExists(fn.GetPath());
		m_svnPaths[fn.GetPath()] = exists;
		return exists;
		
	} else {
		return iter->second;
		
	}
}

void SvnFileExplorerTraverser::Traverse(const wxTreeItemId& item)
{
	m_rootItem = item;

	wxString wcDB;
	wcDB << m_repoPath << wxFileName::GetPathSeparator() << wxT(".svn") << wxFileName::GetPathSeparator() << wxT("wc.db");
	wxFileName fnDB = wcDB;
	if(fnDB.FileExists()) {
		// Version 1.7 or higher
		// Load all the repository paths
		m_svnPaths.clear();

		try {
			wxSQLite3Database db;
			db.Open(fnDB.GetFullPath());
			if(db.IsOpen()) {
				wxString query;
				query << wxT("select distinct(parent_relpath) from NODES");
				wxSQLite3ResultSet rs = db.ExecuteQuery(query);
				while (rs.NextRow()) {
					wxString path;
					path << m_repoPath << wxFileName::GetPathSeparator() << rs.GetString(0);
					wxFileName fnPath(path, wxT(""));
					fnPath.AppendDir(wxT(".svn"));
					m_svnPaths[fnPath.GetPath()] = true;
				}
				db.Close();
			}

		} catch (wxSQLite3Exception &e) {

		}
	}

	// Determine the root item for our repository
	VdtcTreeItemBase * itemData = dynamic_cast<VdtcTreeItemBase*>(m_tree->GetItemData(item));
	if( itemData && !IsPathUnderSvn(itemData->GetFullpath(), itemData->IsDir()) ) {
		return;
	}

	wxTreeTraverser::Traverse(m_rootItem);

	// Mark the parents as modified as well
	if(!m_dirs.empty()) {
		m_dirs.insert(m_rootItem);
		Set_t::const_iterator iter = m_dirs.begin();
		
		for(; iter != m_dirs.end(); iter++) {
			
			VdtcTreeItemBase * itemData = dynamic_cast<VdtcTreeItemBase*>(m_tree->GetItemData(item));
			if(!itemData)
				continue;

			int itemIndx = itemData->GetIconId() == -1 ? -1 : m_imgCount + itemData->GetIconId() + (m_imgCount * SvnFileExplorerTraverser::Modified);
			m_tree->SetItemImage(*iter, itemIndx);
			m_tree->SetItemImage(*iter, itemIndx, wxTreeItemIcon_Selected);
			m_tree->SetItemImage(*iter, itemIndx, wxTreeItemIcon_SelectedExpanded);
		}

	}
}

void SvnFileExplorerTraverser::DoGetParentsUpToRoot(const wxTreeItemId& item, wxArrayTreeItemIds& items)
{
	if(m_tree->GetRootItem() == item)
		return;

	wxTreeItemId parent = m_tree->GetItemParent(item);
	while( parent.IsOk() && parent != m_tree->GetRootItem() ) {

		VdtcTreeItemBase * itemData = dynamic_cast<VdtcTreeItemBase*>(m_tree->GetItemData(parent));
		if(!itemData)
			break;

		if(!IsPathUnderSvn(itemData->GetFullpath(), itemData->IsDir()))
			break;

		items.Add( parent );
		parent = m_tree->GetItemParent(parent);

	}
}
