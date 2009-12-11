#include "plugin.h"
#include "svn_login_dialog.h"
#include "svncommithandler.h"
#include "svn_copy_dialog.h"
#include "svninfohandler.h"
#include "commit_dialog.h"
#include "svn_default_command_handler.h"
#include <wx/menu.h>
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
	Connect(XRCID("svn_commit2"), wxCommandEventHandler(SubversionPage::OnCommitWithLogin), NULL, this);
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
	wxString rootDir = m_textCtrlRootDir->GetValue();

	wxTreeItemId root;
	if(rootDir.IsEmpty()) {
		root = m_treeCtrl->AddRoot(svnNO_FILES_TO_DISPLAY, 0, 0);
		return;
	}

	wxString command;
	command << DoGetSvnExeName() << wxT("--xml -q status");
	m_simpleCommand.Execute(command, rootDir, new SvnStatusHandler(m_plugin));
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

	wxWindowUpdateLocker locker( m_treeCtrl );
	ClearAll();

	// Add root node
	wxString rootDir = m_textCtrlRootDir->GetValue();
	wxTreeItemId root = m_treeCtrl->AddRoot(rootDir, 0, 0, new SvnTreeData(SvnTreeData::SvnNodeTypeRoot, rootDir));

	// TODO :: Compare original path with the current path set
	// if they different, skip this

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
	executeable << wxT("\"") << ssd.GetExecutable() << wxT("\" --non-interactive ");
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
	m_selectionInfo.Clear();
	SvnTreeData::SvnNodeType type(SvnTreeData::SvnNodeTypeInvalid);
	for(size_t i=0; i<items.GetCount(); i++) {
		if(items.Item(i).IsOk() == false) {
			m_selectionInfo.Clear();
			return m_selectionInfo.m_selectionType; // Invalid
		}

		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(items.Item(i));
		if ( !data ) {
			m_selectionInfo.Clear();
			return m_selectionInfo.m_selectionType; // Invalid
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeRoot && items.GetCount() == 1) {
			// populate the list of paths with all the added paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeRoot;
			return m_selectionInfo.m_selectionType;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the added paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeAddedRoot;
			return m_selectionInfo.m_selectionType;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeDeletedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the deleted paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeDeletedRoot;
			return m_selectionInfo.m_selectionType;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeConflictRoot && items.GetCount() == 1) {
			// populate the list of paths with all the conflicted paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeConflictRoot;
			return m_selectionInfo.m_selectionType;
		}

		if(data->GetType() == SvnTreeData::SvnNodeTypeModifiedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the conflicted paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeModifiedRoot;
			return m_selectionInfo.m_selectionType;
		}

		if(type == SvnTreeData::SvnNodeTypeInvalid &&
		   (data->GetType() == SvnTreeData::SvnNodeTypeFile || data->GetType() == SvnTreeData::SvnNodeTypeRoot)) {
			type = data->GetType();
			m_selectionInfo.m_selectionType = type;
			m_selectionInfo.m_paths.Add(data->GetFilepath());

		} else if( type == SvnTreeData::SvnNodeTypeInvalid ) {
			type = data->GetType();

		} else if(data->GetType() != type) {
			m_selectionInfo.m_paths.Clear();
			return SvnTreeData::SvnNodeTypeInvalid;

		} else {
			// Same type, just add the path
			m_selectionInfo.m_paths.Add(data->GetFilepath());
		}
	}
	return type;
}

void SubversionPage::CreateSecondRootMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Append(XRCID("svn_update"),  wxT("Update"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_revert"),  wxT("Revert"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_diff"),    wxT("Create Diff..."));

	menu->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnCommit), NULL, this);
	menu->Connect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnUpdate), NULL, this);
	menu->Connect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnRevert), NULL, this);
	menu->Connect(XRCID("svn_diff"),   wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnDiff),   NULL, this);
}

void SubversionPage::CreateFileMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Append(XRCID("svn_update"),  wxT("Update"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_revert"),  wxT("Revert"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_add"),     wxT("Add"));
	menu->Append(XRCID("svn_delete"),  wxT("Delete"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_resolve"), wxT("Resolve"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_diff"),    wxT("Create Diff..."));

	menu->Connect(XRCID("svn_commit"),  wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnCommit),  NULL, this);
	menu->Connect(XRCID("svn_update"),  wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnUpdate),  NULL, this);
	menu->Connect(XRCID("svn_add"),     wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnAdd),     NULL, this);
	menu->Connect(XRCID("svn_revert"),  wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnRevert),  NULL, this);
	menu->Connect(XRCID("svn_resolve"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnResolve), NULL, this);
	menu->Connect(XRCID("svn_delete"),  wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnDelete),  NULL, this);
	menu->Connect(XRCID("svn_diff"),    wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnDiff),    NULL, this);
}

void SubversionPage::CreateRootMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Append(XRCID("svn_update"),  wxT("Update"));
	menu->AppendSeparator();

	menu->Append(XRCID("svn_revert"),  wxT("Revert"));
	menu->AppendSeparator();

	menu->Append(XRCID("svn_tag"),     wxT("Create Tag"));
	menu->Append(XRCID("svn_branch"),  wxT("Create Branch"));

	menu->AppendSeparator();
	menu->Append(XRCID("svn_diff"),    wxT("Create Diff..."));
	menu->Append(XRCID("svn_patch"),   wxT("Apply Patch..."));

	menu->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnCommit), NULL, this);
	menu->Connect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnUpdate), NULL, this);
	menu->Connect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnRevert), NULL, this);
	menu->Connect(XRCID("svn_tag"),    wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnTag),    NULL, this);
	menu->Connect(XRCID("svn_branch"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnBranch), NULL, this);
	menu->Connect(XRCID("svn_diff"),   wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnDiff),   NULL, this);
	menu->Connect(XRCID("svn_patch"),  wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SubversionPage::OnPatch),  NULL, this);
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
				paths.Add( data->GetFilepath() );
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

////////////////////////////////////////////
// Source control command handlers
////////////////////////////////////////////

void SubversionPage::OnUpdate(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT(" update ");

	if(m_selectionInfo.m_selectionType != SvnTreeData::SvnNodeTypeRoot) {
		// Concatenate list of files to be updated
		for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}
	}

	m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin));
}

void SubversionPage::OnCommit(wxCommandEvent& event)
{
	wxString command;

	// Pope the "Commit Dialog" dialog

	CommitDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), m_selectionInfo.m_paths, m_plugin->GetManager());
	if(dlg.ShowModal() == wxID_OK) {
		m_selectionInfo.m_paths = dlg.GetPaths();
		if(m_selectionInfo.m_paths.IsEmpty())
			return;

		command << DoGetSvnExeName()
				<< wxT(" commit ");

		for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}

		command << wxT(" -m \"");
		command << dlg.GetMesasge();
		command << wxT("\"");
		m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnCommitHandler(m_plugin));
	}
}


void SubversionPage::OnAdd(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT(" add ");

	// Concatenate list of files to be updated
	for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
		command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
	}

	m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin));
}

void SubversionPage::OnRevert(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT(" revert --recursive ");

	if(m_selectionInfo.m_selectionType != SvnTreeData::SvnNodeTypeRoot) {
		// Concatenate list of files to be updated
		for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}
	} else {
		command << wxT(".");
	}
	m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin));
}

void SubversionPage::OnBranch(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT("info --xml ");

	m_simpleCommand.Execute(command, m_textCtrlRootDir->GetValue(), new SvnInfoHandler(m_plugin, SvnInfo_Branch));
}

void SubversionPage::OnTag(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT("info --xml ");

	m_simpleCommand.Execute(command, m_textCtrlRootDir->GetValue(), new SvnInfoHandler(m_plugin, SvnInfo_Tag));
}

void SubversionPage::OnDelete(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT(" delete ");

	// Concatenate list of files to be updated
	for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
		command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
	}

	m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin));
}

void SubversionPage::OnResolve(wxCommandEvent& event)
{
	wxString command;
	command << DoGetSvnExeName() << wxT(" resolved ");

	// Concatenate list of files to be updated
	for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
		command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
	}

	m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin));
}

void SubversionPage::OnDiff(wxCommandEvent& event)
{
}

void SubversionPage::OnPatch(wxCommandEvent& event)
{
}

void SubversionPage::OnSvnInfo(const SvnInfo& svnInfo, int reason)
{
	if(reason == SvnInfo_Info ) {
		// Do something with this
	} else {

		SvnCopyDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());

		switch(reason) {
		default:
		case SvnInfo_Tag:
			dlg.SetTitle(wxT("Create Tag"));
			dlg.SetSourceURL(svnInfo.m_sourceUrl);
			dlg.SetTargetURL(svnInfo.m_url + wxT("/tags/"));
			break;
		case SvnInfo_Branch:
			dlg.SetTitle(wxT("Create Branch"));
			dlg.SetSourceURL(svnInfo.m_sourceUrl);
			dlg.SetTargetURL(svnInfo.m_url + wxT("/branches/"));
			break;
		}
		if(dlg.ShowModal() == wxID_OK) {
			wxString command;
			command << DoGetSvnExeName()
					<< wxT("copy ")
					<< dlg.GetSourceURL()
					<< wxT(" ")
					<< dlg.GetTargetURL()
					<< wxT(" -m \"")
					<< dlg.GetMessage()
					<< wxT("\"");
			m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin));
		}
	}
}

void SubversionPage::OnCommitWithLogin(wxCommandEvent& event)
{
	SvnLoginDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());
	if(dlg.ShowModal() == wxID_OK) {
		wxString command;
		command << DoGetSvnExeName() << wxT(" commit --username ") << dlg.GetUsername() << wxT(" --password ") << dlg.GetPassword() << wxT(" ");

		// Pop the commit dialog message now
		CommitDialog commitdlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), m_selectionInfo.m_paths, m_plugin->GetManager());
		if(commitdlg.ShowModal() == wxID_OK) {
			m_selectionInfo.m_paths = commitdlg.GetPaths();

			if(m_selectionInfo.m_paths.IsEmpty())
				return;

			for(size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
				command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
			}

			command << wxT(" -m \"");
			command << commitdlg.GetMesasge();
			command << wxT("\"");
			m_plugin->GetShell()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnCommitHandler(m_plugin));
		}
	}
}
