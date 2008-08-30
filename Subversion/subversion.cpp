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

static bool IsIgnoredFile(const wxString &file, const wxString &patten) {
	wxStringTokenizer tkz(patten, wxT(";"), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		if (wxMatchWild(tkz.NextToken(), file)) {
			return true;
		}
	}
	return false;
}

#define VALIDATE_SVNPATH()\
	{\
		ExeLocator locator;\
		wxString where;\
		if(!locator.Locate(m_options.GetExePath(), where)){\
			wxString message;\
			message << wxT("SVN plugin error: failed to locate svn client installed (searched for: ") << m_options.GetExePath() << wxT(")");\
			wxLogMessage(message);\
			return;\
		}\
	}

static SubversionPlugin* theSvnPlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager) {
	if (theSvnPlugin == 0) {
		theSvnPlugin = new SubversionPlugin(manager);
	}
	return theSvnPlugin;
}

//Define the plugin entry point
extern "C" EXPORT PluginInfo GetPluginInfo() {
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("Svn"));
	info.SetDescription(wxT("Subversion - integration of the subversion command line tool"));
	info.SetVersion(wxT("v1.1"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() {
	return PLUGIN_INTERFACE_VERSION;
}

SubversionPlugin::SubversionPlugin(IManager *manager)
		: IPlugin(manager)
		, m_svnMenu(NULL)
		, m_svn(NULL)
		, topWin(NULL)
		, m_initIsDone(false)
		, m_explorerSepItem(NULL)
		, m_editorSepItem(NULL)
		, m_workspaceSepItem(NULL)
		, m_projectSepItem(NULL) {
	m_svn = new SvnDriver(this, manager);

	manager->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &m_options);
	//m_timer->Start((int)m_options.GetRefreshInterval(), true);

	m_longName = wxT("Subversion");
	m_shortName = wxT("SVN");

	Notebook *book = m_mgr->GetOutputPaneNotebook();
	SvnTab *svnwin = new SvnTab(book);

	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("svn_repo"));
	wxString caption( wxT("Subversion") );
	book->AddPage(svnwin, caption, bmp);

	//Connect items
	if (!topWin) {
		topWin = m_mgr->GetTheApp();
	}

	if (topWin) {
		topWin->Connect(wxEVT_FILE_SAVED, wxCommandEventHandler(SubversionPlugin::OnFileSaved), NULL, this);
		topWin->Connect(wxEVT_FILE_EXP_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnFileExplorerInitDone), NULL, this);
		topWin->Connect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SubversionPlugin::OnProjectFileAdded), NULL, this);
		topWin->Connect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(SubversionPlugin::OnRefreshIconsCond), NULL, this);
		topWin->Connect(wxEVT_PROJ_ADDED, wxCommandEventHandler(SubversionPlugin::OnRefreshIconsCond), NULL, this);
		topWin->Connect(wxEVT_PROJ_REMOVED, wxCommandEventHandler(SubversionPlugin::OnRefreshIconsCond), NULL, this);
		topWin->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnAppInitDone), NULL, this);
		topWin->Connect(wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler(SubversionPlugin::OnLinkClicked), NULL, this);
		topWin->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SubversionPlugin::OnRefrshIconsStatus), NULL, this);

		topWin->Connect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdate), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommit), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_add"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAdd), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_diff"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiff), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_patch"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnPatch), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowSvnStatus_FileExplorer), NULL, (wxEvtHandler*)this);
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
		topWin->Connect(XRCID("svn_update_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateWsp), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_commit_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitWsp), NULL, (wxEvtHandler*)this);

		topWin->Connect(XRCID("svn_refresh_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportPrj), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_update_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdatePrj), NULL, (wxEvtHandler*)this);
		topWin->Connect(XRCID("svn_commit_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitPrj), NULL, (wxEvtHandler*)this);

		topWin->Connect(XRCID("svn_refresh_icons"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRefrshIconsStatus), NULL, (wxEvtHandler*)this);
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

	}
}

wxMenu *SubversionPlugin::CreateEditorPopMenu() {
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

wxMenu *SubversionPlugin::CreatePopMenu() {
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

SubversionPlugin::~SubversionPlugin() {
	SvnCommitMsgsMgr::Release();
	UnPlug();
}

void SubversionPlugin::OnSvnAbort(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nAborting ...\n"));
	m_svn->Abort();
}

void SubversionPlugin::OnChangeLog(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->ChangeLog();
}

void SubversionPlugin::OnResolveConflict(wxCommandEvent& e) {
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

void SubversionPlugin::OnResolveConflictFile(wxCommandEvent& e) {
	VALIDATE_SVNPATH();
	wxUnusedVar(e);

	if (m_mgr->GetActiveEditor()) {
		// this method is called from within the Editor's context menu
		m_svn->PrintMessage(wxString::Format(wxT("----\nResolving Conflicted File: %s ...\n"), m_mgr->GetActiveEditor()->GetFileName().GetFullPath().c_str()));
		m_svn->ResolveConflictedFile(m_mgr->GetActiveEditor()->GetFileName(), new UpdatePostCmdAction(m_mgr, this));
	}
}

void SubversionPlugin::OnCleanup(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nPerforming cleanup ...\n"));
	m_svn->Cleanup();
}

void SubversionPlugin::OnUpdate(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	m_svn->Update(new UpdatePostCmdAction(m_mgr, this));
}

void SubversionPlugin::OnCommit(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->Commit();
}

void SubversionPlugin::OnCommitFile(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	//get the current active editor name
	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->CommitFile(wxT("\"") + editor->GetFileName().GetFullPath() + wxT("\""), new SvnIconRefreshHandler(m_mgr, this));
	}
}

void SubversionPlugin::OnUpdateFile(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->UpdateFile(wxT("\"") + editor->GetFileName().GetFullPath() + wxT("\""), new UpdatePostCmdAction(m_mgr, this));
	}
}

void SubversionPlugin::OnSvnAdd(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nAdding file(s)...\n"));
	m_svn->Add();
}

void SubversionPlugin::OnDiff(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCreating diff file...\n"));
	m_svn->Diff();
}

void SubversionPlugin::OnDiffFile(wxCommandEvent &event) {
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCreating diff file...\n"));

	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->DiffFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnRevertFile(wxCommandEvent &e) {
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	IEditor *editor = m_mgr->GetActiveEditor();
	if (editor) {
		m_svn->RevertFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnDelete(wxCommandEvent &e) {
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	m_svn->Delete();
}

void SubversionPlugin::OnRevert(wxCommandEvent &e) {
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	m_svn->Revert();
}

void SubversionPlugin::OnShowSvnStatus_FileExplorer(wxCommandEvent &event) {
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

wxToolBar *SubversionPlugin::CreateToolBar(wxWindow *parent) {
	wxUnusedVar(parent);
	return NULL;
}

void SubversionPlugin::CreatePluginMenu(wxMenu *pluginsMenu) {
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

void SubversionPlugin::HookPopupMenu(wxMenu *menu, MenuType type) {
	if (type == MenuTypeFileExplorer) {
		if (!menu->FindItem(XRCID("SVN_POPUP"))) {
			m_explorerSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("SVN_POPUP"), wxT("Svn"), CreatePopMenu());
		}
	} else if (type == MenuTypeEditor) {

		if (!menu->FindItem(XRCID("SVN_EDITOR_POPUP"))) {
			m_editorSepItem = menu->AppendSeparator();
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

void SubversionPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type) {
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
			menu->Destroy(m_editorSepItem);
			m_editorSepItem = NULL;
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

void SubversionPlugin::OnFileSaved(wxCommandEvent &e) {
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

void SubversionPlugin::OnOptions(wxCommandEvent &event) {
	wxUnusedVar(event);
	SvnOptionsDlg *dlg = new SvnOptionsDlg(NULL, m_options);
	if (dlg->ShowModal() == wxID_OK) {
		m_options = dlg->GetOptions();
		m_mgr->GetConfigTool()->WriteObject(wxT("SubversionOptions"), &m_options);
	}
	dlg->Destroy();
}

void SubversionPlugin::UnPlug() {
	topWin->Disconnect(XRCID("svn_commit_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_revert_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevertFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_diff_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiffFile), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdate), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommit), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_diff"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiff), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowSvnStatus_FileExplorer), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_changelog"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnChangeLog), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_abort"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAbort), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_cleanup"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCleanup), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_add"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAdd), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_delete"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDelete), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevert), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_refresh_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_update_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_commit_wsp"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitWsp), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("svn_refresh_prj"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnShowReportPrj), NULL, (wxEvtHandler*)this);
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

void SubversionPlugin::OnFileExplorerInitDone(wxCommandEvent &event) {
	wxUnusedVar(event);
}

void SubversionPlugin::OnProjectFileAdded(wxCommandEvent &event) {
	if (IsWorkspaceUnderSvn() == false) {
		return;
	}

	if (m_options.GetFlags() & SvnAutoAddFiles) {
		void *cdata(NULL);
		wxArrayString *files(NULL);
		cdata = event.GetClientData();
		if (cdata) {
			files = (wxArrayString*)cdata;
			for (size_t i=0; i< files->GetCount(); i++) {
				m_svn->Add(files->Item(i));
			}
		}
	}
	if (m_options.GetFlags() & SvnKeepIconsUpdated) {
		SvnIconRefreshHandler handler(m_mgr, this);
		handler.DoCommand();
	}
	event.Skip();
}

void SubversionPlugin::OnAppInitDone(wxCommandEvent &event) {
	m_initIsDone = true;
}

void SubversionPlugin::DoGetWspSvnStatus(wxArrayString &output) {
	wxString command;

	//get list of paths to check
	std::vector<wxFileName> projectFiles;
	std::map<wxString, bool> workspaceFolders;
	
	wxString errMsg;
	wxArrayString projects;
	
	m_mgr->GetWorkspace()->GetProjectList(projects);
	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), errMsg);
		if(p){
			// for each project get list of its files
			projectFiles.clear();
			p->GetFiles(projectFiles, true);
			
			// map the folders of each file 
			for(size_t j=0; j<projectFiles.size(); j++){
				workspaceFolders[projectFiles.at(j).GetPath()] = true;
			}
		}
	}
	
	command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive --no-ignore -q ");
	
	//concatenate list of files here
	std::map<wxString, bool>::iterator iter = workspaceFolders.begin();
	for (; iter != workspaceFolders.end(); iter++) {
		command << wxT("\"") <<  iter->first << wxT("\" ");
	}

	ProcUtils::ExecuteCommand(command, output);
}

void SubversionPlugin::DoGetSvnStatus(const wxString &basePath, wxArrayString &output) {
	wxString command;
	command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive -q --no-ignore \"") << basePath << wxT("\"");
	ProcUtils::ExecuteCommand(command, output);
}

void SubversionPlugin::DoMakeHTML(const wxArrayString &output, const wxString &basePath) {
	Notebook *book =  m_mgr->GetMainNotebook();

	wxString origin(wxT("explorer"));
	wxString _base(basePath);
	if (basePath == wxT("workspace")) {
		origin = wxT("workspace");
		_base.Empty();
	} else if (basePath == wxT("project")) {
		origin = wxT("project");
		_base.Empty();
	}


	book->Freeze();
	for ( size_t i=0; i< book->GetPageCount(); i++) {
		wxHtmlWindow *win = dynamic_cast<wxHtmlWindow *>(book->GetPage(i));
		if (win && book->GetPageText(i) == wxT("SVN Status")) {
			//we found a SVN status page, close it
			book->DeletePage(i);
			break;
		}
	}

	wxString path = m_mgr->GetStartupDirectory();
	wxString name = wxT("svnreport.html");

	wxFileName fn(path, name);
	wxHtmlWindow *reportPage = new wxHtmlWindow(m_mgr->GetMainNotebook(), wxID_ANY, wxDefaultPosition, wxSize(1, 1));

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

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateModified);
	wxString formatStr = FormatRaws(files, _base, SvnXmlParser::StateModified);
	content.Replace(wxT("$(ModifiedFiles)"), formatStr);

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateConflict);
	formatStr = FormatRaws(files, _base, SvnXmlParser::StateConflict);
	content.Replace(wxT("$(ConflictFiles)"), formatStr);

	files.Clear();
	SvnXmlParser::GetFiles(rawData, files, SvnXmlParser::StateUnversioned);
	formatStr = FormatRaws(files, _base, SvnXmlParser::StateUnversioned);
	content.Replace(wxT("$(UnversionedFiles)"), formatStr);

	content.Replace(wxT("$(BasePath)"), _base);
	content.Replace(wxT("$(Origin)"), origin);

	reportPage->SetPage(content);

	//create new report
	m_mgr->GetMainNotebook()->AddPage(reportPage, wxT("SVN Status"), wxNullBitmap, true);
	book->Thaw();
}

void SubversionPlugin::DoGetPrjSvnStatus(wxArrayString &output) {
	//get the selected project name
	wxString command;
	ProjectPtr p = GetSelectedProject();
	if (!p) {
		return;
	}

	command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive --no-ignore -q ");
	//concatenate list of files here
	command << wxT("\"") <<  p->GetFileName().GetPath() << wxT("\" ");
	ProcUtils::ExecuteCommand(command, output);
}

void SubversionPlugin::DoGeneratePrjReport() {
	wxArrayString output;
	DoGetPrjSvnStatus(output);

	DoMakeHTML(output, wxT("project"));
}

void SubversionPlugin::DoGenerateWspReport() {
	wxArrayString output;
	DoGetWspSvnStatus(output);

	DoMakeHTML(output, wxT("workspace"));
}

void SubversionPlugin::DoGenerateReport(const wxString &basePath) {
	wxArrayString output;
	DoGetSvnStatus(basePath, output);
	DoMakeHTML(output, basePath);
}

wxString SubversionPlugin::FormatRaws(const wxArrayString &lines, const wxString &basePath, SvnXmlParser::FileState state) {
	SvnOptions data;
	m_mgr->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &data);

	wxString content;
	if (lines.IsEmpty()) {
		content << wxT("<tr><td><font size=2 face=\"Verdana\">");
		content << wxT("No files were found.");
		content << wxT("</font></td></tr>");
	}

	for (size_t i=0; i<lines.GetCount(); i++) {
		if ( IsIgnoredFile(lines.Item(i), data.GetPattern() ) ) {
			continue;
		}

		content << wxT("<tr><td><font size=2 face=\"Verdana\">");
		content << wxT("<a href=\"action:open-file:") << lines.Item(i) << wxT("\" >") << lines.Item(i) << wxT("</a>") ;
		
		// incase the file is already using an absolute path, dont use basePath
		wxString _basePath = basePath;
		wxFileName fn(lines.Item(i));
		if(fn.IsAbsolute()){
			_basePath = wxEmptyString;
		}
		
		//for modified files, add Diff menu
		switch (state) {
		case SvnXmlParser::StateModified:
			content << wxT(" - ");
			content << wxT("<a href=\"action:diff:") << _basePath << lines.Item(i) << wxT("\" >") << wxT("Diff") << wxT("</a>");
			content << wxT(" ");
			content << wxT("<a href=\"action:revert-$(Origin):") << _basePath << lines.Item(i) << wxT("\" >") << wxT("Revert") << wxT("</a>");
			break;
		case SvnXmlParser::StateUnversioned:
			content << wxT(" - ");
			content << wxT("<a href=\"action:add-$(Origin):") << _basePath << lines.Item(i) << wxT("\" >") << wxT("Add") << wxT("</a>");
			break;
		default:
			break;
		}

		content << wxT("</font></td></tr>");
		content << wxT("</font></td></tr>");
	}
	return content;
}

void SubversionPlugin::OnLinkClicked(wxHtmlLinkEvent &e) {
	wxHtmlLinkInfo info = e.GetLinkInfo();
	wxString action = info.GetHref();

	if (action.StartsWith(wxT("action:"))) {

		action = action.AfterFirst(wxT(':'));
		wxString command = action.BeforeFirst(wxT(':'));
		wxString fileName = action.AfterFirst(wxT(':'));

		wxFileName fn(fileName);
		if (command == wxT("add-explorer")) {
			m_svn->Add(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh")));
		} else if (command == wxT("add-project")) {
			m_svn->Add(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_prj")));
		} else if (command == wxT("add-workspace")) {
			m_svn->Add(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_wsp")));
		} else if (command == wxT("diff")) {
			m_svn->DiffFile(fn);
		} else if (command == wxT("revert-workspace")) {
			m_svn->RevertFile(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_wsp")));
		} else if (command == wxT("revert-project")) {
			m_svn->RevertFile(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh_prj")));
		} else if (command == wxT("revert-explorer")) {
			m_svn->RevertFile(fn, new SvnReportGeneratorAction(this, XRCID("svn_refresh")));
		} else if (command == wxT("commit-all-explorer")) {
			//Commit all files
			m_svn->CommitFile(wxT("\"") + fn.GetFullPath() + wxT("\""));
		} else if (command == wxT("refresh-explorer")) {

			//Commit all files
			SendSvnMenuEvent(XRCID("svn_refresh"));

		} else if (command == wxT("refresh-workspace")) {

			//Commit all files
			SendSvnMenuEvent(XRCID("svn_refresh_wsp"));

		} else if (command == wxT("commit-all-workspace")) {

			SendSvnMenuEvent(XRCID("svn_commit_wsp"));

		} else if (command == wxT("refresh-project")) {

			SendSvnMenuEvent(XRCID("svn_refresh_prj"));

		} else if (command == wxT("commit-all-project")) {

			SendSvnMenuEvent(XRCID("svn_commit_prj"));

		} else {
			e.Skip();
		}
	}
}

wxMenu* SubversionPlugin::CreateWorkspacePopMenu() {
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

wxMenu* SubversionPlugin::CreateProjectPopMenu() {
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

void SubversionPlugin::OnShowReportWsp(wxCommandEvent &e) {
	wxUnusedVar(e);
	wxWindowDisabler disabler;
	DoGenerateWspReport();
}

void SubversionPlugin::OnUpdateWsp(wxCommandEvent &e) {
	wxString file = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME);
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	//concatenate list of files here
	m_svn->UpdateFile(wxT("\"") + file + wxT("\""), new UpdatePostCmdAction(m_mgr, this));
}

void SubversionPlugin::OnCommitWsp(wxCommandEvent &e) {
	wxString file = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME);
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->CommitFile(wxT("\"") + file + wxT("\""), new SvnIconRefreshHandler(m_mgr, this));
}

void SubversionPlugin::OnShowReportPrj(wxCommandEvent &e) {
	wxUnusedVar(e);
	wxWindowDisabler disabler;
	DoGeneratePrjReport();
}

void SubversionPlugin::OnUpdatePrj(wxCommandEvent &e) {
	ProjectPtr p = GetSelectedProject();
	if (!p) {
		return;
	}

	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	//concatenate list of files here
	m_svn->UpdateFile(wxT("\"") + p->GetFileName().GetPath() + wxT("\""), new UpdatePostCmdAction(m_mgr, this));
}

void SubversionPlugin::OnCommitPrj(wxCommandEvent &e) {
	ProjectPtr p = GetSelectedProject();
	if (!p) {
		return;
	}

	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->CommitFile(wxT("\"") + p->GetFileName().GetPath() + wxT("\""), new SvnIconRefreshHandler(m_mgr, this));
}

ProjectPtr SubversionPlugin::GetSelectedProject() {
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

void SubversionPlugin::OnRefrshIconsStatus(wxCommandEvent &e) {
	wxWindowDisabler disabler;
	SvnIconRefreshHandler handler(m_mgr, this);
	handler.DoCommand();
	e.Skip();
}

void SubversionPlugin::OnRefreshIconsCond(wxCommandEvent &e) {
	wxWindowDisabler disabler;
	if (m_options.GetFlags() & SvnKeepIconsUpdated) {
		SvnIconRefreshHandler handler(m_mgr, this);
		handler.DoCommand();
	}
	e.Skip();
}

bool SubversionPlugin::IsWorkspaceUnderSvn() {
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

void SubversionPlugin::SendSvnMenuEvent(int id) {
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, id);
	event.SetEventObject(this);
	wxPostEvent(this, event);
}

void SubversionPlugin::OnPatch(wxCommandEvent& e)
{
	m_svn->PrintMessage(wxT("----\nApplying patch ...\n"));
	m_svn->ApplyPatch(new SvnIconRefreshHandler(m_mgr, this));
}
