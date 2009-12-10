#include "plugin.h"
#include "commit_dialog.h"
#include "svncommithandler.h"
#include <wx/menu.h>
#include "wxterminal.h"
#include <wx/dirdlg.h>
#include "fileextmanager.h"
#include "svnsettingsdata.h"
#include "svnstatushandler.h"
#include <wx/wupdlock.h>
#include "subversion_strings.h"
#include "subversion_page.h"
#include <wx/xrc/xmlres.h>
#include "svntreedata.h"
#include <wx/imaglist.h>
#include "imanager.h"
#include "workspace.h"
#include <wx/app.h>
#include "subversion2.h"
#include "svnshell.h"

SubversionPage::SubversionPage( wxWindow* parent, Subversion2 *plugin )
		: SubversionPageBase( parent )
		, m_plugin          ( plugin )
{
	CreatGUIControls();
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SubversionPage::OnWorkspaceLoaded), NULL, this);
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SubversionPage::OnWorkspaceClosed), NULL, this);
}

void SubversionPage::OnChangeRootDir( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path(m_textCtrlRootDir->GetValue());
	wxString new_path = wxDirSelector(wxT("Select working directory:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrlRootDir->SetValue(new_path);
		BuildTree();
	}
}

void SubversionPage::OnTreeMenu( wxTreeEvent& event )
{
	// Popup the menu
	wxArrayTreeItemIds items;
	size_t count = m_treeCtrl->GetSelections(items);
	if(count) {
		SvnTreeData::SvnNodeType type = DoGetSelectionType(items);
		if(type == SvnTreeData::SvnNodeTypeInvalid)
			// Mix or an invalid selection
			return;

		wxMenu menu;
		switch (type) {
		case SvnTreeData::SvnNodeTypeFile:
			CreateFileMenu( &menu );
			break;

		case SvnTreeData::SvnNodeTypeRoot:
			CreateRootMenu( &menu );
			break;

		case SvnTreeData::SvnNodeTypeAddedRoot:
		case SvnTreeData::SvnNodeTypeDeletedRoot:
		case SvnTreeData::SvnNodeTypeModifiedRoot:
			CreateSecondRootMenu( &menu );
			break;

		default:
			return;
		}

		PopupMenu( &menu );
	}
}

void SubversionPage::CreatGUIControls()
{
	// Assign the image list
	wxImageList *imageList = new wxImageList(16, 16, true);

	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("folder")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("error")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("edit_add")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("edit_delete")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("project_conflict")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("files_unversioned")));

	// File icons 6-13
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("project" ) ) ); // 6
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("folder" ) ) );  // 7
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("page_white_c" ) ) );//8
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("page_white_cplusplus" ) ) );//9
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("page_white_h" ) ) );//10
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("page_white_text" ) ) );//11
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("workspace" ) ) );//12
	imageList->Add( wxXmlResource::Get()->LoadBitmap(wxT("formbuilder" ) ) );//13

	m_treeCtrl->AssignImageList( imageList );

	if(m_plugin->GetManager()->IsWorkspaceOpen()) {
		m_textCtrlRootDir->SetValue(m_plugin->GetManager()->GetWorkspace()->GetWorkspaceFileName().GetPath());
	}
	BuildTree();
}

void SubversionPage::BuildTree()
{
	ClearAll();
	wxString rootDir = m_textCtrlRootDir->GetValue();

	wxTreeItemId root;
	if(rootDir.IsEmpty()) {
		root = m_treeCtrl->AddRoot(svnNO_FILES_TO_DISPLAY, 0, 0);
		return;
	} else {
		root = m_treeCtrl->AddRoot(rootDir, 0, 0, new SvnTreeData(SvnTreeData::SvnNodeTypeRoot, rootDir));
	}

	wxString command;
	command << DoGetSvnExeName() << wxT("--xml -q --non-interactive status");
	m_simpleCommand.Execute(command, rootDir, new SvnStatusHandler(m_plugin->GetManager(), this));
}

void SubversionPage::OnWorkspaceLoaded(wxCommandEvent& event)
{
	event.Skip();
	m_textCtrlRootDir->SetValue(m_plugin->GetManager()->GetWorkspace()->GetWorkspaceFileName().GetPath());
	BuildTree();
}

void SubversionPage::OnWorkspaceClosed(wxCommandEvent& event)
{
	event.Skip();
	m_textCtrlRootDir->SetValue(wxT(""));
	ClearAll();
	m_plugin->GetShell()->Clear();
}

void SubversionPage::ClearAll()
{
	m_treeCtrl->DeleteAllItems();
}

void SubversionPage::UpdateTree(const wxArrayString& modifiedFiles, const wxArrayString& conflictedFiles, const wxArrayString& unversionedFiles, const wxArrayString& newFiles, const wxArrayString& deletedFiles)
{
	wxTreeItemId root = m_treeCtrl->GetRootItem();
	wxWindowUpdateLocker locker( m_treeCtrl );

	// TODO :: Compare original path with the current path set
	// if they different, skip this

	if(m_treeCtrl->ItemHasChildren(root)) {
		m_treeCtrl->DeleteChildren(root);
	}

	DoAddNode(svnMODIFIED_FILES,    1, SvnTreeData::SvnNodeTypeModifiedRoot,    modifiedFiles);
	DoAddNode(svnADDED_FILES,       2, SvnTreeData::SvnNodeTypeAddedRoot,       newFiles);
	DoAddNode(svnDELETED_FILES,     3, SvnTreeData::SvnNodeTypeDeletedRoot,     deletedFiles);
	DoAddNode(svnCONFLICTED_FILES,  4, SvnTreeData::SvnNodeTypeConflictRoot,    conflictedFiles);
	DoAddNode(svnUNVERSIONED_FILES, 5, SvnTreeData::SvnNodeTypeUnversionedRoot, unversionedFiles);

	if(m_treeCtrl->ItemHasChildren(root)) {
		m_treeCtrl->Expand(root);
	}
}

void SubversionPage::DoAddNode(const wxString& title, int imgId, SvnTreeData::SvnNodeType nodeType, const wxArrayString& files)
{
	wxTreeItemId root = m_treeCtrl->GetRootItem();
	wxString basePath = m_textCtrlRootDir->GetValue();

	// Add the basic four root items
	if(files.IsEmpty() == false) {

		wxTreeItemId parent = m_treeCtrl->AppendItem(root, title, imgId, imgId, new SvnTreeData(nodeType, wxT("")));

		// Add all children items
		for(size_t i=0; i<files.GetCount(); i++) {
			wxString filename(files.Item(i));
			m_treeCtrl->AppendItem(parent, files.Item(i), DoGetIconIndex(filename), DoGetIconIndex(filename), new SvnTreeData(SvnTreeData::SvnNodeTypeFile, files.Item(i)));
		}

		if( nodeType != SvnTreeData::SvnNodeTypeUnversionedRoot) {
			m_treeCtrl->Expand(parent);
		}
	}
}

wxString SubversionPage::DoGetSvnExeName()
{
	SvnSettingsData ssd;
	m_plugin->GetManager()->GetConfigTool()->ReadObject(wxT("SvnSettingsData"), &ssd);
	wxString executeable;
	executeable << wxT("\"") << ssd.GetExecutable() << wxT("\" ");
	return executeable;
}

int SubversionPage::DoGetIconIndex(const wxString& filename)
{
	FileExtManager::Init();
	FileExtManager::FileType type = FileExtManager::GetType(filename);

	int iconIndex( 11 );

	switch ( type ) {
	case FileExtManager::TypeHeader:
		iconIndex = 10;
		break;

	case FileExtManager::TypeSourceC:
		iconIndex = 8;
		break;

	case FileExtManager::TypeSourceCpp:
		iconIndex = 9;
		break;

	case FileExtManager::TypeProject:
		iconIndex = 6;
		break;

	case FileExtManager::TypeWorkspace:
		iconIndex = 12;
		break;

	case FileExtManager::TypeFormbuilder:
		iconIndex = 13;
		break;
	default:
		iconIndex = 11;
		break;
	}
	return iconIndex;
}

SvnTreeData::SvnNodeType SubversionPage::DoGetSelectionType(const wxArrayTreeItemIds& items)
{
	m_paths.Clear();
	SvnTreeData::SvnNodeType type(SvnTreeData::SvnNodeTypeInvalid);
	for(size_t i=0; i<items.GetCount(); i++) {
		if(items.Item(i).IsOk() == false) {
			m_paths.Clear();
			return SvnTreeData::SvnNodeTypeInvalid;
		}

		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(items.Item(i));
		if ( !data ) {
			m_paths.Clear();
			return SvnTreeData::SvnNodeTypeInvalid;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeRoot && items.GetCount() == 1) {
			// populate the list of paths with all the added paths
			DoGetPaths( items.Item(i), m_paths );
			return SvnTreeData::SvnNodeTypeRoot;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the added paths
			DoGetPaths( items.Item(i), m_paths );
			return SvnTreeData::SvnNodeTypeAddedRoot;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeDeletedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the deleted paths
			DoGetPaths( items.Item(i), m_paths );
			return SvnTreeData::SvnNodeTypeDeletedRoot;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeConflictRoot && items.GetCount() == 1) {
			// populate the list of paths with all the conflicted paths
			DoGetPaths( items.Item(i), m_paths );
			return SvnTreeData::SvnNodeTypeConflictRoot;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeModifiedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the conflicted paths
			DoGetPaths( items.Item(i), m_paths );
			return SvnTreeData::SvnNodeTypeModifiedRoot;
		}

		if(type == SvnTreeData::SvnNodeTypeInvalid &&
		   (data->GetType() == SvnTreeData::SvnNodeTypeFile || data->GetType() == SvnTreeData::SvnNodeTypeRoot)) {
			type = data->GetType();
			m_paths.Add(data->GetFilepath());

		} else if( type == SvnTreeData::SvnNodeTypeInvalid ) {
			type = data->GetType();

		} else if(data->GetType() != type) {
			m_paths.Clear();
			return SvnTreeData::SvnNodeTypeInvalid;

		} else {
			// Same type, just add the path
			m_paths.Add(data->GetFilepath());
		}
	}
	return type;
}

void SubversionPage::CreateSecondRootMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnCommit), NULL, this);
}

void SubversionPage::CreateFileMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnCommit), NULL, this);
}

void SubversionPage::CreateRootMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnCommit), NULL, this);
}

void SubversionPage::OnCommit(wxCommandEvent& event)
{
	wxString command;

	// Pope the "Commit Dialog" dialog

	CommitDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), m_paths, m_plugin->GetManager());
	if(dlg.ShowModal() == wxID_OK) {
		m_paths = dlg.GetPaths();
		if(m_paths.IsEmpty())
			return;

		command << DoGetSvnExeName()
				<< wxT(" commit ");

		for(size_t i=0; i<m_paths.GetCount(); i++) {
			command << wxT("\"") << m_paths.Item(i) << wxT("\" ");
		}

		command << wxT(" -m \"");
		command << dlg.GetMesasge();
		command << wxT("\"");
		m_plugin->GetShell()->Run(command, m_textCtrlRootDir->GetValue(), new SvnCommitHandler(m_plugin->GetManager(), this));
	}
}

void SubversionPage::DoGetPaths(const wxTreeItemId& parent, wxArrayString& paths)
{
	if( m_treeCtrl->ItemHasChildren(parent) == false ) {
		return;
	}

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_treeCtrl->GetFirstChild(parent, cookie);
	while( item.IsOk() ) {
		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(item);
		if(data) {
			if(data->GetFilepath().IsEmpty() == false) {
				m_paths.Add( data->GetFilepath() );
			}

			if(( data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot    ||
				 data->GetType() == SvnTreeData::SvnNodeTypeModifiedRoot ||
				 data->GetType() == SvnTreeData::SvnNodeTypeDeletedRoot) &&

				m_treeCtrl->ItemHasChildren(item)) {

				DoGetPaths(item, paths);
			}
		}
		item = m_treeCtrl->GetNextChild(parent, cookie);
	}
}
