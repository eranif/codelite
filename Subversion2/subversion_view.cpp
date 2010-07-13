#include <wx/app.h>
#include "diff_dialog.h"
#include "svn_checkout_dialog.h"
#include "subversion2_ui.h"
#include <wx/settings.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include "plugin.h"
#include "procutils.h"
#include "svn_login_dialog.h"
#include "svn_command_handlers.h"
#include "svn_copy_dialog.h"
#include "commit_dialog.h"
#include "svn_default_command_handler.h"
#include <wx/menu.h>
#include <wx/dirdlg.h>
#include "fileextmanager.h"
#include "svnsettingsdata.h"
#include "svnstatushandler.h"
#include <wx/wupdlock.h>
#include "subversion_strings.h"
#include "subversion_view.h"
#include <wx/xrc/xmlres.h>
#include "svntreedata.h"
#include <wx/imaglist.h>
#include "imanager.h"
#include "workspace.h"
#include "subversion2.h"
#include "svn_console.h"
#include "globals.h"

BEGIN_EVENT_TABLE(SubversionView, SubversionPageBase)
	EVT_UPDATE_UI(XRCID("svn_stop"),         SubversionView::OnStopUI)
	EVT_UPDATE_UI(XRCID("clear_svn_output"), SubversionView::OnClearOuptutUI)

	EVT_MENU(XRCID("svn_link_editor"),        SubversionView::OnLinkEditor)
	EVT_MENU(XRCID("svn_commit"),             SubversionView::OnCommit)
	EVT_MENU(XRCID("svn_update"),             SubversionView::OnUpdate)
	EVT_MENU(XRCID("svn_revert"),             SubversionView::OnRevert)
	EVT_MENU(XRCID("svn_tag"),                SubversionView::OnTag)
	EVT_MENU(XRCID("svn_branch"),             SubversionView::OnBranch)
	EVT_MENU(XRCID("svn_diff"),               SubversionView::OnDiff)
	EVT_MENU(XRCID("svn_patch"),              SubversionView::OnPatch)
	EVT_MENU(XRCID("svn_patch_dry_run"),      SubversionView::OnPatchDryRun)
	EVT_MENU(XRCID("svn_resolve"),            SubversionView::OnResolve)
	EVT_MENU(XRCID("svn_add"),                SubversionView::OnAdd)
	EVT_MENU(XRCID("svn_delete"),             SubversionView::OnDelete)
	EVT_MENU(XRCID("svn_ignore_file"),        SubversionView::OnIgnoreFile)
	EVT_MENU(XRCID("svn_ignore_file_pattern"),SubversionView::OnIgnoreFilePattern)
	EVT_MENU(XRCID("svn_blame"),              SubversionView::OnBlame)
	EVT_MENU(XRCID("svn_checkout"),           SubversionView::OnCheckout)
	EVT_MENU(XRCID("svn_open_file"),          SubversionView::OnOpenFile)
END_EVENT_TABLE()

SubversionView::SubversionView( wxWindow* parent, Subversion2 *plugin )
	: SubversionPageBase( parent )
	, m_plugin          ( plugin )
	, m_simpleCommand   ( plugin )
	, m_diffCommand     ( plugin )
{
	CreatGUIControls();
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_WORKSPACE_LOADED,      wxCommandEventHandler(SubversionView::OnWorkspaceLoaded),     NULL, this);
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_WORKSPACE_CLOSED,      wxCommandEventHandler(SubversionView::OnWorkspaceClosed),     NULL, this);
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_FILE_SAVED,            wxCommandEventHandler(SubversionView::OnRefreshView),         NULL, this);
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_PROJ_FILE_ADDED,       wxCommandEventHandler(SubversionView::OnFileAdded  ),         NULL, this);
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_FILE_RENAMED,          wxCommandEventHandler(SubversionView::OnFileRenamed),         NULL, this);
	m_plugin->GetManager()->GetTheApp()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SubversionView::OnActiveEditorChanged), NULL, this);
}

SubversionView::~SubversionView()
{
}

void SubversionView::OnChangeRootDir( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path(m_textCtrlRootDir->GetValue());
	wxString new_path = wxDirSelector(wxT(""), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlRootDir->SetValue(new_path);
		BuildTree();
	}
}

void SubversionView::OnTreeMenu( wxTreeEvent& event )
{
	// Popup the menu
	wxArrayTreeItemIds items;
	size_t count = m_treeCtrl->GetSelections(items);
	if (count) {
		SvnTreeData::SvnNodeType type = DoGetSelectionType(items);
		if (type == SvnTreeData::SvnNodeTypeInvalid)
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

void SubversionView::CreatGUIControls()
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

	// Add toolbar
	// Create the toolbar
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
	tb->AddTool(XRCID("svn_link_editor"), wxT("Link Editor"), wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), wxT("Link Editor"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("svn_link_editor"), m_plugin->GetSettings().GetFlags() & SvnLinkEditor);

	tb->AddTool(XRCID("clear_svn_output"), wxT("Clear Svn Output Tab"), wxXmlResource::Get()->LoadBitmap(wxT("document_delete")), wxT("Clear Svn Output Tab"), wxITEM_NORMAL);
	tb->AddTool(XRCID("svn_refresh"),      wxT("Refresh View"), wxXmlResource::Get()->LoadBitmap ( wxT ( "svn_refresh" ) ), wxT ( "Refresh View" ) );
	tb->AddSeparator();

	tb->AddTool(XRCID("svn_stop"),         wxT("Stop current svn process"), wxXmlResource::Get()->LoadBitmap ( wxT ( "stop_build16" ) ), wxT ( "Stop current svn process" ) );
	tb->AddTool(XRCID("svn_cleanup"),      wxT("Svn Cleanup"), wxXmlResource::Get()->LoadBitmap ( wxT ( "eraser" ) ), wxT ( "Svn Cleanup" ) );

	tb->AddSeparator();
	tb->AddTool(XRCID("svn_checkout"),         wxT("Svn Checkout"), wxXmlResource::Get()->LoadBitmap ( wxT ( "svn_checkout" ) ), wxT ( "Svn Checkout" ) );

	tb->AddSeparator();
	tb->AddTool(XRCID("svn_settings"),     wxT("Svn Settings..."), wxXmlResource::Get()->LoadBitmap ( wxT ( "svn_settings" ) ), wxT ( "Svn Settings..." ) );
	tb->AddTool(XRCID("svn_info"),         wxT("Svn Info"), wxXmlResource::Get()->LoadBitmap ( wxT ( "svn_info" ) ), wxT ( "Svn Info" ) );

	tb->Connect(XRCID("clear_svn_output"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnClearOuptut), NULL, this);
	tb->Connect(XRCID("svn_stop"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnStop),        NULL, this);
	tb->Connect(XRCID("svn_cleanup"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnCleanup),     NULL, this);
	tb->Connect(XRCID("svn_info"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnShowSvnInfo), NULL, this);
	tb->Connect(XRCID("svn_refresh"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnRefreshView), NULL, this);
	tb->Connect(XRCID("svn_settings"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnSettings),    NULL, this);

	wxSizer *sz = GetSizer();
	sz->Insert(0, tb, 0, wxEXPAND);
	tb->Realize();

	if (m_plugin->GetManager()->IsWorkspaceOpen()) {
		m_textCtrlRootDir->SetValue(m_plugin->GetManager()->GetWorkspace()->GetWorkspaceFileName().GetPath());
	}
	BuildTree();
}

void SubversionView::BuildTree()
{
	BuildTree( m_textCtrlRootDir->GetValue() );
}

void SubversionView::BuildTree(const wxString& root)
{
	if(root.IsEmpty())
		return;

	m_textCtrlRootDir->SetValue(root);
	wxString command;
	command << m_plugin->GetSvnExeName() << wxT("--xml -q status");
	m_simpleCommand.Execute(command, root, new SvnStatusHandler(m_plugin, wxNOT_FOUND, NULL), m_plugin);
}

void SubversionView::OnWorkspaceLoaded(wxCommandEvent& event)
{
	event.Skip();
	Workspace *workspace = m_plugin->GetManager()->GetWorkspace();
	if(m_plugin->GetManager()->IsWorkspaceOpen() && workspace) {
		m_textCtrlRootDir->SetValue(workspace->GetWorkspaceFileName().GetPath());
		BuildTree();
	}
}

void SubversionView::OnWorkspaceClosed(wxCommandEvent& event)
{
	event.Skip();
	m_textCtrlRootDir->SetValue(wxT(""));
	ClearAll();
	m_plugin->GetConsole()->Clear();
}

void SubversionView::ClearAll()
{
	m_treeCtrl->DeleteAllItems();
}

void SubversionView::UpdateTree(const wxArrayString& modifiedFiles, const wxArrayString& conflictedFiles, const wxArrayString& unversionedFiles, const wxArrayString& newFiles, const wxArrayString& deletedFiles)
{

	wxWindowUpdateLocker locker( m_treeCtrl );
	ClearAll();

	// Add root node
	wxString rootDir = m_textCtrlRootDir->GetValue();
	wxTreeItemId root = m_treeCtrl->AddRoot(rootDir, 0, 0, new SvnTreeData(SvnTreeData::SvnNodeTypeRoot, rootDir));

	if(root.IsOk() == false)
		return;

	DoAddNode(svnMODIFIED_FILES,    1, SvnTreeData::SvnNodeTypeModifiedRoot,    modifiedFiles);
	DoAddNode(svnADDED_FILES,       2, SvnTreeData::SvnNodeTypeAddedRoot,       newFiles);
	DoAddNode(svnDELETED_FILES,     3, SvnTreeData::SvnNodeTypeDeletedRoot,     deletedFiles);
	DoAddNode(svnCONFLICTED_FILES,  4, SvnTreeData::SvnNodeTypeConflictRoot,    conflictedFiles);
	DoAddNode(svnUNVERSIONED_FILES, 5, SvnTreeData::SvnNodeTypeUnversionedRoot, unversionedFiles);

	if (m_treeCtrl->ItemHasChildren(root)) {
		m_treeCtrl->Expand(root);
	}
	DoLinkEditor();
}

void SubversionView::DoAddNode(const wxString& title, int imgId, SvnTreeData::SvnNodeType nodeType, const wxArrayString& files)
{
	wxTreeItemId root = m_treeCtrl->GetRootItem();
	wxString basePath = m_textCtrlRootDir->GetValue();

	// Add the basic four root items
	if (files.IsEmpty() == false) {

		wxTreeItemId parent = m_treeCtrl->AppendItem(root, title, imgId, imgId, new SvnTreeData(nodeType, wxT("")));

		// Set the parent node with bold font
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		font.SetWeight(wxBOLD);
		m_treeCtrl->SetItemFont(parent, font);

		// Add all children items
		for (size_t i=0; i<files.GetCount(); i++) {
			wxString filename(files.Item(i));
			m_treeCtrl->AppendItem(parent, files.Item(i), DoGetIconIndex(filename), DoGetIconIndex(filename), new SvnTreeData(SvnTreeData::SvnNodeTypeFile, files.Item(i)));
		}

		if ( nodeType != SvnTreeData::SvnNodeTypeUnversionedRoot) {
			m_treeCtrl->Expand(parent);
		}
	}
}

int SubversionView::DoGetIconIndex(const wxString& filename)
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

SvnTreeData::SvnNodeType SubversionView::DoGetSelectionType(const wxArrayTreeItemIds& items)
{
	m_selectionInfo.Clear();
	SvnTreeData::SvnNodeType type(SvnTreeData::SvnNodeTypeInvalid);
	for (size_t i=0; i<items.GetCount(); i++) {
		if (items.Item(i).IsOk() == false) {
			m_selectionInfo.Clear();
			return m_selectionInfo.m_selectionType; // Invalid
		}

		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(items.Item(i));
		if ( !data ) {
			m_selectionInfo.Clear();
			return m_selectionInfo.m_selectionType; // Invalid
		}

		if (data->GetType() == SvnTreeData::SvnNodeTypeRoot && items.GetCount() == 1) {
			// populate the list of paths with all the added paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeRoot;
			return m_selectionInfo.m_selectionType;
		}

		if (data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the added paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeAddedRoot;
			return m_selectionInfo.m_selectionType;
		}

		if (data->GetType() == SvnTreeData::SvnNodeTypeDeletedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the deleted paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeDeletedRoot;
			return m_selectionInfo.m_selectionType;
		}

		if (data->GetType() == SvnTreeData::SvnNodeTypeConflictRoot && items.GetCount() == 1) {
			// populate the list of paths with all the conflicted paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeConflictRoot;
			return m_selectionInfo.m_selectionType;
		}

		if (data->GetType() == SvnTreeData::SvnNodeTypeModifiedRoot && items.GetCount() == 1) {
			// populate the list of paths with all the conflicted paths
			DoGetPaths( items.Item(i), m_selectionInfo.m_paths );
			m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeModifiedRoot;
			return m_selectionInfo.m_selectionType;
		}

		if (type == SvnTreeData::SvnNodeTypeInvalid &&
		    (data->GetType() == SvnTreeData::SvnNodeTypeFile || data->GetType() == SvnTreeData::SvnNodeTypeRoot)) {
			type = data->GetType();
			m_selectionInfo.m_selectionType = type;
			m_selectionInfo.m_paths.Add(data->GetFilepath());

		} else if ( type == SvnTreeData::SvnNodeTypeInvalid ) {
			type = data->GetType();

		} else if (data->GetType() != type) {
			m_selectionInfo.m_paths.Clear();
			return SvnTreeData::SvnNodeTypeInvalid;

		} else {
			// Same type, just add the path
			m_selectionInfo.m_paths.Add(data->GetFilepath());
		}
	}
	return type;
}

void SubversionView::CreateSecondRootMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),  wxT("Commit"));
	menu->Append(XRCID("svn_update"),  wxT("Update"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_revert"),  wxT("Revert"));
	menu->AppendSeparator();
	menu->Append(XRCID("svn_diff"),    wxT("Create Diff..."));
}

void SubversionView::CreateFileMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_open_file"),  wxT("Open File..."));
	menu->AppendSeparator();
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
	menu->AppendSeparator();

	menu->Append(XRCID("svn_blame"),    wxT("Blame..."));
	menu->AppendSeparator();

	wxMenu *subMenu;
	subMenu = new wxMenu;
	subMenu->Append(XRCID("svn_ignore_file"),         wxT("Ignore this file"));
	subMenu->Append(XRCID("svn_ignore_file_pattern"), wxT("Ignore this file pattern"));
	menu->Append(wxID_ANY, wxT("Ignore"), subMenu);
}

void SubversionView::CreateRootMenu(wxMenu* menu)
{
	menu->Append(XRCID("svn_commit"),        wxT("Commit"));
	menu->Append(XRCID("svn_update"),        wxT("Update"));
	menu->AppendSeparator();

	menu->Append(XRCID("svn_revert"),        wxT("Revert"));
	menu->AppendSeparator();

	menu->Append(XRCID("svn_tag"),           wxT("Create Tag"));
	menu->Append(XRCID("svn_branch"),        wxT("Create Branch"));
	menu->AppendSeparator();

	menu->Append(XRCID("svn_diff"),          wxT("Create Diff..."));
	menu->Append(XRCID("svn_patch"),         wxT("Apply Patch..."));
	menu->Append(XRCID("svn_patch_dry_run"), wxT("Apply Patch - Dry Run..."));
}

void SubversionView::DoGetPaths(const wxTreeItemId& parent, wxArrayString& paths)
{
	if ( m_treeCtrl->ItemHasChildren(parent) == false ) {
		return;
	}

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_treeCtrl->GetFirstChild(parent, cookie);
	while ( item.IsOk() ) {
		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(item);
		if (data) {
			if (data->GetFilepath().IsEmpty() == false) {
				paths.Add( data->GetFilepath() );
			}

			if (( data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot    ||
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

void SubversionView::OnUpdate(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}
	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
	command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" update ");

	if (m_selectionInfo.m_selectionType != SvnTreeData::SvnNodeTypeRoot) {
		// Concatenate list of files to be updated
		for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}
	}

	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnUpdateHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnCommit(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}

	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
	command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" commit ");

	CommitDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), m_selectionInfo.m_paths, m_plugin);
	if (dlg.ShowModal() == wxID_OK) {
		m_selectionInfo.m_paths = dlg.GetPaths();
		if (m_selectionInfo.m_paths.IsEmpty())
			return;


		for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}

		command << wxT(" -m \"");
		command << dlg.GetMesasge();
		command << wxT("\"");
		m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnCommitHandler(m_plugin, event.GetId(), this));
	}
}


void SubversionView::OnAdd(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}

	command << m_plugin->GetSvnExeName(false) << loginString << wxT(" add ");

	// Concatenate list of files to be updated
	for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
		command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
	}

	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnRevert(wxCommandEvent& event)
{
	wxString command;

	// Svn revert does not require login string
	command << m_plugin->GetSvnExeName(false) << wxT(" revert --recursive ");

	if (m_selectionInfo.m_selectionType != SvnTreeData::SvnNodeTypeRoot) {
		// Concatenate list of files to be updated
		for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}
	} else {
		command << wxT(".");
	}
	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnBranch(wxCommandEvent& event)
{
	wxString command;
	command << m_plugin->GetSvnExeName() << wxT("info --xml ");

	SvnInfo svnInfo;
	m_plugin->DoGetSvnInfoSync(svnInfo, m_textCtrlRootDir->GetValue());

	command.Clear();
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}

	// Prompt user for URLs + comment
	SvnCopyDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());

	dlg.SetTitle(wxT("Create Branch"));
	dlg.SetSourceURL(svnInfo.m_sourceUrl);
	dlg.SetTargetURL(svnInfo.m_sourceUrl);

	if (dlg.ShowModal() == wxID_OK) {
		command.Clear();
		// Prepare the 'copy' command
		bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
		command
		        << m_plugin->GetSvnExeName(nonInteractive)
		        << loginString
		        << wxT(" copy ")
		        << dlg.GetSourceURL()
		        << wxT(" ")
		        << dlg.GetTargetURL()
		        << wxT(" -m \"")
		        << dlg.GetMessage()
		        << wxT("\"");

		m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
	}
}

void SubversionView::OnTag(wxCommandEvent& event)
{
	wxString command;
	command << m_plugin->GetSvnExeName() << wxT("info --xml ");

	SvnInfo svnInfo;
	m_plugin->DoGetSvnInfoSync(svnInfo, m_textCtrlRootDir->GetValue());

	// Prompt the login dialog now
	command.Clear();
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}

	// Prompt user for URLs + comment
	SvnCopyDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());

	dlg.SetTitle(wxT("Create Tag"));
	dlg.SetSourceURL(svnInfo.m_sourceUrl);
	dlg.SetTargetURL(svnInfo.m_sourceUrl);

	if (dlg.ShowModal() == wxID_OK) {
		// Prepare the 'copy' command
		bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
		command.Clear();
		command
		        << m_plugin->GetSvnExeName(nonInteractive)
		        << loginString
		        << wxT(" copy ")
		        << dlg.GetSourceURL()
		        << wxT(" ")
		        << dlg.GetTargetURL()
		        << wxT(" -m \"")
		        << dlg.GetMessage()
		        << wxT("\"");

		m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
	}
}

void SubversionView::OnDelete(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}
	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
	command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" --force delete ");

	// Concatenate list of files to be updated
	for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
		command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
	}

	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnResolve(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}

	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
	command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" resolved ");

	// Concatenate list of files to be updated
	for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
		command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
	}

	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnDiff(wxCommandEvent& event)
{
	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}

	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);

	DiffDialog dlg(this, m_plugin->GetManager());
	if(dlg.ShowModal() == wxID_OK) {
		wxString from = dlg.GetFromRevision();
		wxString to   = dlg.GetToRevision();

		if(to.IsEmpty() == false) {
			to.Prepend(wxT(":"));
		}

		// Simple diff
		wxString command;
		command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" diff -r") << from << to << wxT(" ");
		for (size_t i=0; i<m_selectionInfo.m_paths.GetCount(); i++) {
			command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
		}
		m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDiffHandler(m_plugin, event.GetId(), this), false);
	}
}

void SubversionView::OnPatch(wxCommandEvent& event)
{
	m_plugin->Patch(false, m_textCtrlRootDir->GetValue(), this, event.GetId());
}

void SubversionView::OnPatchDryRun(wxCommandEvent& event)
{
	m_plugin->Patch(true, m_textCtrlRootDir->GetValue(), this, event.GetId());
}

void SubversionView::OnCleanup(wxCommandEvent& event)
{
	wxUnusedVar(event);
	wxString command;
	command << m_plugin->GetSvnExeName(false) << wxT(" cleanup ");
	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, wxNOT_FOUND, NULL));
}

void SubversionView::OnStop(wxCommandEvent& event)
{
	wxUnusedVar(event);
	m_plugin->GetConsole()->Stop();
}

void SubversionView::OnClearOuptut(wxCommandEvent& event)
{
	wxUnusedVar(event);
	m_plugin->GetConsole()->Clear();
}

void SubversionView::OnRefreshView(wxCommandEvent& event)
{
	event.Skip();
	BuildTree();
}

void SubversionView::OnFileAdded(wxCommandEvent& event)
{
	event.Skip();
	SvnSettingsData ssd = m_plugin->GetSettings();
	if(ssd.GetFlags() & SvnAddFileToSvn) {
		wxArrayString *files = (wxArrayString*)event.GetClientData();
		if(files) {
			bool     addToSvn(false);
			wxString command;
			command << m_plugin->GetSvnExeName() << wxT(" add ");
			for(size_t i=0; i<files->GetCount(); i++) {

				if(m_plugin->IsPathUnderSvn(files->Item(i))) {
					command << wxT("\"") << files->Item(i) << wxT("\" ");
					addToSvn = true;
				}

			}

			if(addToSvn) {
				command.RemoveLast();
				m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
			}
		}
	}
}

void SubversionView::OnFileRenamed(wxCommandEvent& event)
{
	wxArrayString *files = (wxArrayString*)event.GetClientData();

	// If the Svn Client Version is set to 0.0 it means that we dont have SVN client installed
	if( m_plugin->GetSvnClientVersion() && files && (m_plugin->GetSettings().GetFlags() & SvnRenameFileInRepo) ) {
		wxString oldName = files->Item(0);
		wxString newName = files->Item(1);

		if(m_plugin->IsPathUnderSvn(oldName) == false) {
			event.Skip();
			return;
		}

		wxString command;
		command << m_plugin->GetSvnExeName() << wxT(" rename \"") << oldName << wxT("\" \"") << newName << wxT("\"");
		m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));

	} else
		event.Skip();
}

void SubversionView::OnShowSvnInfo(wxCommandEvent& event)
{
	wxUnusedVar(event);

	SvnInfo svnInfo;
	m_plugin->DoGetSvnInfoSync(svnInfo, m_textCtrlRootDir->GetValue());

	SvnInfoDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());
	dlg.m_textCtrlAuthor->SetValue( svnInfo.m_author );
	dlg.m_textCtrlDate->SetValue( svnInfo.m_date );
	dlg.m_textCtrlRevision->SetValue( svnInfo.m_revision );
	dlg.m_textCtrlRootURL->SetValue( svnInfo.m_url );
	dlg.m_textCtrlURL->SetValue( svnInfo.m_sourceUrl );
	dlg.ShowModal();
}

void SubversionView::OnItemActivated(wxTreeEvent& event)
{
	wxArrayTreeItemIds items;
	wxArrayString      paths;
	size_t count = m_treeCtrl->GetSelections(items);
	for(size_t i=0; i<count; i++) {
		wxTreeItemId item = items.Item(i);

		if(item.IsOk() == false)
			continue;

		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(item);
		if (data && data->GetType() == SvnTreeData::SvnNodeTypeFile) {
			paths.Add(/*m_textCtrlRootDir->GetValue() + wxFileName::GetPathSeparator() + */data->GetFilepath());
		}
	}

	wxString loginString;
	if(m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString) == false) {
		return;
	}
	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);
	wxString diffAgainst(wxT("BASE"));

	// Simple diff
	wxString command;
	command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" diff -r") << diffAgainst << wxT(" ");
	for (size_t i=0; i<paths.GetCount(); i++) {
		command << wxT("\"") << paths.Item(i) << wxT("\" ");
	}
	m_plugin->GetConsole()->Execute(command, m_textCtrlRootDir->GetValue(), new SvnDiffHandler(m_plugin, event.GetId(), this), false);
}

void SubversionView::OnStopUI(wxUpdateUIEvent& event)
{
	event.Enable(m_plugin->GetConsole()->IsRunning());
}

void SubversionView::OnClearOuptutUI(wxUpdateUIEvent& event)
{
	event.Enable(m_plugin->GetConsole()->IsEmpty() == false);
}

void SubversionView::OnCheckout(wxCommandEvent& event)
{
	wxString loginString;
	if(!m_plugin->LoginIfNeeded(event, m_textCtrlRootDir->GetValue(), loginString))
		return;

	wxString command;
	bool nonInteractive = m_plugin->GetNonInteractiveMode(event);

	SvnCheckoutDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), m_plugin);
	if(dlg.ShowModal() == wxID_OK) {
		command << m_plugin->GetSvnExeName(nonInteractive) << loginString << wxT(" co ") << dlg.GetURL() << wxT(" \"") << dlg.GetTargetDir() << wxT("\"");
		m_plugin->GetConsole()->ExecuteURL(command, dlg.GetURL(), new SvnCheckoutHandler(m_plugin, event.GetId(), this), true);
	}
}

void SubversionView::OnIgnoreFile(wxCommandEvent& event)
{
	wxUnusedVar(event);
	m_plugin->IgnoreFiles(m_selectionInfo.m_paths, false);
}

void SubversionView::OnIgnoreFilePattern(wxCommandEvent& event)
{
	wxUnusedVar(event);
	m_plugin->IgnoreFiles(m_selectionInfo.m_paths, true);
}

void SubversionView::OnSettings(wxCommandEvent& event)
{
	wxUnusedVar(event);
	m_plugin->EditSettings();
}

void SubversionView::OnBlame(wxCommandEvent& event)
{
	m_plugin->Blame(event, m_selectionInfo.m_paths);
}

void SubversionView::OnLinkEditor(wxCommandEvent& event)
{
	SvnSettingsData ssd = m_plugin->GetSettings();
	if(event.IsChecked())
		ssd.SetFlags(ssd.GetFlags() | SvnLinkEditor);
	else
		ssd.SetFlags(ssd.GetFlags() & ~SvnLinkEditor);

	m_plugin->SetSettings(ssd);

	DoLinkEditor();
}

void SubversionView::DoLinkEditor()
{
	if(!(m_plugin->GetSettings().GetFlags() & SvnLinkEditor))
		return;

	IEditor *editor = m_plugin->GetManager()->GetActiveEditor();
	if(!editor)
		return;

	wxString fullPath = editor->GetFileName().GetFullPath();
	wxTreeItemId root = m_treeCtrl->GetRootItem();
	if(root.IsOk() == false)
		return;

	wxString basePath = m_textCtrlRootDir->GetValue();
	wxTreeItemIdValue cookie;
	wxTreeItemIdValue childCookie;
	wxTreeItemId parent = m_treeCtrl->GetFirstChild(root, cookie);
	while(parent.IsOk()) {
		// Loop over the main nodes 'modified', 'unversioned' etc
		if(m_treeCtrl->ItemHasChildren(parent)) {
			// Loop over the files under the main nodes
			wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, childCookie);
			while(child.IsOk()) {
				wxString itemText = m_treeCtrl->GetItemText(child);
				wxFileName fn(basePath + wxFileName::GetPathSeparator() + itemText);
				if(fn.GetFullPath() == fullPath) {
					m_treeCtrl->UnselectAll();
					m_treeCtrl->SelectItem(child);
					m_treeCtrl->EnsureVisible(child);
					return;
				}
				child = m_treeCtrl->GetNextChild(parent, childCookie);
			}
		}
		parent = m_treeCtrl->GetNextChild(root, cookie);
	}
}

void SubversionView::OnActiveEditorChanged(wxCommandEvent& event)
{
	event.Skip();
	DoLinkEditor();
}

void SubversionView::DisconnectEvents()
{
	m_plugin->GetManager()->GetTheApp()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SubversionView::OnWorkspaceLoaded),          NULL, this);
	m_plugin->GetManager()->GetTheApp()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SubversionView::OnWorkspaceClosed),          NULL, this);
	m_plugin->GetManager()->GetTheApp()->Disconnect(wxEVT_FILE_SAVED,       wxCommandEventHandler(SubversionView::OnRefreshView),              NULL, this);
	m_plugin->GetManager()->GetTheApp()->Disconnect(wxEVT_PROJ_FILE_ADDED,  wxCommandEventHandler(SubversionView::OnFileAdded),                NULL, this);
	m_plugin->GetManager()->GetTheApp()->Disconnect(wxEVT_FILE_RENAMED,     wxCommandEventHandler(SubversionView::OnFileRenamed),              NULL, this);
	m_plugin->GetManager()->GetTheApp()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SubversionView::OnActiveEditorChanged), NULL, this);
}

void SubversionView::OnOpenFile(wxCommandEvent& event)
{
	wxUnusedVar(event);

	wxArrayTreeItemIds items;
	wxArrayString      paths;
	size_t count = m_treeCtrl->GetSelections(items);
	for(size_t i=0; i<count; i++) {
		wxTreeItemId item = items.Item(i);

		if(item.IsOk() == false)
			continue;

		SvnTreeData *data = (SvnTreeData *)m_treeCtrl->GetItemData(item);
		if (data && data->GetType() == SvnTreeData::SvnNodeTypeFile) {
			paths.Add(m_textCtrlRootDir->GetValue() + wxFileName::GetPathSeparator() + data->GetFilepath());
		}
	}

	for(size_t i=0; i<paths.GetCount(); i++) {

		if(wxFileName(paths.Item(i)).IsDir() == false)
			m_plugin->GetManager()->OpenFile(paths.Item(i));

	}
}
