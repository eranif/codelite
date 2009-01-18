//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : subversion.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "updatepostcmdaction.h"
#include "applypatchpostcmdaction.h"
#include "svnreportgeneratoraction.h"
#include "custom_tab.h"
#include "svntab.h"
#include "wx/ffile.h"
#include "svniconrefreshhandler.h"
#include "wx/html/htmlwin.h"
#include "workspace.h"
#include "svnadditemsdlg.h"
#include "wx/tokenzr.h"
#include "subversion.h"
#include "procutils.h"
#include "svncommitmsgsmgr.h"
#include "wx/busyinfo.h"
#include "globals.h"
#include "wx/menu.h"
#include "wx/xrc/xmlres.h"
#include "svndriver.h"
#include "wx/app.h"
#include "virtualdirtreectrl.h"
#include "wx/treectrl.h"
#include "svnhandler.h"
#include "svnoptionsdlg.h"
#include "exelocator.h"
#include "svnxmlparser.h"
#include "dirsaver.h"
#include <vector>
#include "custom_tabcontainer.h"

int ProjectConflictIconId 	= wxNOT_FOUND;
int ProjectModifiedIconId 	= wxNOT_FOUND;
int ProjectOkIconId 		= wxNOT_FOUND;
int WorkspaceModifiedIconId = wxNOT_FOUND;
int WorkspaceConflictIconId = wxNOT_FOUND;
int WorkspaceOkIconId 		= wxNOT_FOUND;
int FileModifiedIconId		= wxNOT_FOUND;
int FileConflictIconId 		= wxNOT_FOUND;
int FileOkIconId			= wxNOT_FOUND;
int FolderModifiedIconId	= wxNOT_FOUND;
int FolderConflictIconId 	= wxNOT_FOUND;
int FolderOkIconId			= wxNOT_FOUND;
int wxFBModifiedIconId		= wxNOT_FOUND;
int wxFBOkIconId			= wxNOT_FOUND;
int wxFBConflictIconId		= wxNOT_FOUND;

#define VALIDATE_SVNPATH(){\
		if(!SanityCheck()){\
			return;\
		}\
	}

static bool IsIgnoredFile(const wxString &file, const wxString &patten)
{
	wxStringTokenizer tkz(patten, wxT(";"), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		if (wxMatchWild(tkz.NextToken(), file)) {
			return true;
		}
	}
	return false;
}

static SubversionPlugin* theSvnPlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (theSvnPlugin == 0) {
		theSvnPlugin = new SubversionPlugin(manager);
	}
	return theSvnPlugin;
}

//Define the plugin entry point
extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("Svn"));
	info.SetDescription(wxT("Subversion - integration of the subversion command line tool"));
	info.SetVersion(wxT("v1.1"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

SubversionPlugin::SubversionPlugin(IManager *manager)
		: IPlugin(manager)
		, m_svnMenu(NULL)
		, m_svn(NULL)
		, topWin(NULL)
		, m_explorerSepItem(NULL)
		, m_workspaceSepItem(NULL)
		, m_projectSepItem(NULL)
{
	m_svn = new SvnDriver(this, manager);

	manager->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &m_options);

	// Set SSH client environment variable
	DoSetSshEnv();

	m_longName = wxT("Subversion");
	m_shortName = wxT("SVN");

	Notebook *book = m_mgr->GetOutputPaneNotebook();
	SvnTab *svnwin = new SvnTab(book);

	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("svn_repo"));
	wxString caption( wxT("Subversion") );
	book->AddPage(svnwin, caption, wxT("Subversion"), bmp);

	//Connect items
	if (!topWin) {
		topWin = m_mgr->GetTheApp();
	}

	if (topWin) {
		topWin->Connect(wxEVT_FILE_SAVED, wxCommandEventHandler(SubversionPlugin::OnFileSaved), NULL, this);
		topWin->Connect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SubversionPlugin::OnProjectFileAdded), NULL, this);
		topWin->Connect(wxEVT_FILE_VIEW_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnRefrshIconsStatus), NULL, this);
        topWin->Connect(wxEVT_FILE_VIEW_REFRESHED, wxCommandEventHandler(SubversionPlugin::OnRefreshIconsCond), NULL, this);

		topWin->Connect(wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler(SubversionPlugin::OnLinkClicked), NULL, this);

		topWin->Connect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdate), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommit), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_add"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAdd), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_diff"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiff), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_patch"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnPatch), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowSvnStatus_FileExplorer), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_svr_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowSvnServerStatus_FileExplorer), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_cleanup"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCleanup), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_changelog"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnChangeLog), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_abort"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAbort), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_delete"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDelete), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevert), NULL, (wxEvtHandler*)this);

		topWin->Connect(XRCID("svn_commit_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitFile), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_update_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateFile), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_revert_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevertFile), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_diff_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiffFile), NULL, (wxEvtHandler*)this);

		topWin->Connect(XRCID("svn_refresh_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportWsp), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_svr_refresh_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowServerReportWsp), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_update_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateWsp), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_commit_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitWsp), NULL, (wxEvtHandler*)this);

		topWin->Connect(XRCID("svn_refresh_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportPrj), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_svr_refresh_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowServerReportPrj), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_update_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdatePrj), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_commit_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitPrj), NULL, (wxEvtHandler*)this);

		topWin->Connect(XRCID("svn_refresh_icons"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRefrshIconsStatusInternal), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("editor_resolve_conflicted_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnResolveConflictFile), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_resolve_conflicted_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnResolveConflict), NULL, (wxEvtHandler*)this);
	}

	//wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	//tree->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(SubversionPlugin::OnTreeExpanded), NULL, this);
	wxTreeCtrl *tree = m_mgr->GetTree(TreeFileView);
	if (tree) {
		//IMPORTANT!
		//note that the order the images are added is important !!
		//do not change it
		ProjectOkIconId			= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("project_ok")));
		ProjectModifiedIconId 	= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("project_modified")));
		ProjectConflictIconId 	= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("project_conflict")));

		WorkspaceOkIconId		= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("workspace_ok")));
		WorkspaceModifiedIconId = tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("workspace_modified")));
		WorkspaceConflictIconId = tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("workspace_conflict")));

		FileOkIconId			= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_ok")));
		FileModifiedIconId 		= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_modified")));
		FileConflictIconId 		= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_conflict")));

		FolderOkIconId			= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("folder_ok")));
		FolderModifiedIconId 		= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("folder_modified")));
		FolderConflictIconId 		= tree->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("folder_conflict")));

		// wxFormBuilder support
		wxString basePath(m_mgr->GetInstallDirectory() + wxT("/plugins/resources/"));
		wxBitmap wxfb_ok, wxfb_modified, wxfb_conflict;

		wxfb_ok.LoadFile(basePath + wxT("wxfb_ok.png"), wxBITMAP_TYPE_PNG);
		wxfb_modified.LoadFile(basePath + wxT("wxfb_modified.png"), wxBITMAP_TYPE_PNG);
		wxfb_conflict.LoadFile(basePath + wxT("wxfb_conflict.png"), wxBITMAP_TYPE_PNG);

		wxFBConflictIconId = tree->GetImageList()->Add(wxfb_conflict);
		wxFBOkIconId = tree->GetImageList()->Add(wxfb_ok);
		wxFBModifiedIconId = tree->GetImageList()->Add(wxfb_modified);
	}
}

wxMenu *SubversionPlugin::CreateEditorPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_commit_file"), wxT("&Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_update_file"), wxT("&Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_diff_file"), wxT("&Diff"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_revert_file"), wxT("&Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("editor_resolve_conflicted_file"), wxT("Resol&ve"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu *SubversionPlugin::CreatePopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_refresh"), wxT("Show Svn S&tatus Report"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_update"), wxT("&Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_commit"), wxT("&Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_add"), wxT("&Add"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_delete"), wxT("&Delete"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_revert"), wxT("&Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_resolve_conflicted_file"), wxT("Resol&ve"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_diff"), wxT("Show D&iff"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_patch"), wxT("Apply &Patch"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_changelog"), wxT("Create Change &Log"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_cleanup"), wxT("Cl&eanup"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_abort"), wxT("A&bort Current Operation"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

SubversionPlugin::~SubversionPlugin()
{
	SvnCommitMsgsMgr::Release();
	UnPlug();
}

void SubversionPlugin::OnSvnAbort(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);

	m_svn->PrintMessage(wxT("----\nAborting ...\n"));
	m_svn->Abort();
}

void SubversionPlugin::OnChangeLog(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->ChangeLog();
}

void SubversionPlugin::OnResolveConflict(wxCommandEvent& e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);

	// this methods is invoked from the file level menu (file explorer / workspace view)
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if (item.m_fileName.FileExists()) {
		m_svn->PrintMessage(wxString::Format(wxT("----\nResolving Conflicted File: %s ...\n"), item.m_fileName.GetFullPath().c_str()));
		m_svn->ResolveConflictedFile(item.m_fileName, new UpdatePostCmdAction(m_mgr, this));
	} else {
		m_svn->PrintMessage(wxString::Format(wxT("----\nPlease select a file to resolve and not the whole directory\n")));
	}
}

void SubversionPlugin::OnResolveConflictFile(wxCommandEvent& e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);

	if (m_mgr->GetActiveEditor()) {
		// this method is called from within the Editor's context menu
		m_svn->PrintMessage(wxString::Format(wxT("----\nResolving Conflicted File: %s ...\n"), m_mgr->GetActiveEditor()->GetFileName().GetFullPath().c_str()));
		m_svn->ResolveConflictedFile(m_mgr->GetActiveEditor()->GetFileName(), new UpdatePostCmdAction(m_mgr, this));
	}
}

void SubversionPlugin::OnCleanup(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nPerforming cleanup ...\n"));
	m_svn->Cleanup();
}

void SubversionPlugin::OnUpdate(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
    PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion"));
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	m_svn->Update(new UpdatePostCmdAction(m_mgr, this));
}

void SubversionPlugin::OnCommit(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
    PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion"));
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->Commit();
}

void SubversionPlugin::OnCommitFile(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
    PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion"));
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	//get the current active editor name
	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->CommitFile(wxT("\"") + editor->GetFileName().GetFullPath() + wxT("\""), new SvnIconRefreshHandler(m_mgr, this));
	}
}

void SubversionPlugin::OnUpdateFile(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
    PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion"));
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->UpdateFile(wxT("\"") + editor->GetFileName().GetFullPath() + wxT("\""), new UpdatePostCmdAction(m_mgr, this));
	}
}

void SubversionPlugin::OnSvnAdd(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nAdding file(s)...\n"));
	m_svn->Add();
}

void SubversionPlugin::OnDiff(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCreating diff file...\n"));
	m_svn->Diff();
}

void SubversionPlugin::OnDiffFile(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCreating diff file...\n"));

	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->DiffFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnRevertFile(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->RevertFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnDelete(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	m_svn->Delete();
}

void SubversionPlugin::OnRevert(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	m_svn->Revert();
}

void SubversionPlugin::OnShowSvnStatus_FileExplorer(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if (item.m_item.IsOk()) {
		//Generate report for base directory
		if (item.m_fileName.IsDir()) {
			//Run the SVN command
			// Execute a sync command to get modified files

			DoGenerateReport(item.m_fileName.GetPath(wxPATH_GET_VOLUME));
			return;
		}
	}
	event.Skip();
}

void SubversionPlugin::OnShowSvnServerStatus_FileExplorer(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if (item.m_item.IsOk()) {
		//Generate report for base directory
		if (item.m_fileName.IsDir()) {
			//Run the SVN command
			// Execute a sync command to get modified files

			DoGenerateReport(item.m_fileName.GetPath(wxPATH_GET_VOLUME), true);
			return;
		}
	}
	event.Skip();
}

wxToolBar *SubversionPlugin::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);
	return NULL;
}

void SubversionPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_options"), wxT("Options..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Subversion"), menu);

	if (!topWin) {
		topWin = m_mgr->GetTheApp();
	}
	topWin->Connect(XRCID("svn_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnOptions), NULL, (wxEvtHandler*)this);
}

void SubversionPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileExplorer) {
		if (!menu->FindItem(XRCID("SVN_POPUP"))) {
			m_explorerSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("SVN_POPUP"), wxT("Svn"), CreatePopMenu());
		}
	} else if (type == MenuTypeEditor) {

		if (!menu->FindItem(XRCID("SVN_EDITOR_POPUP"))) {
			menu->Append(XRCID("SVN_EDITOR_POPUP"), wxT("Svn"), CreateEditorPopMenu());
		}
	} else if (type == MenuTypeFileView_Workspace) {

		if (!IsWorkspaceUnderSvn()) {
			return;
		}

		if (!menu->FindItem(XRCID("SVN_WORKSPACE_POPUP"))) {
			m_workspaceSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("SVN_WORKSPACE_POPUP"), wxT("Svn"), CreateWorkspacePopMenu());
		}

	} else if (type == MenuTypeFileView_Project) {
		if (!IsWorkspaceUnderSvn()) {
			return;
		}

		if (!menu->FindItem(XRCID("SVN_PROJECT_POPUP"))) {
			//No svn menu
			m_projectSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("SVN_PROJECT_POPUP"), wxT("Svn"), CreateProjectPopMenu());
		}

	}
}

void SubversionPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileExplorer) {
		wxMenuItem *item = menu->FindItem(XRCID("SVN_POPUP"));
		if (item) {
			menu->Destroy(item);
			menu->Destroy(m_explorerSepItem);
			m_explorerSepItem = NULL;
		}

	} else if (type == MenuTypeEditor) {
		wxMenuItem *item = menu->FindItem(XRCID("SVN_EDITOR_POPUP"));
		if (item) {
			menu->Destroy(item);
		}
	} else if (type == MenuTypeFileView_Workspace) {
		wxMenuItem *item = menu->FindItem(XRCID("SVN_WORKSPACE_POPUP"));
		if (item) {
			menu->Destroy(item);
			menu->Destroy(m_workspaceSepItem);
			m_workspaceSepItem = NULL;
		}
	} else if (type == MenuTypeFileView_Project) {
		wxMenuItem *item = menu->FindItem(XRCID("SVN_PROJECT_POPUP"));
		if (item) {
			menu->Destroy(item);
			menu->Destroy(m_projectSepItem);
			m_projectSepItem = NULL;
		}
	}
}

void SubversionPlugin::OnFileSaved(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();

	SvnOptions options;
	m_mgr->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &options);

	bool updateAfterSave ( false );
	options.GetFlags() & SvnUpdateAfterSave ? updateAfterSave = true : updateAfterSave = false;

	if (updateAfterSave) {
		SvnIconRefreshHandler handler(m_mgr, this);
		handler.DoCommand();
	}
	e.Skip();
}

void SubversionPlugin::OnOptions(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SvnOptionsDlg dlg(NULL, m_options, m_mgr);
	if (dlg.ShowModal() == wxID_OK) {
		m_options = dlg.GetOptions();
		m_mgr->GetConfigTool()->WriteObject(wxT("SubversionOptions"), &m_options);

		DoSetSshEnv();
	}
}

void SubversionPlugin::UnPlug()
{
    topWin->Disconnect(wxEVT_FILE_SAVED, wxCommandEventHandler(SubversionPlugin::OnFileSaved), NULL, this);
    topWin->Disconnect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SubversionPlugin::OnProjectFileAdded), NULL, this);
    topWin->Disconnect(wxEVT_FILE_VIEW_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnRefrshIconsStatus), NULL, this);
    topWin->Disconnect(wxEVT_FILE_VIEW_REFRESHED, wxCommandEventHandler(SubversionPlugin::OnRefreshIconsCond), NULL, this);

    topWin->Disconnect(wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler(SubversionPlugin::OnLinkClicked), NULL, this);

	topWin->Disconnect(XRCID("svn_commit_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_revert_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevertFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_diff_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiffFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdate), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommit), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_diff"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiff), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowSvnStatus_FileExplorer), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_svr_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowSvnServerStatus_FileExplorer), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_changelog"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnChangeLog), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_abort"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAbort), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_cleanup"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCleanup), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_add"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAdd), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_delete"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDelete), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevert), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_refresh_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_svr_refresh_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowServerReportWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_commit_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_refresh_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportPrj), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_svr_refresh_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowServerReportPrj), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdatePrj), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_commit_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitPrj), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("editor_resolve_conflicted_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnResolveConflictFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_resolve_conflicted_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnResolveConflict), NULL, (wxEvtHandler*)this);

	// before this plugin is un-plugged we must remove the tab we added
	for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
		if (m_mgr->GetOutputPaneNotebook()->GetPageText(i) == wxT("Subversion")) {
			SvnTab *win = (SvnTab*)m_mgr->GetOutputPaneNotebook()->GetPage(i);
			m_mgr->GetOutputPaneNotebook()->RemovePage(i, false);
			win->Destroy();
			break;
		}
	}

	if (m_svn) {
		m_svn->Shutdown();
		delete m_svn;
		m_svn = NULL;
	}

	if (m_svnMenu) {
		delete m_svnMenu;
		m_svnMenu = NULL;
	}
}

void SubversionPlugin::OnProjectFileAdded(wxCommandEvent &e)
{
	e.Skip();
	if (IsWorkspaceUnderSvn() == false) {
		return;
	}

	if (m_options.GetFlags() & SvnAutoAddFiles) {
		void *cdata(NULL);
		wxArrayString *files(NULL);
		cdata = e.GetClientData();
		if (cdata) {
			files = (wxArrayString*)cdata;
			for (size_t i=0; i< files->GetCount(); i++) {
				m_svn->Add(files->Item(i));
			}
		}
	}
}

void SubversionPlugin::DoGetWspSvnStatus(const wxString &basePath, wxArrayString &output, bool inclOutOfDate)
{
    SmartPtr<PluginBusyMessage> wait_msg;
    if (inclOutOfDate) {
        wait_msg.Reset(new PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion")));
    }

	//get list of paths to check
	std::map<wxString, bool> workspaceFolders;

	workspaceFolders[basePath] = true;

	wxString errMsg;
	wxArrayString projects;
	m_mgr->GetWorkspace()->GetProjectList(projects);
	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), errMsg);
		if (p) {
			workspaceFolders[p->GetFileName().GetPath()] = true;
			// for each project get list of its files and map the folders
			std::vector<wxFileName> projectFiles;
			p->GetFiles(projectFiles, true);
			for (size_t j=0; j<projectFiles.size(); j++) {
				workspaceFolders[projectFiles.at(j).GetPath()] = true;
			}
		}
	}

	//form basic svn command line
	wxString command;
	command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive -q --no-ignore ");
	if (inclOutOfDate) {
		command << wxT("-u ");
	}

	//build list of directories
	std::map<wxString, bool>::iterator iter = workspaceFolders.begin(), next = iter;
	for (next++; iter != workspaceFolders.end(); iter = next++) {
		wxFileName dir = wxFileName::DirName(iter->first);

		//check that this directory is actually under svn control
		dir.AppendDir(wxT(".svn"));
		if (!dir.DirExists()) {
			m_svn->PrintMessage(wxString::Format(wxT("Skipping non-versioned directory: %s\n"), iter->first.c_str()));
			workspaceFolders.erase(iter);
			continue;
		}
		dir.RemoveLastDir();

		//check that this directory is not a child of another directory under svn control
		do {
			dir.RemoveLastDir();
		} while (dir.GetDirCount() > 0 && workspaceFolders.find(dir.GetPath()) == workspaceFolders.end());
		if (dir.GetDirCount() > 0) {
			workspaceFolders.erase(iter);
			continue;
		}

		//directory passed tests, so add it to the cmd line
		command << wxT("\"") <<  iter->first << wxT("\" ");
	}

	m_svn->PrintMessage(wxString::Format(wxT("Executing: %s\n"), command.c_str()));
	ProcUtils::ExecuteCommand(command, output);
}

void SubversionPlugin::DoGetSvnStatus(const wxString &basePath, wxArrayString &output, bool inclOutOfDate)
{
    SmartPtr<PluginBusyMessage> wait_msg;
    if (inclOutOfDate) {
        wait_msg.Reset(new PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion")));
    }

	wxString command;
	command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive -q --no-ignore ");
	if (inclOutOfDate) {
		command << wxT("-u ");
	}
	command << wxT("\"") << basePath << wxT("\"");
	ProcUtils::ExecuteCommand(command, output);
}

static int compare_files(const wxString &first, const wxString &second)
{
	wxFileName firstFile(first);
	wxFileName secondFile(second);

	firstFile.MakeAbsolute();
	secondFile.MakeAbsolute();

	int cmp = firstFile.GetVolume().CmpNoCase(secondFile.GetVolume());
	if (cmp != 0)
		return cmp;

	wxArrayString firstDirs = firstFile.GetDirs();
	wxArrayString secondDirs = secondFile.GetDirs();
	for (size_t i = 0; i < firstDirs.Count() && i < secondDirs.Count(); i++) {
		cmp = firstDirs[i].CmpNoCase(secondDirs[i]);
		if (cmp != 0)
			return cmp;
	}

	cmp = firstDirs.Count()-secondDirs.Count();
	if (cmp != 0)
		return cmp;

	cmp = firstFile.GetName().CmpNoCase(secondFile.GetName());
	if (cmp != 0)
		return cmp;

	return firstFile.GetExt().CmpNoCase(secondFile.GetExt());
}

void SubversionPlugin::DoMakeHTML(const wxArrayString &output, const wxString &origin, const wxString &basePath, bool inclOutOfDate)
{
	wxString path = m_mgr->GetStartupDirectory();
	wxString name = wxT("svnreport.html");

	wxFileName fn(path, name);

	//read the file content
	wxString content;
	ReadFileWithConversion(fn.GetFullPath(), content);

	wxString rawData;
	for (size_t i=0; i< output.GetCount(); i++) {
		rawData << output.Item(i);
	}

	//replace the page macros
	//$(ModifiedFiles)
	wxArrayString files;
	wxString formatStr;

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateOutOfDate);
	files.Sort(compare_files);
	formatStr = FormatRaws(files, basePath, SvnXmlParser::StateOutOfDate, inclOutOfDate);
	content.Replace(wxT("$(OutOfDateFiles)"), formatStr);

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateModified);
	files.Sort(compare_files);
	formatStr = FormatRaws(files, basePath, SvnXmlParser::StateModified);
	content.Replace(wxT("$(ModifiedFiles)"), formatStr);

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateConflict);
	files.Sort(compare_files);
	formatStr = FormatRaws(files, basePath, SvnXmlParser::StateConflict);
	content.Replace(wxT("$(ConflictFiles)"), formatStr);

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateUnversioned);
	files.Sort(compare_files);
	formatStr = FormatRaws(files, basePath, SvnXmlParser::StateUnversioned);
	content.Replace(wxT("$(UnversionedFiles)"), formatStr);

	content.Replace(wxT("$(RptType)"), inclOutOfDate ? wxT("Online") : wxT("Offline"));
	content.Replace(wxT("$(Origin)"), origin);
	content.Replace(wxT("$(BasePath)"), basePath);
	content.Replace(wxT("$(DateTime)"), wxDateTime::Now().Format());

	wxHtmlWindow *reportPage = dynamic_cast<wxHtmlWindow*>(m_mgr->FindPage(wxT("SVN Status")));
    if (reportPage == NULL) {
        reportPage = new wxHtmlWindow(m_mgr->GetDockingManager()->GetManagedWindow(), wxID_ANY, wxDefaultPosition, wxSize(1, 1));
        m_mgr->AddPage(reportPage, wxT("SVN Status"));
    }
	reportPage->SetPage(content);
    m_mgr->SelectPage(reportPage);
}

void SubversionPlugin::DoGetPrjSvnStatus(const wxString &basePath, wxArrayString &output, bool inclOutOfDate)
{
    SmartPtr<PluginBusyMessage> wait_msg;
    if (inclOutOfDate) {
        wait_msg.Reset(new PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion")));
    }

	//get the selected project name
	wxString command;
	command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive -q --no-ignore ");
	if (inclOutOfDate) {
		command << wxT("-u ");
	}
	wxFileName dir(basePath);
	dir.AppendDir(wxT(".svn"));
	if (dir.DirExists()) {
		command << wxT("\"") <<  basePath << wxT("\" ");
		ProcUtils::ExecuteCommand(command, output);
	} else {
		m_svn->PrintMessage(wxString::Format(_("Directory '%s' is not under SVN\n"), basePath.c_str()));
	}
}

void SubversionPlugin::DoGeneratePrjReport(bool inclOutOfDate)
{
	wxString basePath;
	wxArrayString output;
	ProjectPtr p = GetSelectedProject();
	if (p) {
		basePath = p->GetFileName().GetPath();
		DoGetPrjSvnStatus(basePath, output, inclOutOfDate);
	}
	DoMakeHTML(output, wxT("project"), basePath, inclOutOfDate);
}

void SubversionPlugin::DoGenerateWspReport(bool inclOutOfDate)
{
	wxString basePath = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath();
	wxArrayString output;
	DoGetWspSvnStatus(basePath, output, inclOutOfDate);
	DoMakeHTML(output, wxT("workspace"), basePath, inclOutOfDate);
}

void SubversionPlugin::DoGenerateReport(const wxString &basePath, bool inclOutOfDate)
{
	wxArrayString output;
	DoGetSvnStatus(basePath, output, inclOutOfDate);
	DoMakeHTML(output, wxT("explorer"), basePath, inclOutOfDate);
}

wxString SubversionPlugin::FormatRaws(const wxArrayString &lines, const wxString &basePath, SvnXmlParser::FileState state, bool inclOutOfDate)
{
	SvnOptions data;
	m_mgr->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &data);

	wxString content;
	content << wxT("<tr><td><font size=2 face=\"Verdana\"><center>");

	if (!lines.IsEmpty()) {
		content << wxT("<table width=100% cellspacing=\"0\" cellpadding=\"2\">");
	} else if (state != SvnXmlParser::StateOutOfDate || inclOutOfDate) {
		content << wxT("<b>No files were found.</b>");
	} else {
		content << wxT("<b>Query not made.</b>");
	}

	wxColour lineCol(0xd0,0xff,0xff);
	wxString lastDir;
	for (size_t i=0; i<lines.GetCount(); i++) {
		if ( IsIgnoredFile(lines.Item(i), data.GetPattern() ) ) {
			continue;
		}

		wxFileName fn(lines.Item(i));
		fn.MakeAbsolute();

		if (lastDir != fn.GetPath()) {
			lineCol.Set(lineCol.Red() == 0xff ? 0xd0 : 0xff, 0xff, 0xff);
		}
		content << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX)<< wxT("\">");

		if (lastDir != fn.GetPath()) {
			wxFileName rfn = fn;
			rfn.MakeRelativeTo(basePath);
			content << wxT("<td align=\"left\">") << rfn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) << wxT("</td>");
		} else {
			content << wxT("<td>&nbsp;</td>");
		}

		content << wxT("<td><a href=\"action:open-file:") << lines.Item(i) << wxT("\" >");
		content << fn.GetFullName();
		content << wxT("</a></td>") ;

		//for modified files, add Diff menu
		switch (state) {
		case SvnXmlParser::StateModified:
			content << wxT("<td><a href=\"action:diff:") << fn.GetFullPath() << wxT("\" >") << wxT("Diff") << wxT("</a></td>");
			content << wxT("<td><a href=\"action:revert-$(Origin):") << fn.GetFullPath() << wxT("\" >") << wxT("Revert") << wxT("</a></td>");
			break;
		case SvnXmlParser::StateUnversioned:
			content << wxT("<td><a href=\"action:add-$(Origin):") << fn.GetFullPath() << wxT("\" >") << wxT("Add") << wxT("</a></td>");
			content << wxT("<td>&nbsp;</td>");
			break;
		case SvnXmlParser::StateOutOfDate:
			content << wxT("<td><a href=\"action:update-$(Origin):") << fn.GetFullPath() << wxT("\" >") << wxT("Update") << wxT("</a></td>");
			content << wxT("<td>&nbsp;</td>");
		default:
			content << wxT("<td>&nbsp;</td>");
			content << wxT("<td>&nbsp;</td>");
			break;
		}

		content << wxT("</tr>");
		lastDir = fn.GetPath();
	}
	if (!lines.IsEmpty()) {
		content << wxT("</table>");
	}
	content << wxT("</center></font></td></tr>");
	return content;
}

void SubversionPlugin::OnLinkClicked(wxHtmlLinkEvent &e)
{
	wxHtmlLinkInfo info = e.GetLinkInfo();
	wxString action = info.GetHref();

	if (action.StartsWith(wxT("action:"))) {

		action = action.AfterFirst(wxT(':'));
		wxString command = action.BeforeFirst(wxT(':'));
		wxString fileName = action.AfterFirst(wxT(':'));

		wxFileName fn(fileName);

		if (command == wxT("commit-all-explorer")) {
			m_svn->CommitFile(wxT("\"") + fn.GetFullPath() + wxT("\""));
		} else if (command == wxT("commit-all-project")) {
			SendSvnMenuEvent(XRCID("svn_commit_prj"));
		} else if (command == wxT("commit-all-workspace")) {
			SendSvnMenuEvent(XRCID("svn_commit_wsp"));

		} else if (command == wxT("update-all-explorer")) {
			m_svn->UpdateFile(wxT("\"") + fn.GetFullPath() + wxT("\""));
		} else if (command == wxT("update-all-project")) {
			SendSvnMenuEvent(XRCID("svn_update_prj"));
		} else if (command == wxT("update-all-workspace")) {
			SendSvnMenuEvent(XRCID("svn_update_wsp"));

		} else if (command == wxT("refresh-explorer")) {
			SendSvnMenuEvent(XRCID("svn_refresh"));
		} else if (command == wxT("refresh-project")) {
			SendSvnMenuEvent(XRCID("svn_refresh_prj"));
		} else if (command == wxT("refresh-workspace")) {
			SendSvnMenuEvent(XRCID("svn_refresh_wsp"));

		} else if (command == wxT("refresh-server-explorer")) {
			SendSvnMenuEvent(XRCID("svn_svr_refresh"));
		} else if (command == wxT("refresh-server-project")) {
			SendSvnMenuEvent(XRCID("svn_svr_refresh_prj"));
		} else if (command == wxT("refresh-server-workspace")) {
			SendSvnMenuEvent(XRCID("svn_svr_refresh_wsp"));

		} else if (command == wxT("diff")) {
			m_svn->DiffFile(fn);

		} else if (command == wxT("add-explorer")) {
			m_svn->Add(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh")));
		} else if (command == wxT("add-project")) {
			m_svn->Add(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_prj")));
		} else if (command == wxT("add-workspace")) {
			m_svn->Add(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_wsp")));

		} else if (command == wxT("revert-explorer")) {
			m_svn->RevertFile(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh")));
		} else if (command == wxT("revert-project")) {
			m_svn->RevertFile(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_prj")));
		} else if (command == wxT("revert-workspace")) {
			m_svn->RevertFile(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_wsp")));

		} else if (command == wxT("update-explorer")) {
			m_svn->UpdateFile(wxT("\"") + fn.GetFullPath() + wxT("\""), new SvnReportGeneratorAction(this, XRCID("svn_svr_refresh")));
		} else if (command == wxT("update-project")) {
			m_svn->UpdateFile(wxT("\"") + fn.GetFullPath() + wxT("\""), new SvnReportGeneratorAction(this, XRCID("svn_svr_refresh_prj")));
		} else if (command == wxT("update-workspace")) {
			m_svn->UpdateFile(wxT("\"") + fn.GetFullPath() + wxT("\""), new SvnReportGeneratorAction(this, XRCID("svn_svr_refresh_wsp")));

		} else {
			e.Skip();
		}
	}
}

wxMenu* SubversionPlugin::CreateWorkspacePopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_refresh_wsp"), wxT("Show Svn S&tatus Report"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_refresh_icons"), wxT("Refresh Svn Icons"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_update_wsp"), wxT("&Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_commit_wsp"), wxT("&Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu* SubversionPlugin::CreateProjectPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_refresh_prj"), wxT("Show Svn S&tatus Report"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_refresh_icons"), wxT("Refresh Svn Icons"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_update_prj"), wxT("&Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_commit_prj"), wxT("&Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

void SubversionPlugin::OnShowReportWsp(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxWindowDisabler disabler;
	DoGenerateWspReport();
}

void SubversionPlugin::OnShowServerReportWsp(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxWindowDisabler disabler;
	DoGenerateWspReport(true);
}

void SubversionPlugin::OnUpdateWsp(wxCommandEvent &e)
{
	wxString file = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME);
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	//concatenate list of files here
	m_svn->UpdateFile(wxT("\"") + file + wxT("\""), new UpdatePostCmdAction(m_mgr, this));
}

void SubversionPlugin::OnCommitWsp(wxCommandEvent &e)
{
	wxString file = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME);
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->CommitFile(wxT("\"") + file + wxT("\""), new SvnIconRefreshHandler(m_mgr, this));
}

void SubversionPlugin::OnShowReportPrj(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxWindowDisabler disabler;
	DoGeneratePrjReport();
}

void SubversionPlugin::OnShowServerReportPrj(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxWindowDisabler disabler;
	DoGeneratePrjReport(true);
}

void SubversionPlugin::OnUpdatePrj(wxCommandEvent &e)
{
	ProjectPtr p = GetSelectedProject();
	if (!p) {
		return;
	}

    PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion"));
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	//concatenate list of files here
	m_svn->UpdateFile(wxT("\"") + p->GetFileName().GetPath() + wxT("\""), new UpdatePostCmdAction(m_mgr, this));
}

void SubversionPlugin::OnCommitPrj(wxCommandEvent &e)
{
	ProjectPtr p = GetSelectedProject();
	if (!p) {
		return;
	}

    PluginBusyMessage(m_mgr, wxT("Contacting SVN server..."), XRCID("subversion"));
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->CommitFile(wxT("\"") + p->GetFileName().GetPath() + wxT("\""), new SvnIconRefreshHandler(m_mgr, this));
}

ProjectPtr SubversionPlugin::GetSelectedProject()
{
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	if ( item.m_text.IsEmpty() ) {
		return NULL;
	}

	wxString errMsg;
	ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(item.m_text, errMsg);
	if (!p) {
		return NULL;
	}
	return p;
}

void SubversionPlugin::OnRefrshIconsStatus(wxCommandEvent &e)
{
	DoRefreshIcons();
	e.Skip();
}

void SubversionPlugin::OnRefreshIconsCond(wxCommandEvent &e)
{
  	if (m_options.GetFlags() & SvnKeepIconsUpdated) {
        DoRefreshIcons();
    }
	e.Skip();
}

bool SubversionPlugin::IsWorkspaceUnderSvn()
{
	if (!m_mgr->IsWorkspaceOpen()) {
		//no workspace is opened
		return false;
	}
	wxString file_name = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

	if (wxDir::Exists(file_name + wxFileName::GetPathSeparator() + wxT(".svn"))) {
		return true;
	}
	if (wxDir::Exists(file_name + wxFileName::GetPathSeparator() + wxT("_svn"))) {
		return true;
	}
	return false;
}

void SubversionPlugin::SendSvnMenuEvent(int id)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, id);
	event.SetEventObject(this);
	wxPostEvent(this, event);
}

void SubversionPlugin::OnPatch(wxCommandEvent& e)
{
	m_svn->PrintMessage(wxT("----\nApplying patch ...\n"));
	m_svn->ApplyPatch(new ApplyPatchPostCmdAction(m_mgr, this));
}

bool SubversionPlugin::SanityCheck()
{
	static bool hasSvn = false;

	if (!hasSvn) {
		ExeLocator locator;
		wxString where;
		if (!locator.Locate(m_options.GetExePath(), where)) {
			wxString message;
			message << wxT("SVN plugin error: failed to locate svn client installed (searched for: ") << m_options.GetExePath() << wxT(")");
			wxLogMessage(message);
			return false;
		}
		hasSvn = true;
	}
	return hasSvn;
}

void SubversionPlugin::DoRefreshIcons()
{
    if (!m_mgr->IsWorkspaceOpen())
        return;

	wxWindowDisabler disabler;
    PluginBusyMessage wait_msg(m_mgr, wxT("Updating SVN Icons..."), XRCID("subversion"));

    SvnIconRefreshHandler handler(m_mgr, this);
    handler.DoCommand();
}

void SubversionPlugin::OnRefrshIconsStatusInternal(wxCommandEvent& e)
{
	wxUnusedVar(e); // don't skip
	DoRefreshIcons();
}

void SubversionPlugin::DoSetSshEnv()
{
	wxString ssh_client = m_options.GetSshClient();
	ssh_client.Trim().Trim(false);
	if(ssh_client.empty() == false){
		wxSetEnv(wxT("SVN_SSH"), ssh_client.c_str());
		wxLogMessage(wxString::Format(wxT("Environment variable SVN_SSH is set to %s"), ssh_client.c_str()));
	}
}
