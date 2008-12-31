//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : manager.cpp
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
#include "precompiled_header.h"
#include "custombuildrequest.h"
#include <wx/stdpaths.h>
#include "memoryview.h"
#include "dockablepanemenumanager.h"
#include <wx/busyinfo.h>
#include "refactorindexbuildjob.h"
#include "jobqueue.h"
#include <wx/progdlg.h>
#include "cppwordscanner.h"
#include "tokendb.h"
#include "archive.h"
#include "environmentconfig.h"
#include "buidltab.h"
#include "errorstab.h"
#include "fileexplorer.h"
#include "manager.h"
#include "ctags_manager.h"
#include "frame.h"
#include "cl_editor.h"
#include "menumanager.h"
#include "language.h"
#include "editor_config.h"
#include "parse_thread.h"
#include "search_thread.h"
#include "workspace.h"
#include "cpp_symbol_tree.h"
#include "fileview.h"
#include <wx/file.h>
#include "wx/arrstr.h"
#include "context_manager.h"
#include "wx/tokenzr.h"
#include "buildmanager.h"
#include "macros.h"
#include "dirtraverser.h"
#include "depends_dlg.h"
#include "build_settings_config.h"
#include "dirsaver.h"
#include "algorithm"
#include "async_executable_cmd.h"
#include "fileutils.h"
#include "workspace_pane.h"
#include "close_all_dlg.h"
#include "vcimporter.h"
#include "debuggermanager.h"
#include "output_pane.h"
#include "localvarstree.h"
#include "wx/regex.h"
#include "simpletable.h"
#include "quickwatchdlg.h"
#include "fileexplorertree.h"
#include "listctrlpanel.h"
#include "breakpointdlg.h"
#include "globals.h"
#include "exelocator.h"
#include "attachdbgprocdlg.h"
//#include "debuggertip.h"
#include "openwindowspanel.h"
#include "threadlistpanel.h"
#include "plugin.h"
#include "custom_notebook.h"
#include "sessionmanager.h"
#include "workspacetab.h"
#include <vector>

extern unsigned int UTF8Length ( const wchar_t *uptr, unsigned int tlen );
//
// The CodeLite manager class
//
//
#if defined (__WXMSW__)
static wxString CL_EXEC_WRAPPER = wxT ( "le_exec.exe" );
#elif defined (__WXGTK__)
static wxString CL_EXEC_WRAPPER = wxT ( "le_exec.sh" );
#else
static wxString CL_EXEC_WRAPPER = wxEmptyString;
#endif

#define CHECK_MSGBOX(res)										\
	if( !res )													\
	{															\
		wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
		return;													\
	}

#define CHECK_MSGBOX_BOOL(res)								\
	if( !res )													\
	{															\
		wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
		return res;												\
	}

static bool HideDebuggerPane = true;

//--------------------------------------------------------------
// Helper methods
//--------------------------------------------------------------
/**
 * \brief strip accelerators and mnemonics from given text
 * \param text
 * \return
 */
static wxString StripAccelAndNemonics ( const wxString &text )
{
	//possible mnemonics:
	//_ and &
	wxString stripedText ( text );
	stripedText.Replace ( wxT ( "_" ), wxEmptyString );
	stripedText.Replace ( wxT ( "&" ), wxEmptyString );
	return stripedText.BeforeFirst ( wxT ( '\t' ) );
}

/**
 * \brief remove any accelerator key from the menu text and return only
 * the text including any mnemonics
 * \param text
 * \return
 */
static wxString StripAccel ( const wxString &text )
{
	return text.BeforeFirst ( wxT ( '\t' ) );
}

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

Manager::Manager ( void )
		: m_shellProcess ( NULL )
		, m_asyncExeCmd ( NULL )
		, m_dbgCanInteract ( false )
		, m_quickWatchDlg ( NULL )
		, m_frameLineno ( wxNOT_FOUND )
		, m_useTipWin ( false )
		, m_tipWinPos ( wxNOT_FOUND )
		, m_workspceClosing ( false )
		, m_isShutdown ( false )
{
	m_breakptsmgr = new BreakptMgr;
}

Manager::~Manager ( void )
{
	UnInitialize();
	if ( m_shellProcess ) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}
	delete m_breakptsmgr;
}

wxFrame *Manager::GetMainFrame()
{
	return Frame::Get();
}

BreakptMgr* Manager::GetBreakpointsMgr()
{
	return m_breakptsmgr;
}

bool Manager::IsWorkspaceOpen() const
{
	return WorkspaceST::Get()->GetName().IsEmpty() == false;
}

bool Manager::OpenFile ( const BrowseRecord &rec )
{
	return Frame::Get()->GetMainBook()->OpenFile(rec) != NULL;
}

bool Manager::OpenFile ( const wxString &file_name, const wxString &projectName, int lineno, long position )
{
	return Frame::Get()->GetMainBook()->OpenFile(file_name, projectName, lineno, position) != NULL;
}

void Manager::UnInitialize()
{
	//stop background processes
	DbgStop();
	JobQueueSingleton::Instance()->Stop();
	ParseThreadST::Get()->Stop();
	SearchThreadST::Get()->Stop();

	//free all plugins
	PluginManager::Get()->UnLoad();

	// release singleton objects
	DebuggerMgr::Free();
	JobQueueSingleton::Release();
	ParseThreadST::Free();  //since the parser is making use of the TagsManager,
	TagsManagerST::Free();  //it is important to release it *before* the TagsManager
	LanguageST::Free();
	WorkspaceST::Free();
	ContextManager::Free();
	BuildManagerST::Free();
	BuildSettingsConfigST::Free();
	SearchThreadST::Free();
	MenuManager::Free();
	EditorConfigST::Free();
	EnvironmentConfig::Release();
}

void Manager::CreateWorkspace ( const wxString &name, const wxString &path )
{
	// make sure that the workspace pane is visible
	ShowWorkspacePane (Frame::Get()->GetWorkspaceTab()->GetCaption());

	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateWorkspace ( name, path, errMsg );
	CHECK_MSGBOX ( res );
	OpenWorkspace ( path + PATH_SEP + name + wxT ( ".workspace" ) );
}

void Manager::CreateProject ( ProjectData &data )
{
	if ( IsWorkspaceOpen() == false ) {
		//create a workspace before creating a project
		CreateWorkspace ( data.m_name, data.m_path );
	}

	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateProject ( data.m_name,
	           data.m_path,
	           data.m_srcProject->GetSettings()->GetProjectType ( wxEmptyString ),
	           false,
	           errMsg );
	CHECK_MSGBOX ( res );
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName ( data.m_name, errMsg );

	//copy the project settings to the new one
	proj->SetSettings ( data.m_srcProject->GetSettings() );

	// now add the new project to the build matrix
	WorkspaceST::Get()->AddProjectToBuildMatrix ( proj );
	ProjectSettingsPtr settings = proj->GetSettings();

	//set the compiler type
	ProjectSettingsCookie cookie;
	BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration ( cookie );
	while ( bldConf ) {
		bldConf->SetCompilerType ( data.m_cmpType );
		bldConf = settings->GetNextBuildConfiguration ( cookie );
	}
	proj->SetSettings ( settings );

	//copy the files as they appear in the source project
	proj->SetFiles ( data.m_srcProject );

	{
		// copy the actual files from the template directory to the new project path
		DirSaver ds;
		wxSetWorkingDirectory ( proj->GetFileName().GetPath() );

		// get list of files
		std::vector<wxFileName> files;
		data.m_srcProject->GetFiles ( files, true );
		for ( size_t i=0; i<files.size(); i++ ) {
			wxFileName f ( files.at ( i ) );
			wxCopyFile ( f.GetFullPath(), f.GetFullName() );
		}
	}

	wxString projectName = proj->GetName();
	RetagProject ( projectName );
	SendCmdEvent ( wxEVT_PROJ_ADDED, ( void* ) &projectName );
}

void Manager::CloseWorkspace()
{
	SetWorkspaceClosing ( true );

	//close any debugging session that is currently opened
	DbgStop();

	//save the current session before closing
	SessionEntry session;
	session.SetWorkspaceName ( WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath() );
	Frame::Get()->GetMainBook()->SaveSession(session);
	SessionManager::Get().Save ( WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath(), session );

	// since we closed the workspace, we also need to set the 'LastActiveWorkspaceName' to be
	// default
	SessionManager::Get().SetLastWorkspaceName ( wxT ( "Default" ) );

	WorkspaceST::Get()->CloseWorkspace();

	if ( !IsShutdownInProgress() ) {
		SendCmdEvent ( wxEVT_WORKSPACE_CLOSED );
	}

#ifdef __WXMSW__
	// Under Windows, and in order to avoid locking the directory set the working directory back to the start up directory
	wxSetWorkingDirectory ( GetStarupDirectory() );
#endif
}

void Manager::OpenWorkspace ( const wxString &path )
{
	CloseWorkspace();

	// make sure that the workspace pane is visible
	// ShowWorkspacePane(WorkspacePane::FILE_VIEW);
	wxString errMsg;
	bool res = WorkspaceST::Get()->OpenWorkspace ( path, errMsg );
	if ( !res ) {
		// in case part of the workspace was opened, close the workspace
		CloseWorkspace();
		wxMessageBox ( errMsg, wxT ( "Error" ), wxOK | wxICON_HAND );
		return;
	}

	// do workspace initializations
	DoSetupWorkspace ( path );
	SetWorkspaceClosing ( false );
}

ProjectTreePtr Manager::GetProjectFileViewTree ( const wxString &projectName )
{
	if ( !IsWorkspaceOpen() ) {
		return NULL;
	}

	wxString err_msg;
	ProjectPtr prj = WorkspaceST::Get()->FindProjectByName ( projectName, err_msg );
	if ( !prj ) {
		return NULL;
	}
	return prj->AsTree();
}

void Manager::GetProjectList ( wxArrayString &list )
{
	WorkspaceST::Get()->GetProjectList ( list );
}

void Manager::AddProject ( const wxString & path )
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddProject ( path, errMsg );
	CHECK_MSGBOX ( res );

	wxFileName fn ( path );
	wxString projectName ( fn.GetName() );
	RetagProject ( projectName );
	SendCmdEvent ( wxEVT_PROJ_ADDED, ( void* ) &projectName );
}

bool Manager::RemoveProject ( const wxString &name )
{
	if ( name.IsEmpty() ) {
		return false;
	}

	ProjectPtr proj = GetProject ( name );

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveProject ( name, errMsg );
	CHECK_MSGBOX_BOOL ( res );

	if ( proj ) {
		//remove symbols from the database
		std::vector<wxFileName> projectFiles;
		proj->GetFiles ( projectFiles, true );
		TagsManagerST::Get()->DeleteFilesTags ( projectFiles );
		wxArrayString prjfls;
		for ( size_t i = 0; i < projectFiles.size(); i++ ) {
			prjfls.Add ( projectFiles[i].GetFullPath() );
		}
		SendCmdEvent ( wxEVT_PROJ_FILE_REMOVED, ( void* ) &prjfls );
	} // if(proj)

	SendCmdEvent ( wxEVT_PROJ_REMOVED, ( void* ) &name );

	return true;
}

wxString Manager::GetActiveProjectName()
{
	return WorkspaceST::Get()->GetActiveProjectName();
}

void Manager::SetActiveProject ( const wxString &name )
{
	WorkspaceST::Get()->SetActiveProject ( WorkspaceST::Get()->GetActiveProjectName(), false );
	WorkspaceST::Get()->SetActiveProject ( name, true );
}


void Manager::AddVirtualDirectory ( const wxString &virtualDirFullPath )
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateVirtualDirectory ( virtualDirFullPath, errMsg );
	CHECK_MSGBOX ( res );
}

void Manager::RemoveVirtualDirectory ( const wxString &virtualDirFullPath )
{
	wxString errMsg;
	wxString project = virtualDirFullPath.BeforeFirst ( wxT ( ':' ) );
	ProjectPtr p = WorkspaceST::Get()->FindProjectByName ( project, errMsg );
	if ( !p ) {
		return;
	}

	// Update symbol tree and database
	wxString vdPath = virtualDirFullPath.AfterFirst ( wxT ( ':' ) );
	wxArrayString files;
	p->GetFilesByVirtualDir ( vdPath, files );
	for ( size_t i=0; i<files.Count(); i++ ) {
		RemoveFileFromSymbolTree ( files.Item ( i ), p->GetName() );
	}

	//and finally, remove the virtual dir from the workspace
	bool res = WorkspaceST::Get()->RemoveVirtualDirectory ( virtualDirFullPath, errMsg );
	CHECK_MSGBOX ( res );

	SendCmdEvent ( wxEVT_PROJ_FILE_REMOVED, ( void* ) &files );
}

void Manager::SaveWorkspace()
{
	WorkspaceST::Get()->Save();
}

bool Manager::AddNewFileToProject ( const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxFile file;
	if ( !file.Create ( fileName.GetData(), true ) )
		return false;

	if ( file.IsOpened() ) {
		file.Close();
	}

	return AddFileToProject ( fileName, vdFullPath, openIt );
}

bool Manager::AddFileToProject ( const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxString project;
	project = vdFullPath.BeforeFirst ( wxT ( ':' ) );

	// Add the file to the project
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddNewFile ( vdFullPath, fileName, errMsg );
	if ( !res ) {
		//file or virtual dir does not exist
		return false;
	}

	if ( openIt ) {
		OpenFile ( fileName, project );
	}

	TagTreePtr ttp;
	if ( project.IsEmpty() == false ) {
		std::vector<DbRecordPtr> comments;
		if ( TagsManagerST::Get()->GetParseComments() ) {
			ttp = TagsManagerST::Get()->ParseSourceFile ( fileName, &comments );
			TagsManagerST::Get()->StoreComments ( comments );
		} else {
			ttp = TagsManagerST::Get()->ParseSourceFile ( fileName );
		}
		TagsManagerST::Get()->Store ( ttp );
	}

	//send notification command event that files was added to
	//project
	wxArrayString files;
	files.Add ( fileName );
	SendCmdEvent ( wxEVT_PROJ_FILE_ADDED, ( void* ) &files );
	return true;
}

void Manager::AddFilesToProject ( const wxArrayString &files, const wxString &vdFullPath, wxArrayString &actualAdded )
{
	wxString project;
	project = vdFullPath.BeforeFirst ( wxT ( ':' ) );

	// Add the file to the project
	wxString errMsg;
	//bool res = true;
	size_t i=0;

	//try to find this file in the workspace
	for ( i=0; i<files.GetCount(); i++ ) {
		wxString projName = this->GetProjectNameByFile ( files.Item ( i ) );
		//allow adding the file, only if it does not already exist under the current project
		//(it can be already exist under the different project)
		if ( projName.IsEmpty() || projName != project ) {
			actualAdded.Add ( files.Item ( i ) );
		}
	}

	for ( i=0; i<actualAdded.GetCount(); i++ ) {
		Workspace *wsp = WorkspaceST::Get();
		wsp->AddNewFile ( vdFullPath, actualAdded.Item ( i ), errMsg );
	}

	//convert wxArrayString to vector for the ctags api
	std::vector<wxFileName> vFiles;
	for ( size_t i=0; i<actualAdded.GetCount(); i++ ) {
		vFiles.push_back ( actualAdded.Item ( i ) );
	}

	//re-tag the added files
	if ( vFiles.empty() == false ) {
		TagsManagerST::Get()->RetagFiles ( vFiles );
	}

	if ( !actualAdded.IsEmpty() ) {
		SendCmdEvent ( wxEVT_PROJ_FILE_ADDED, ( void* ) &actualAdded );
	}
}

bool Manager::RemoveFile ( const wxString &fileName, const wxString &vdFullPath )
{
	wxString project = vdFullPath.BeforeFirst ( wxT ( ':' ) );
	wxFileName absPath ( fileName );
	absPath.MakeAbsolute ( GetProjectCwd ( project ) );

	Frame::Get()->GetMainBook()->ClosePage(absPath.GetFullPath());

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveFile ( vdFullPath, fileName, errMsg );
	CHECK_MSGBOX_BOOL ( res );

	RemoveFileFromSymbolTree ( absPath, project );
	wxArrayString files(1, &fileName);
	SendCmdEvent(wxEVT_PROJ_FILE_REMOVED, (void*)&files);

	return true;
}

void Manager::RemoveFileFromSymbolTree ( const wxFileName &fileName, const wxString &project )
{
	if ( project.IsEmpty() == false ) {
		// Remove the file from the tags database as well
		TagsManagerST::Get()->Delete ( TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName(), fileName.GetFullPath() );
	}
}

wxString Manager::GetProjectCwd ( const wxString &project ) const
{
	wxString errMsg;
	ProjectPtr p = WorkspaceST::Get()->FindProjectByName ( project, errMsg );
	if ( !p ) {
		return wxGetCwd();
	}

	wxFileName projectFileName ( p->GetFileName() );
	projectFileName.MakeAbsolute();
	return projectFileName.GetPath();
}

bool Manager::ShowOutputPane ( wxString focusWin, bool commit )
{
	// make the output pane visible
	bool showedIt ( false );

	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( wxT ( "Output View" ) );
	if ( info.IsOk() && !info.IsShown() ) {
		info.Show();
		showedIt = true;
		if ( commit ) {
			Frame::Get()->GetDockingManager().Update();
		}
	}

	// set the selection to focus win
	OutputPane *pane = Frame::Get()->GetOutputPane();
	int index = pane->GetNotebook()->GetPageIndex( focusWin );
	if ( index != wxNOT_FOUND && ( size_t ) index != pane->GetNotebook()->GetSelection() ) {
        wxWindow *focus = wxWindow::FindFocus();
		pane->GetNotebook()->SetSelection ( ( size_t ) index );
        if (focus) {
            focus->SetFocus();
        }
	}
	return showedIt;
}

void Manager::DoShowPane ( const wxString& pane )
{
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( pane );
	if ( info.IsOk() && !info.IsShown() ) {
		info.Show();
	}
}

void Manager::ShowDebuggerPane ( bool show )
{
	// make the output pane visible
	wxArrayString dbgPanes;
	dbgPanes.Add ( wxT ( "Debugger" ) );
	dbgPanes.Add ( DebuggerPane::LOCALS );
	dbgPanes.Add ( DebuggerPane::FRAMES );
	dbgPanes.Add ( DebuggerPane::WATCHES );
	dbgPanes.Add ( DebuggerPane::BREAKPOINTS );
	dbgPanes.Add ( DebuggerPane::THREADS );
	dbgPanes.Add ( DebuggerPane::MEMORY );

	if ( show ) {

		for ( size_t i=0; i<dbgPanes.GetCount(); i++ ) {
			wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( dbgPanes.Item ( i ) );
			// show all debugger related panes
			if ( info.IsOk() && !info.IsShown() ) {
				info.Show();
			}
		}

	} else {

		// hide all debugger related panes
		for ( size_t i=0; i<dbgPanes.GetCount(); i++ ) {
			wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( dbgPanes.Item ( i ) );
			// show all debugger related panes
			if ( info.IsOk() && info.IsShown() ) {
				info.Hide();
			}
		}
	}
	Frame::Get()->GetDockingManager().Update();

}

void Manager::HidePane ( const wxString &paneName, bool commit )
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( paneName );
	if ( info.IsOk() && info.IsShown() ) {
		info.Hide();
		if ( commit ) {
			Frame::Get()->GetDockingManager().Update();
		}
	}
}

void Manager::ShowWorkspacePane ( wxString focusWin, bool commit )
{
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( wxT ( "Workspace View" ) );
	if ( info.IsOk() && !info.IsShown() ) {
		info.Show();
		if ( commit ) {
			Frame::Get()->GetDockingManager().Update();
		}
	}

	// set the selection to focus win
	Notebook *book = Frame::Get()->GetWorkspacePane()->GetNotebook();
	int index = book->GetPageIndex ( focusWin );
	if ( index != wxNOT_FOUND && ( size_t ) index != book->GetSelection() ) {
		book->SetSelection ( ( size_t ) index );
	}
}

ProjectSettingsPtr Manager::GetProjectSettings ( const wxString &projectName ) const
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName ( projectName, errMsg );
	if ( !proj ) {
		wxLogMessage ( errMsg );
		return NULL;
	}

	return proj->GetSettings();
}

void Manager::SetProjectSettings ( const wxString &projectName, ProjectSettingsPtr settings )
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName ( projectName, errMsg );
	if ( !proj ) {
		wxLogMessage ( errMsg );
		return;
	}

	proj->SetSettings ( settings );
}

BuildMatrixPtr Manager::GetWorkspaceBuildMatrix() const
{
	return WorkspaceST::Get()->GetBuildMatrix();
}

void Manager::SetWorkspaceBuildMatrix ( BuildMatrixPtr matrix )
{
	WorkspaceST::Get()->SetBuildMatrix ( matrix );
	SendCmdEvent(wxEVT_WORKSPACE_CONFIG_CHANGED);
}

void Manager::TogglePanes()
{
	static bool toggled = false;
	// list of panes to be toggled on and off
	static wxArrayString panes;

	Frame::Get()->Freeze();
	if ( !toggled ) {
		panes.Clear();
		// create the list of panes to be tested
		wxArrayString candidates;
		candidates.Add ( wxT ( "Output View" ) );
		candidates.Add ( wxT ( "Workspace View" ) );
		candidates.Add ( wxT ( "Debugger" ) );

		// add the detached tabs list
		wxArrayString dynamicPanes = Frame::Get()->GetDockablePaneMenuManager()->GetDeatchedPanesList();
		for ( size_t i=0; i<dynamicPanes.GetCount(); i++ ) {
			candidates.Add ( dynamicPanes.Item ( i ) );
		}

		for ( size_t i=0; i<candidates.GetCount(); i++ ) {
			wxAuiPaneInfo info;
			info = Frame::Get()->GetDockingManager().GetPane ( candidates.Item ( i ) );
			if ( info.IsOk() && info.IsShown() ) {
				panes.Add ( candidates.Item ( i ) );
			}
		}

		// hide the matched panes
		for ( size_t i=0; i<panes.GetCount(); i++ ) {
			HidePane ( panes.Item ( i ), false );
		}

		//update changes
		Frame::Get()->GetDockingManager().Update();
		toggled = true;

	} else {

		for ( size_t i=0; i<panes.GetCount(); i++ ) {
			wxString pane_name = panes.Item ( i );
			if ( pane_name == wxT ( "Output View" ) ) {
				ShowOutputPane ( wxEmptyString, false );
				continue;
			}
			if ( pane_name == wxT ( "Workspace View" ) ) {
				ShowWorkspacePane ( wxEmptyString, false );
				continue;
			}

			DoShowPane ( pane_name );
		}

		Frame::Get()->GetDockingManager().Update();
		toggled = false;
	}
	Frame::Get()->Thaw();
}

bool Manager::CreateDefaultNewCompiler ( const wxString &name )
{
	if ( BuildSettingsConfigST::Get()->IsCompilerExist ( name ) ) {
		wxMessageBox ( _ ( "A compiler with this name already exist" ), wxT ( "Error" ), wxOK | wxICON_HAND );
		return false;
	}

	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler ( name );
	cmp->SetName ( name );
	BuildSettingsConfigST::Get()->SetCompiler ( cmp );
	return true;
}

bool Manager::DeleteCompiler ( const wxString &name )
{
	if ( wxMessageBox ( _ ( "Remove Compiler?" ), wxT ( "Confirm" ), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
		BuildSettingsConfigST::Get()->DeleteCompiler ( name );
		return true;
	}
	return false;
}

void Manager::GetProjectTemplateList ( std::list<ProjectPtr> &list )
{
	wxString tmplateDir = m_startupDir + PATH_SEP + wxT ( "templates/projects" );

	//read all files under this directory
	DirTraverser dt ( wxT ( "*.project" ) );

	wxDir dir ( tmplateDir );
	dir.Traverse ( dt );

	wxArrayString &files = dt.GetFiles();

	if ( files.GetCount() > 0 ) {
		for ( size_t i=0; i<files.GetCount(); i++ ) {
			ProjectPtr proj ( new Project() );
			if ( !proj->Load ( files.Item ( i ) ) ) {
				//corrupted xml file?
				wxLogMessage ( wxT ( "Failed to load template project: " ) + files.Item ( i ) + wxT ( " (corrupted XML?)" ) );
				continue;
			}
			list.push_back ( proj );
		}
	} else {
		//if we ended up here, it means the installation got screwed up since
		//there should be at least 8 project templates !
		//create 3 default empty projects
		ProjectPtr exeProj ( new Project() );
		ProjectPtr libProj ( new Project() );
		ProjectPtr dllProj ( new Project() );
		libProj->Create ( wxT ( "Static Library" ), wxEmptyString, tmplateDir, Project::STATIC_LIBRARY );
		dllProj->Create ( wxT ( "Dynamic Library" ), wxEmptyString, tmplateDir, Project::DYNAMIC_LIBRARY );
		exeProj->Create ( wxT ( "Executable" ), wxEmptyString, tmplateDir, Project::EXECUTABLE );
		list.push_back ( libProj );
		list.push_back ( dllProj );
		list.push_back ( exeProj );
	}
}

void Manager::SaveProjectTemplate ( ProjectPtr proj, const wxString &name, const wxString &description )
{
	// create new project
	wxString tmplateDir = GetStarupDirectory() + wxT ( "/templates/projects/" ) + name + wxT ( "/" );

	// create the directory
	Mkdir ( tmplateDir );

	// copy the project to the new location
	proj->CopyTo ( tmplateDir, name, description );
}

ProjectPtr Manager::GetProject ( const wxString &name ) const
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName ( name, errMsg );
	if ( !proj ) {
		wxLogMessage ( errMsg );
		return NULL;
	}
	return proj;
}

void Manager::PopupProjectDependsDlg ( const wxString &projectName )
{
	DependenciesDlg *dlg = new DependenciesDlg ( GetMainFrame(), projectName );
	dlg->ShowModal();
	dlg->Destroy();
}

bool Manager::IsBuildEndedSuccessfully() const
{
	//build is still running?
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		return false;
	}

	wxArrayString lines;
	CompileRequest *cr = dynamic_cast<CompileRequest*> ( m_shellProcess );
	if ( cr ) {
		if ( !cr->GetLines ( lines ) ) {
			return false;
		}

		//check every line to see if we got an error/warning
		wxString project ( cr->GetProjectName() );

		// TODO :: change the call to ' GetProjBuildConf' to pass the correct
		// build configuration
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( project, wxEmptyString );
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler ( bldConf->GetCompilerType() );
		wxString errPattern = cmp->GetErrPattern();
		wxString warnPattern = cmp->GetWarnPattern();

		wxRegEx reErr ( errPattern );
		wxRegEx reWarn ( warnPattern );
		for ( size_t i=0; i<lines.GetCount(); i++ ) {
			if ( reWarn.IsValid() && reWarn.Matches ( lines.Item ( i ) ) ) {
				//skip warnings
				continue;
			}

			if ( reErr.IsValid() && reErr.Matches ( lines.Item ( i ) ) ) {
				return false;
			}
		}
	}
	return true;
}

void Manager::CompileFile ( const wxString &projectName, const wxString &fileName, bool preprocessOnly )
{
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		return;
	}

	if ( m_shellProcess ) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}

	//save all files before compiling, but dont saved new documents
	if (!Frame::Get()->GetMainBook()->SaveAll(false, false))
		return;

	//If a debug session is running, stop it.
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() ) {
		if ( wxMessageBox ( _ ( "This would terminate the current debug session, continue?" ),  wxT ( "Confirm" ), wxICON_QUESTION|wxYES_NO|wxCANCEL ) != wxYES ) {
			return;
		} else {
			DbgStop();
		}
	}

	wxString conf;
	// get the selected configuration to be built
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( projectName, wxEmptyString );
	if ( bldConf ) {
		conf = bldConf->GetName();
	}

	QueueCommand info ( projectName, conf, false, QueueCommand::Build );
	if ( bldConf && bldConf->IsCustomBuild() ) {
		info.SetCustomBuildTarget ( preprocessOnly ? wxT("Preprocess File") : wxT ( "Compile Single File" ) );
		info.SetKind ( QueueCommand::CustomBuild );
	}

	switch ( info.GetKind() ) {
        case QueueCommand::Build:
            m_shellProcess = new CompileRequest ( GetMainFrame(), info, fileName, false, preprocessOnly );
            break;
        case  QueueCommand::CustomBuild:
            m_shellProcess = new CustomBuildRequest ( GetMainFrame(), info, fileName );
            break;
	}
	m_shellProcess->Process();
}

void Manager::StopBuild()
{
	if ( m_shellProcess && !m_shellProcess->IsBusy() ) {
		return;
	}

	if ( m_shellProcess ) {
		m_shellProcess->Stop();
	}

	// clear the build queue as well
	this->m_buildQueue.clear();
}

bool Manager::IsBuildInProgress() const
{
	return m_shellProcess && m_shellProcess->IsBusy();
}

bool Manager::IsProgramRunning() const
{
	return ( m_asyncExeCmd && m_asyncExeCmd->IsBusy() );
}

void Manager::DebugMessage ( wxString msg )
{
	Frame::Get()->GetOutputPane()->GetDebugWindow()->AppendLine(msg);
}

void Manager::ExecuteNoDebug ( const wxString &projectName )
{
	//an instance is already running
	if ( m_asyncExeCmd && m_asyncExeCmd->IsBusy() ) {
		return;
	}
	wxString wd;

	// we call it here once for the 'wd'
	wxString execLine = GetProjectExecutionCommand ( projectName, wd, true );
	ProjectPtr proj = GetProject ( projectName );

	DirSaver ds;

	//print the current directory
	::wxSetWorkingDirectory ( proj->GetFileName().GetPath() );

	//now set the working directory according to working directory field from the
	//project settings
	::wxSetWorkingDirectory ( wd );

	//execute the command line
	//the a sync command is a one time executable object,
	m_asyncExeCmd = new AsyncExeCmd (Frame::Get()->GetOutputPane()->GetOutputWindow());

	//execute the program:
	//- no hiding the console
	//- no redirection of the stdin/out
	EnvironmentConfig::Instance()->ApplyEnv ( NULL );

	// call it again here to get the actual exection line - we do it here since
	// the environment has been applied
	execLine = GetProjectExecutionCommand ( projectName, wd, true );
	m_asyncExeCmd->Execute ( execLine, false, false );

	if ( m_asyncExeCmd->GetProcess() ) {
		m_asyncExeCmd->GetProcess()->Connect ( wxEVT_END_PROCESS, wxProcessEventHandler ( Manager::OnProcessEnd ), NULL, this );
	}
	EnvironmentConfig::Instance()->UnApplyEnv();
}

void Manager::OnProcessEnd ( wxProcessEvent &event )
{
	m_asyncExeCmd->ProcessEnd ( event );
	m_asyncExeCmd->GetProcess()->Disconnect ( wxEVT_END_PROCESS, wxProcessEventHandler ( Manager::OnProcessEnd ), NULL, this );
	delete m_asyncExeCmd;
	m_asyncExeCmd = NULL;

	//unset the environment variables
	EnvironmentConfig::Instance()->UnApplyEnv();

	//return the focus back to the editor
	if ( Frame::Get()->GetMainBook()->GetActiveEditor() ) {
		Frame::Get()->GetMainBook()->GetActiveEditor()->SetActive();
	}
}

void Manager::ShowMainToolbar ( bool show )
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( wxT ( "Standard Toolbar" ) );
	if ( info.IsOk() && show && !info.IsShown() ) {
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	} else if ( info.IsOk() && !show && info.IsShown() ) {
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}
}

BuildConfigPtr Manager::GetActiveProjectBuildConf()
{
	BuildMatrixPtr matrix = GetWorkspaceBuildMatrix();
	wxString projConf = matrix->GetProjectSelectedConf ( matrix->GetSelectedConfigurationName(), GetActiveProjectName() );
	ProjectSettingsPtr settings = GetProjectSettings ( GetActiveProjectName() );
	return settings->GetBuildConfiguration ( projConf );
}

void Manager::SetExternalDatabase ( const wxFileName &dbname )
{
	//close any opened external database
	CloseExternalDatabase();

	// build the external database
	TagsManagerST::Get()->OpenExternalDatabase ( dbname );

	if ( TagsManagerST::Get()->GetExtDatabase()->IsOpen() ) {
		Frame::Get()->SetStatusMessage ( wxString::Format ( wxT ( "External DB: '%s'" ), dbname.GetFullName().GetData() ), 1 );
		EditorConfigST::Get()->SetTagsDatabase ( dbname.GetFullPath() );
	}
}

void Manager::GetWorkspaceFiles ( std::vector<wxFileName> &files, bool absPath )
{
	wxArrayString projects;
	GetProjectList ( projects );
	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		ProjectPtr p = GetProject ( projects.Item ( i ) );
		p->GetFiles ( files, absPath );
	}
}

bool Manager::IsFileInWorkspace ( const wxString &fileName )
{
	wxFileName findme ( fileName );
	std::vector<wxFileName> files;

	GetWorkspaceFiles ( files, true );
	std::vector<wxFileName>::const_iterator iter = std::find ( files.begin(), files.end(), findme );
	return iter != files.end();
}

void Manager::RetagProject ( const wxString &projectName )
{
	//get the project file list
	ProjectPtr proj = GetProject ( projectName );
	if ( proj ) {
		//change the directory to the project dir
		std::vector<wxFileName> projectFiles;
		proj->GetFiles ( projectFiles, true );

		//call tags manager for re-tagging
		TagsManagerST::Get()->RetagFiles ( projectFiles );
		SendCmdEvent ( wxEVT_FILE_RETAGGED, ( void* ) &projectFiles );
	}
}

void Manager::RetagWorkspace()
{
	wxArrayString projects;
	GetProjectList ( projects );
	std::vector<wxFileName> projectFiles;

	// in the case of re-tagging the entire workspace
	// it is faster to drop the tables instead of deleting
	TagsManagerST::Get()->GetDatabase()->RecreateDatabase();

	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		ProjectPtr proj = GetProject ( projects.Item ( i ) );
		if ( proj ) {
			//change the directory to the project dir
			proj->GetFiles ( projectFiles, true );
		}
	}
	//call tags manager for re-tagging
	TagsManagerST::Get()->RetagFiles ( projectFiles );
	SendCmdEvent ( wxEVT_FILE_RETAGGED, ( void* ) &projectFiles );
}

void Manager::KillProgram()
{
	if ( !IsProgramRunning() )
		return;

	m_asyncExeCmd->Terminate();
}

wxString Manager::GetProjectNameByFile ( const wxString &fullPathFileName )
{
	wxArrayString projects;
	GetProjectList ( projects );

	std::vector<wxFileName> files;
	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		files.clear();
		ProjectPtr proj = GetProject ( projects.Item ( i ) );
		proj->GetFiles ( files, true );

		for ( size_t xx=0; xx<files.size(); xx++ ) {
			if ( files.at ( xx ).GetFullPath() == fullPathFileName ) {
				return proj->GetName();
			}
		}
	}

	return wxEmptyString;
}

/*
void Manager::ImportFromMakefile(const wxString &path)
{
	OutputMessage(path + wxT("\n"));

	wxFileName fileName = path;
	OutputMessage(fileName.GetPath() + wxT("\n"));

	VariableLexer expander(path);
	wxArrayString expanded = expander.getResult();

	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();

	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	CreateWorkspace(wxT("import_from_") + fileName.GetName(), fileName.GetPath());

	wxArrayString extentions;
	extentions.Add(wxT(".h"));
	extentions.Add(wxT(".hpp"));
	extentions.Add(wxT(".c"));
	extentions.Add(wxT(".cc"));
	extentions.Add(wxT(".cpp"));
	extentions.Add(wxT(".cxx"));

	for (size_t i = 0; i < lexed.size(); i++) {
		Target targ = lexed[i];
		wxArrayString deps = targ.getDeps();

		ProjectPtr proj(new Project());
		proj->Create(targ.getName(), fileName.GetPath(), wxT("importedProject"));
		proj->SetSettings(new ProjectSettings(NULL));

		for (size_t j = 0; j < deps.size(); j++) {
			wxString dep = deps[j];
			if (dep.Right(2) == wxT(".o")) { // string ends with .o!
				wxString file = dep.Left(dep.size()-2);

				for (size_t k = 0; k < extentions.size(); k++) {
					wxString ext = extentions[k];
					wxFileName fileName = proj->GetFileName().GetPathWithSep() + file + ext;
					if (fileName.FileExists()) {
						bool added = proj->AddFile(fileName.GetFullPath(), wxT("Source Files"));
						if (!added) {
							DebugMessage(wxT("WHOOPS WRONG BAD NOT GOOD!\n"));
						}
					}
				}
			} else if (deps.Index(dep, false) != wxNOT_FOUND) {
				wxArrayString dependencies= proj->GetDependencies();
				dependencies.Add(dep);
				proj->SetDependencies(dependencies);
			}
		}
		proj->Save();
		//AddProject(proj->GetFileName().GetFullPath());
	}
	return;
}
*/

LEditor *Manager::GetActiveEditor() const
{
	return Frame::Get()->GetMainBook()->GetActiveEditor();
}

void Manager::AddToRecentlyOpenedFiles ( const wxString &fileName )
{
	// Add this file to the history. Don't check for uniqueness:
	// if it's already on the list, wxFileHistory will move it to the top
	m_recentFiles.AddFileToHistory ( fileName );

	//sync between the history object and the configuration file
	wxArrayString files;
	m_recentFiles.GetFiles ( files );
	EditorConfigST::Get()->SetRecentlyOpenedFies ( files );
}

void Manager::AddToRecentlyOpenedWorkspaces ( const wxString &fileName )
{
	// Add this workspace to the history. Don't check for uniqueness:
	// if it's already on the list, wxFileHistory will move it to the top
	m_recentWorkspaces.AddFileToHistory ( fileName );

	//sync between the history object and the configuration file
	wxArrayString files;
	m_recentWorkspaces.GetFiles ( files );
	EditorConfigST::Get()->SetRecentlyOpenedWorkspaces ( files );
}

void Manager::GetRecentlyOpenedFiles ( wxArrayString &files )
{
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedFies ( files );
}

void Manager::GetRecentlyOpenedWorkspaces ( wxArrayString &files )
{
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedWorkspaces ( files );
}

void Manager::CloseExternalDatabase()
{
	TagsManager *mgr = TagsManagerST::Get();
	mgr->CloseExternalDatabase();
	//remove the entry from the status bar
	Frame::Get()->SetStatusMessage ( wxEmptyString, 1 );
}

bool Manager::MoveFileToVD ( const wxString &fileName, const wxString &srcVD, const wxString &targetVD )
{
	// to move the file between targets, we need to change the file path, we do this
	// by changing the file to be in absolute path related to the src's project
	// and then making it relative to the target's project
	wxString srcProject, targetProject;
	srcProject = srcVD.BeforeFirst ( wxT ( ':' ) );
	targetProject = targetVD.BeforeFirst ( wxT ( ':' ) );
	wxFileName srcProjWd ( GetProjectCwd ( srcProject ), wxEmptyString );

	//set a dir saver point
	wxFileName fn ( fileName );
	wxArrayString files(1, fn.GetFullPath());

	//remove the file from the source project
	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveFile ( srcVD, fileName, errMsg );
	CHECK_MSGBOX_BOOL ( res );
	SendCmdEvent(wxEVT_PROJ_FILE_REMOVED, (void*) &files);

	// Add the file to the project
	res = WorkspaceST::Get()->AddNewFile ( targetVD, fn.GetFullPath(), errMsg );
	if ( !res ) {
		//file or virtual dir does not exist
		return false;
	}
	SendCmdEvent(wxEVT_PROJ_FILE_ADDED, (void*) &files);
	return true;
}

void Manager::GetProjectFiles ( const wxString &project, wxArrayString &files )
{
	std::vector<wxFileName> fileNames;
	ProjectPtr p = GetProject ( project );

	if ( p ) {
		p->GetFiles ( fileNames, true );

		//convert std::vector to wxArrayString
		for ( std::vector<wxFileName>::iterator it = fileNames.begin(); it != fileNames.end(); it ++ ) {
			files.Add ( ( *it ).GetFullPath() );
		}
	}
}

void Manager::GetWorkspaceFiles ( wxArrayString &files )
{
	if ( !IsWorkspaceOpen() ) {
		return;
	}

	wxArrayString projects;
	GetProjectList ( projects );

	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		GetProjectFiles ( projects.Item ( i ), files );
	}
}

void Manager::ImportMSVSSolution ( const wxString &path )
{
	wxFileName fn ( path );
	if ( fn.FileExists() == false ) {
		return;
	}
	wxString errMsg;
	VcImporter importer ( path );
	if ( importer.Import ( errMsg ) ) {
		wxString wspfile;
		wspfile << fn.GetPath() << wxT ( "/" ) << fn.GetName() << wxT ( ".workspace" );
		OpenWorkspace ( wspfile );
	}
}

wxString Manager::ExpandVariables ( const wxString &expression, ProjectPtr proj )
{
	wxString project_name ( proj->GetName() );
	wxString fileName;
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	if ( editor ) {
		fileName = editor->GetFileName().GetFullPath();
	}
	return ExpandAllVariables ( expression, WorkspaceST::Get(), project_name, wxEmptyString, fileName );
}

//--------------------------- Debugger API -----------------------------

void Manager::DbgStart ( long pid )
{
	//set the working directory to the project directory
	DirSaver ds;
	wxString errMsg;
	wxString output;
	wxString debuggerName;
	wxString exepath;
	wxString wd;
	wxString args;
	BuildConfigPtr bldConf;
	ProjectPtr proj;
	long PID ( -1 );

#if defined(__WXGTK__)
	wxString where;
	if ( !ExeLocator::Locate ( wxT ( "xterm" ), where ) ) {
		wxMessageBox ( _ ( "Failed to locate 'xterm' application required by CodeLite, please install it and try again!" ), wxT ( "CodeLite" ), wxOK|wxCENTER|wxICON_WARNING, Frame::Get() );
		return;
	}
#endif

	if ( pid == 1 ) { //attach to process
		AttachDbgProcDlg *dlg = new AttachDbgProcDlg ( NULL );
		if ( dlg->ShowModal() == wxID_OK ) {
			wxString processId = dlg->GetProcessId();
			exepath   = dlg->GetExeName();
			debuggerName = dlg->GetDebugger();
			DebuggerMgr::Get().SetActiveDebugger ( debuggerName );

			processId.ToLong ( &PID );
			if ( exepath.IsEmpty() == false ) {
				wxFileName fn ( exepath );
				wxSetWorkingDirectory ( fn.GetPath() );
				exepath = fn.GetFullName();

			}
			dlg->Destroy();
		} else {
			dlg->Destroy();
			return;
		}
	}

	if ( pid == wxNOT_FOUND ) {
		//need to debug the current project
		proj = WorkspaceST::Get()->FindProjectByName ( GetActiveProjectName(), errMsg );
		if ( proj ) {
			wxSetWorkingDirectory ( proj->GetFileName().GetPath() );
			bldConf = WorkspaceST::Get()->GetProjBuildConf ( proj->GetName(), wxEmptyString );
			if ( bldConf ) {
				debuggerName = bldConf->GetDebuggerType();
				DebuggerMgr::Get().SetActiveDebugger ( debuggerName );
			}
		}
	}

	//make sure we have an active debugger
	IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();
	if ( !dbgr ) {
		//No debugger available,
		wxString message;
		message << wxT ( "Failed to launch debugger '" ) << debuggerName << wxT ( "': debugger not loaded\n" );
		message << wxT ( "Make sure that you have an open workspace and that the active project is of type 'Executable'" );
		wxMessageBox ( message, wxT ( "CodeLite" ), wxOK|wxICON_WARNING );
		return;
	}

	if ( dbgr->IsRunning() ) {
		//debugger is already running, so issue a 'cont' command
		dbgr->Continue();
		dbgr->QueryFileLine();
		return;
	}

	//set the debugger information
	DebuggerInformation dinfo;
	DebuggerMgr::Get().GetDebuggerInformation ( debuggerName, dinfo );

	// if user override the debugger path, apply it
	if ( bldConf ) {
		wxString userDebuggr = bldConf->GetDebuggerPath();
		userDebuggr.Trim().Trim ( false );
		if ( userDebuggr.IsEmpty() == false ) {
			dinfo.path = userDebuggr;
		}
	}
	dbgr->SetDebuggerInformation ( dinfo );

	if ( pid == wxNOT_FOUND ) {
		exepath = bldConf->GetCommand();
		args = bldConf->GetCommandArguments();
		exepath.Prepend ( wxT ( "\"" ) );
		exepath.Append ( wxT ( "\"" ) );
		wd = bldConf->GetWorkingDirectory();

		// Expand variables before passing them to the debugger
		wd = ExpandVariables ( wd, proj );
		exepath = ExpandVariables ( exepath, proj );
	}

	//get the debugger path to execute
	DebuggerInformation dbginfo;
	DebuggerMgr::Get().GetDebuggerInformation ( debuggerName, dbginfo );

	// if user override the debugger path, apply it
	if ( bldConf ) {
		wxString userDebuggr = bldConf->GetDebuggerPath();
		userDebuggr.Trim().Trim ( false );
		if ( userDebuggr.IsEmpty() == false ) {
			dbginfo.path = userDebuggr;
		}
	}

	wxString dbgname = dbginfo.path;
	dbgname = EnvironmentConfig::Instance()->ExpandVariables ( dbgname );

	//set ourselves as the observer for the debugger class
	dbgr->SetObserver ( this );

	// Loop through the open editors and let each editor
	// a chance to update the debugger manager with any line
	// changes (i.e. file was edited and breakpoints were moved)
	// this loop must take place before the debugger start up
	// or else call to UpdateBreakpoint() will yield an attempt to
	// actually add the breakpoint before Run() is called - this can
	// be a problem when adding breakpoint to dll files.
	if ( wxNOT_FOUND == pid ) {
		Frame::Get()->GetMainBook()->UpdateBreakpoints();
	}

	//We can now get all the gathered breakpoints from the manager
	std::vector<BreakpointInfo> bps;

	// since files may have been updated and the breakpoints may have been moved,
	// delete all the information
	GetBreakpointsMgr()->GetBreakpoints ( bps );

	// read
	wxArrayString dbg_cmds;
	if ( pid == wxNOT_FOUND ) {
		//it is now OK to start the debugger...
		dbg_cmds = wxStringTokenize ( bldConf->GetDebuggerStartupCmds(), wxT ( "\n" ), wxTOKEN_STRTOK );
		if ( !dbgr->Start ( dbgname, exepath, wd, bps, dbg_cmds ) ) {
			wxString errMsg;
			errMsg << _ ( "Failed to initialize debugger: " ) << dbgname << _ ( "\n" );
			DebugMessage ( errMsg );
			return;
		}
		m_dbgWaitingFirstBp = true; // This doesn't seem to be used anywhere :/
	} else {
		//Attach to process...
		if ( !dbgr->Start ( dbgname, exepath, PID, bps, dbg_cmds ) ) {
			wxString errMsg;
			errMsg << _ ( "Failed to initialize debugger: " ) << dbgname << _ ( "\n" );
			DebugMessage ( errMsg );
			return;
		}
	}

	// Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
	// so that it uses debugger_ids instead of internal_ids
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

	// let the active editor get the focus
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	if ( editor ) {
		editor->SetActive();
	}

	// mark that we are waiting for the first GotControl()
	DebugMessage ( output );
	DebugMessage ( _ ( "Debug session started successfully!\n" ) );

	// set the debug tab as active
	ShowOutputPane(OutputPane::OUTPUT_DEBUG);

	if ( bldConf && bldConf->GetIsDbgRemoteTarget() && pid == wxNOT_FOUND ) {

		// debugging remote target
		wxString comm;
		wxString port = bldConf->GetDbgHostPort();
		wxString host = bldConf->GetDbgHostName();

		comm << host;

		host = host.Trim().Trim ( false );
		port = port.Trim().Trim ( false );

		if ( port.IsEmpty() == false ) {
			comm << wxT ( ":" ) << port;
		}

		dbgr->Run ( args, comm );

	} else if ( pid == wxNOT_FOUND ) {

		// debugging local target
		dbgr->Run ( args, wxEmptyString );
	}

	// and finally make the debugger pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( wxT ( "Debugger" ) );
	if ( info.IsOk() && !info.IsShown() ) {
		HideDebuggerPane = true;
		ShowDebuggerPane ( true );
	}
}

void Manager::DbgStop()
{
	if ( m_quickWatchDlg ) {
		m_quickWatchDlg->Destroy();
		m_quickWatchDlg = NULL;
	}

	// remove all debugger markers
	DbgUnMarkDebuggerLine();

	// Mark the debugger as non interactive
	m_dbgCanInteract = false;

	//clear the debugger pane
	Frame::Get()->GetDebuggerPane()->Clear();

	// The list of breakpoints is still there, but their debugger_ids are now invalid
	// So remove them. Then reInitialise the breakpt view, to use internal_ids
	GetBreakpointsMgr()->ClearBP_debugger_ids();
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

	// update toolbar state
	UpdateStopped();

	// and finally, hide the debugger pane (if we caused it to appear)
	if ( HideDebuggerPane ) {
		HideDebuggerPane = false;
		ShowDebuggerPane ( false );
	}

	IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();
	if ( !dbgr ) {
		return;
	}

	if ( !dbgr->IsRunning() ) {
		return;
	}

	dbgr->Stop();
	DebuggerMgr::Get().SetActiveDebugger ( wxEmptyString );
	DebugMessage ( _ ( "Debug session ended\n" ) );
}

void Manager::DbgMarkDebuggerLine ( const wxString &fileName, int lineno )
{
	DbgUnMarkDebuggerLine();
	if ( lineno < 0 ) {
		return;
	}

//    wxWindow *focusWin = wxWindow::FindFocus();

	//try to open the file
	wxFileName fn ( fileName );
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	if ( editor && editor->GetFileName().GetFullPath() == fn.GetFullPath() ) {
		editor->HighlightLine ( lineno );
		editor->GotoLine ( lineno-1 );
		editor->EnsureVisible ( lineno-1 );
	} else if (OpenFile ( fn.GetFullPath(), wxEmptyString, lineno-1, wxNOT_FOUND)) {
		editor = Frame::Get()->GetMainBook()->GetActiveEditor();
		if ( editor ) {
			editor->HighlightLine ( lineno );
		}
	}

//    if (focusWin) {
//        focusWin->SetFocus();
//    }
}

void Manager::DbgUnMarkDebuggerLine()
{
	//remove all debugger markers from all editors
	Frame::Get()->GetMainBook()->UnHighlightAll();
}

void Manager::DbgDoSimpleCommand ( int cmd )
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() ) {
		switch ( cmd ) {
		case DBG_NEXT:
			dbgr->Next();
			break;
		case DBG_STEPIN:
			dbgr->StepIn();
			break;
		case DBG_STEPOUT:
			dbgr->StepOut();
			break;
		case DBG_PAUSE:
			dbgr->Interrupt();
			break;
		case DBG_SHOW_CURSOR:
			dbgr->QueryFileLine();
			break;
		default:
			break;
		}
	}
}

void Manager::DbgQuickWatch ( const wxString &expression, bool useTipWin, long pos )
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() ) {
		m_useTipWin = useTipWin;
		m_tipWinPos = pos;
		dbgr->EvaluateExpressionToTree ( expression );
	}
}

void Manager::DbgCancelQuickWatchTip()
{
	/*
	if(m_debuggerTipWin){
		m_debuggerTipWin->Dismiss();
	}
	*/
}

//-------------------------------------------------
//Event handlers from the debugger
//-------------------------------------------------

void Manager::UpdateAddLine ( const wxString &line )
{
	DebugMessage ( line + wxT ( "\n" ) );
}

void Manager::UpdateFileLine ( const wxString &filename, int lineno )
{
	wxString fileName = filename;
	long lineNumber = lineno;
	if ( m_frameLineno != wxNOT_FOUND ) {
		lineNumber = m_frameLineno;
		m_frameLineno = wxNOT_FOUND;
	}

	DbgMarkDebuggerLine ( fileName, lineNumber );
	UpdateDebuggerPane();
}

void Manager::SetMemory ( const wxString& address, size_t count, const wxString &hex_value )
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {
		dbgr->SetMemory ( address, count, hex_value );
	}
}

void Manager::UpdateDebuggerPane()
{
	//Update the debugger pane
	DebuggerPane *pane = Frame::Get()->GetDebuggerPane();

	if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == ( wxWindow* ) pane->GetBreakpointView() ) ) {
		pane->GetBreakpointView()->Initialize();
	}

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {

		//--------------------------------------------------------------------
		// Lookup the selected tab in the debugger notebook and update it
		// once this is done, we need to go through the list of detached panes
		// and scan for another debugger tabs which are visible and need to be
		// updated
		//--------------------------------------------------------------------

		if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == pane->GetLocalsTree() ) || IsPaneVisible ( DebuggerPane::LOCALS ) ) {

			//update the locals tree
			dbgr->QueryLocals();

		}

		if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == pane->GetWatchesTable() ) || IsPaneVisible ( DebuggerPane::WATCHES ) ) {

			//update the watches table
			wxArrayString expressions = pane->GetWatchesTable()->GetExpressions();
			wxString format = pane->GetWatchesTable()->GetDisplayFormat();
			for ( size_t i=0; i<expressions.GetCount(); i++ ) {
				dbgr->EvaluateExpressionToString ( expressions.Item ( i ), format );
			}

		}
		if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == ( wxWindow* ) pane->GetFrameListView() ) || IsPaneVisible ( DebuggerPane::FRAMES ) ) {

			//update the stack call
			dbgr->ListFrames();

		}
		if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == ( wxWindow* ) pane->GetBreakpointView() ) || IsPaneVisible ( DebuggerPane::BREAKPOINTS ) ) {

			// update the breakpoint view
			pane->GetBreakpointView()->Initialize();

		}
		if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == ( wxWindow* ) pane->GetThreadsView() ) || IsPaneVisible ( DebuggerPane::THREADS ) ) {

			// update the thread list
			ThreadEntryArray threads;
			dbgr->ListThreads ( threads );
			pane->GetThreadsView()->PopulateList ( threads );

		}
		if ( ( IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == ( wxWindow* ) pane->GetMemoryView() ) || IsPaneVisible ( DebuggerPane::MEMORY ) ) {

			// Update the memory view tab
			MemoryView *memView = pane->GetMemoryView();
			if ( memView->GetExpression().IsEmpty() == false ) {

				wxString output;
				if ( dbgr->WatchMemory ( memView->GetExpression(), memView->GetSize(), output ) ) {
					memView->SetViewString ( output );
				}
			}

		}

	}
}


void Manager::UpdateLocals ( TreeNode<wxString, NodeData> *tree )
{
	NodeData data = tree->GetData();
	static TreeNode<wxString, NodeData> *thisTree ( NULL );
	static TreeNode<wxString, NodeData> *funcArgsTree ( NULL );

	if ( data.name == wxT ( "*this" ) ) {
		//append this tree to the local's tree
		thisTree = tree;//keep the tree and wait for the other call that will come
	} else if ( data.name == wxT ( "Function Arguments" ) ) {
		funcArgsTree = tree;//keep the tree and wait for the other call that will come
	} else {
		//if we already have thisTree, append it as child of this tree
		if ( funcArgsTree ) {
			tree->AddChild ( funcArgsTree );
			funcArgsTree = NULL;
		}

		if ( thisTree ) {
			tree->AddChild ( thisTree );
			thisTree = NULL;
		}
		Frame::Get()->GetDebuggerPane()->GetLocalsTree()->BuildTree ( tree );
	}
}

void Manager::UpdateStopped()
{
	//do something here....
}

void Manager::UpdateGotControl ( DebuggerReasons reason )
{
	//put us on top of the z-order window
	Frame::Get()->Raise();
	m_dbgCanInteract = true;

	switch ( reason ) {
	case DBG_RECV_SIGNAL_EXC_BAD_ACCESS:
	case DBG_RECV_SIGNAL_SIGSEGV: { //program received signal sigsegv
		wxString signame = wxT ( "SIGSEGV" );
		if ( reason == DBG_RECV_SIGNAL_EXC_BAD_ACCESS ) {
			signame = wxT ( "EXC_BAD_ACCESS" );
		}
		DebugMessage ( _ ( "Program Received signal " ) + signame + _ ( "\n" ) );
		wxMessageBox ( _ ( "Program Received signal " ) + signame + _ ( "\n" ) +
		               _ ( "Stack trace is available in the 'Stack' tab\n" ),
		               wxT ( "CodeLite" ), wxICON_ERROR|wxOK );

		//Print the stack trace
		wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( wxT ( "Debugger" ) );
		if ( info.IsShown() ) {
			Frame::Get()->GetDebuggerPane()->SelectTab ( DebuggerPane::FRAMES );
			UpdateDebuggerPane();
		}
	}
	break;
	case DBG_END_STEPPING:	// finished one of the following: next/step/nexti/stepi
	case DBG_FUNC_FINISHED:
	case DBG_UNKNOWN:		// the most common reason: temporary breakpoint
	case DBG_BP_HIT: { 		// breakpoint reached
		//query the current line and file
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if ( dbgr && dbgr->IsRunning() ) {
			dbgr->QueryFileLine();
		}
	}
	break;
	case DBG_DBGR_KILLED:
		m_dbgCanInteract = false;
		break;
	case DBG_EXITED_NORMALLY:
		//debugging finished, stop the debugger process
		DbgStop();
		break;
	default:
		break;
	}
}

void Manager::UpdateLostControl()
{
	//debugger lost control
	//hide the marker
	DbgUnMarkDebuggerLine();
	m_dbgCanInteract = false;
	DebugMessage ( _ ( "Continuing...\n" ) );
}

void Manager::UpdateBpAdded(const int internal_id, const int debugger_id)
{
	GetBreakpointsMgr()->SetBreakpointDebuggerID(internal_id, debugger_id);
}

void Manager::UpdateExpression ( const wxString &expression, const wxString &evaluated )
{
	Frame::Get()->GetDebuggerPane()->GetWatchesTable()->UpdateExpression ( expression, evaluated );
}

void Manager::UpdateQuickWatch ( const wxString &expression, TreeNode<wxString, NodeData> *tree )
{
	if ( m_useTipWin ) {

#if 0
		m_useTipWin = false;
		if ( m_debuggerTipWin ) {
			delete m_debuggerTipWin;
		}
		m_debuggerTipWin = new DebuggerTip ( GetMainFrame(), expression, tree, m_tipWinPos );
		m_debuggerTipWin->Popup();
#endif

	} else {
		//Display a dialog with the expression
		if ( !m_quickWatchDlg ) {
			//first time?
			m_quickWatchDlg = new QuickWatchDlg ( GetMainFrame(), expression, tree );
			m_quickWatchDlg->ShowModal();
		} else {
			//Dialog already created
			if ( m_quickWatchDlg->IsShown() ) {
				//dialog is visible, just update the tree
				m_quickWatchDlg->Init ( expression, tree );
			} else {
				//Update the tree
				m_quickWatchDlg->Init ( expression, tree );
				m_quickWatchDlg->ShowModal();
			}
		}
	}
}

void Manager::UpdateStackList ( const StackEntryArray &stackArr )
{
	Frame::Get()->GetDebuggerPane()->GetFrameListView()->Update ( stackArr );
}

void Manager::DbgSetFrame ( int frame, int lineno )
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( wxT ( "Debugger" ) );
	if ( info.IsShown() ) {
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {
			//set the frame
			dbgr->SetFrame ( frame );
			m_frameLineno = lineno;
		}
	}
}

void Manager::DbgSetThread ( long threadId )
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {
		//set the frame
		dbgr->SelectThread ( threadId );
		dbgr->QueryFileLine();
	}
}

void Manager::UpdateBuildTools()
{
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder ( wxT ( "GNU makefile for g++/gcc" ) );
	wxString tool = builder->GetBuildToolName();
	wxString origTool ( tool );

	//confirm that it exists...
	wxString path;
	bool is_ok ( true );

	EnvironmentConfig::Instance()->ApplyEnv ( NULL );

	if ( tool.Contains ( wxT ( "$" ) ) ) {
		//expand
		tool = EnvironmentConfig::Instance()->ExpandVariables ( tool );
	}

	if ( !ExeLocator::Locate ( tool, path ) ) {
		is_ok = false;
		//failed to locate the specified build tool
		//try some default names which are commonly used on windows
		if ( !is_ok && ExeLocator::Locate ( wxT ( "mingw32-make" ), path ) ) {
			tool = path;
			is_ok = true;
		}

		if ( !is_ok && ExeLocator::Locate ( wxT ( "make" ), path ) ) {
			tool = path;
			is_ok = true;
		}
	} else {
		//we are good, nothing to be done
		EnvironmentConfig::Instance()->UnApplyEnv();
		return;
	}
	EnvironmentConfig::Instance()->UnApplyEnv();

	wxString message;
	if ( !is_ok ) {
		message << wxT ( "Failed to locate make util '" )
		<< tool << wxT ( "' specified by 'Build Settings'" );
		wxLogMessage ( message );
		return;
	} else {
		wxLogMessage ( wxT ( "Updating build too to '" ) + tool + wxT ( "' from '" ) + origTool + wxT ( "'" ) );
	}

	//update the cached builders
	builder->SetBuildTool ( tool );
	BuildManagerST::Get()->AddBuilder ( builder );

	//update the configuration files
	BuildSystemPtr bsptr = BuildSettingsConfigST::Get()->GetBuildSystem ( wxT ( "GNU makefile for g++/gcc" ) );
	if ( !bsptr ) {
		bsptr = new BuildSystem ( NULL );
		bsptr->SetName ( wxT ( "GNU makefile for g++/gcc" ) );
	}

	bsptr->SetToolPath ( tool );
	BuildSettingsConfigST::Get()->SetBuildSystem ( bsptr );
}

bool Manager::OpenFileAndAppend ( const wxString &fileName, const wxString &text )
{
	bool ret ( false );
	if ( OpenFile ( fileName, wxEmptyString ) ) {
		LEditor* editor = Frame::Get()->GetMainBook()->GetActiveEditor();
		if ( editor ) {

			// if needed, append EOL
			// in an ideal world, we would like that the file will be terminated with 2xEOL
			if(editor->GetText().EndsWith(editor->GetEolString()) == false) {
				editor->AppendText(editor->GetEolString());
			}

			if(editor->GetText().EndsWith(editor->GetEolString() + editor->GetEolString()) == false){
				editor->AppendText(editor->GetEolString());
			}

			//get the current line number
			int lineNum = editor->GetLineCount();
			editor->GotoLine ( lineNum-1 );

			editor->AppendText ( text );
			ret = true;
		}
	}
	return ret;
}

void Manager::RunCustomPreMakeCommand ( const wxString &project )
{
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		return;
	}

	if ( m_shellProcess ) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}

	wxString conf;
	// get the selected configuration to be built
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( project, wxEmptyString );
	if ( bldConf ) {
		conf = bldConf->GetName();
	}

	QueueCommand info ( project, conf, false, QueueCommand::Build );
	m_shellProcess = new CompileRequest (	GetMainFrame(), //owner window
	                                      info,
	                                      wxEmptyString, 	//no file name (valid only for build file only)
	                                      true );			//run premake step only
	m_shellProcess->Process();
}

void Manager::UpdateMenuAccelerators()
{
	MenuItemDataMap menuMap, defAccelMap;

	wxArrayString files;
	DoGetAccelFiles ( files );

	// load user accelerators map
	LoadAcceleratorTable ( files, menuMap );

	// load the default accelerator map
	GetDefaultAcceleratorMap ( defAccelMap );

	// loop over default accelerators map, and search for items that does not exist in the user's list
	std::map< wxString, MenuItemData >::iterator it = defAccelMap.begin();
	for ( ; it != defAccelMap.end(); it++ ) {
		if ( menuMap.find ( it->first ) == menuMap.end() ) {
			// this item does not exist in the users accelerators
			// probably a new accelerator that was added to the default
			// files directly via update/manually modified it
			menuMap[it->first] = it->second;
		}
	}

	wxMenuBar *bar = Frame::Get()->GetMenuBar();

	wxString content;
	std::vector< wxAcceleratorEntry > accelVec;
	size_t count = bar->GetMenuCount();
	for ( size_t i=0; i< count; i++ ) {
		wxMenu * menu = bar->GetMenu ( i );
		UpdateMenu ( menu, menuMap, accelVec );
	}

	//In case we still have items to map, map them. this can happen for items
	//which exist in the list but does not have any menu associated to them in the menu bar (e.g. C++ menu)
	if ( menuMap.empty() == false ) {
//		wxString msg;
//		msg << wxT("Info: UpdateMenuAccelerators: There are still ") << menuMap.size() << wxT(" un-mapped item(s)");
//		wxLogMessage(msg);

		MenuItemDataMap::iterator iter = menuMap.begin();
		for ( ; iter != menuMap.end(); iter++ ) {
			MenuItemData itemData = iter->second;

			wxString txt;
			txt << itemData.action;
			if ( itemData.accel.IsEmpty() == false ) {
				txt << wxT ( "\t" ) << itemData.accel;
			}

			wxAcceleratorEntry* a = wxAcceleratorEntry::Create ( txt );
			if ( a ) {
				long commandId ( 0 );
				itemData.id.ToLong ( &commandId );

				//use the resource ID
				if ( commandId == 0 ) {
					commandId = wxXmlResource::GetXRCID ( itemData.id );
				}

				a->Set ( a->GetFlags(), a->GetKeyCode(), commandId );
				accelVec.push_back ( *a );
				delete a;
			}
		}
	}

	if ( content.IsEmpty() == false ) {
		wxFFile f ( GetStarupDirectory() + wxT ( "/config/accelerators.conf" ), wxT ( "w+b" ) );
		f.Write ( content );
		f.Close();
	}

	//update the accelerator table of the main frame
	wxAcceleratorEntry *entries = new wxAcceleratorEntry[accelVec.size() ];
	for ( size_t i=0; i < accelVec.size(); i++ ) {
		entries[i] = accelVec[i];
	}

	wxAcceleratorTable table ( accelVec.size(), entries );

	//update the accelerator table
	Frame::Get()->SetAcceleratorTable ( table );
	delete [] entries;
}

void Manager::DumpMenu ( wxMenu *menu, const wxString &label, wxString &content )
{
	wxMenuItemList items = menu->GetMenuItems();
	wxMenuItemList::iterator iter = items.begin();
	for ( ; iter != items.end(); iter++ ) {
		wxMenuItem *item =  *iter;
		if ( item->GetSubMenu() ) {
			DumpMenu ( item->GetSubMenu(), label + wxT ( "::" ) + item->GetLabel(), content );
		} else {

			if ( item->GetId() == wxID_SEPARATOR ) {
				continue;
			} else if ( item->GetId() >= RecentFilesSubMenuID && item->GetId() <= RecentFilesSubMenuID + 10 ) {
				continue;
			} else if ( item->GetId() >= RecentWorkspaceSubMenuID && item->GetId() <= RecentWorkspaceSubMenuID + 10 ) {
				continue;
			}

			//dump the content of this menu item
			content << item->GetId();
			content << wxT ( "|" );
			content << label;
			content << wxT ( "|" );
			content << StripAccelAndNemonics ( item->GetText() );
			content << wxT ( "|" );
			if ( item->GetAccel() ) {
				content << item->GetAccel()->ToString();
			}
			content << wxT ( "\n" );
		}
	}
}

void Manager::LoadAcceleratorTable ( const wxArrayString &files, MenuItemDataMap &accelMap )
{
	wxString content;
	for ( size_t i=0; i<files.GetCount(); i++ ) {
		wxString tmpContent;
		if ( ReadFileWithConversion ( files.Item ( i ), tmpContent ) ) {
			wxLogMessage ( wxString::Format ( wxT ( "Loading accelerators from '%s'" ), files.Item ( i ).c_str() ) );
			content << wxT ( "\n" ) << tmpContent;
		}
	}

	wxArrayString lines = wxStringTokenize ( content, wxT ( "\n" ) );
	for ( size_t i = 0; i < lines.GetCount(); i ++ ) {
		if ( lines.Item ( i ).Trim().Trim ( false ).IsEmpty() ) {
			continue;
		}

		MenuItemData item;
		wxString line = lines.Item ( i );

		item.id = line.BeforeFirst ( wxT ( '|' ) );
		line = line.AfterFirst ( wxT ( '|' ) );

		item.parent = line.BeforeFirst ( wxT ( '|' ) );
		line = line.AfterFirst ( wxT ( '|' ) );

		item.action = line.BeforeFirst ( wxT ( '|' ) );
		line = line.AfterFirst ( wxT ( '|' ) );

		item.accel = line.BeforeFirst ( wxT ( '|' ) );
		line = line.AfterFirst ( wxT ( '|' ) );

		accelMap[item.action] = item;
	}
}

void Manager::UpdateMenu ( wxMenu *menu, MenuItemDataMap &accelMap, std::vector< wxAcceleratorEntry > &accelVec )
{
	wxMenuItemList items = menu->GetMenuItems();
	wxMenuItemList::iterator iter = items.begin();
	for ( ; iter != items.end(); iter++ ) {
		wxMenuItem *item =  *iter;
		if ( item->GetSubMenu() ) {
			UpdateMenu ( item->GetSubMenu(), accelMap, accelVec );
		} else {

			if ( item->GetId() == wxID_SEPARATOR ) {
				continue;
			}

			//search this item in the accelMap
			wxString labelText = StripAccelAndNemonics ( item->GetText() );
			if ( accelMap.find ( labelText ) != accelMap.end() ) {
				MenuItemData item_data = accelMap.find ( labelText )->second;

				wxString txt;
				txt << StripAccel ( item->GetText() );

				//set the new accelerator
				if ( item_data.accel.IsEmpty() == false ) {
					txt << wxT ( "\t" ) << item_data.accel;
				}

				txt.Replace ( wxT ( "_" ), wxT ( "&" ) );
				item->SetText ( txt );

				wxAcceleratorEntry* a = wxAcceleratorEntry::Create ( txt );
				if ( a ) {
					a->Set ( a->GetFlags(), a->GetKeyCode(), item->GetId() );
					accelVec.push_back ( *a );
					delete a;
				}

				//remove this entry from the map
				accelMap.erase ( labelText );
			}
		}
	}
}

void Manager::GetAcceleratorMap ( MenuItemDataMap& accelMap )
{
	wxArrayString files;
	DoGetAccelFiles ( files );
	LoadAcceleratorTable ( files, accelMap );
}

void Manager::BuildRefactorDatabase ( const wxString& word, CppTokensMap &l )
{
	wxArrayString projects;
	GetProjectList ( projects );
	std::vector<wxFileName> files;


	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		ProjectPtr proj = GetProject ( projects.Item ( i ) );
		if ( proj ) {
			//change the directory to the project dir
			proj->GetFiles ( files, true );
		}
	}

//	JobQueueSingleton::Instance()->PushJob(new RefactorIndexBuildJob(files, word.c_str()));
	RefactorIndexBuildJob job ( files, word.c_str() );
	job.Parse ( word, l );
}

void Manager::ReplaceInFiles ( const wxString &word, std::list<CppToken> &li )
{
	std::list<CppToken>::iterator iter = li.begin();

	int amount ( 0 );
	if ( iter != li.end() ) {
		amount = word.Len() - iter->getName().Len();
	}

	int off ( 0 );
	wxString file_name ( wxEmptyString );

	for ( ; iter != li.end(); iter++ ) {
		CppToken token = *iter;

		// update next token offset in case we are still in the same file
		if ( !file_name.IsEmpty() ) {
			if ( file_name == token.getFilename() ) {
				token.setOffset ( token.getOffset() +off );
			} else {
				// switched file
				off = 0;
				file_name = token.getFilename();
			}
		} else {
			//first time
			file_name = token.getFilename();
		}

		//open this file
		if ( OpenFile ( token.getFilename(), wxEmptyString ) ) {
			//do the actual replacement here
			wxFileName fn ( token.getFilename() );
			LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();

			// did we managed to open the file correctly?
			if ( editor && editor->GetFileName().GetFullPath() == fn.GetFullPath() ) {

				//select the target string
				editor->SetSelection ( token.getOffset(), token.getOffset() +token.getName().Len() );

				//replace the selection
				if ( editor->GetSelectedText().IsEmpty() == false ) {
					editor->ReplaceSelection ( word );
					off += amount;
//					UpdateOffsetsForFile(li, iter, token.getFilename(), token.getName().Length()-word.Len());
				}
			}
		}
	}
}
void Manager::RetagFile ( const wxString& filename )
{
	if ( !IsWorkspaceClosing() ) {

		if ( !TagsManagerST::Get()->IsValidCtagsFile ( wxFileName ( filename ) ) ) {
			wxLogMessage ( wxT ( "Not a valid C tags file type: %s. Skipping." ), filename.c_str() );
			return;
		}

		// re-tag the file
		ParseRequest *req = new ParseRequest();

		// Put a request on the parsing thread to update the GUI tree for this file
		wxFileName fn = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();
		req->setDbFile ( fn.GetFullPath().c_str() );

		// Construct an absolute file name for ctags
		wxFileName absFile ( filename );
		absFile.MakeAbsolute();
		req->setFile ( absFile.GetFullPath().c_str() );

		wxString msg = wxString::Format(wxT( "Re-tagging file %s..." ), absFile.GetFullName().c_str());
		Frame::Get()->SetStatusMessage(msg, 0, XRCID("retag_file"));

		ParseThreadST::Get()->Add ( req );
	} else {
		wxLogMessage ( wxString::Format ( wxT ( "Workspace in being closed, skipping re-tag for file %s" ), filename.c_str() ) );
	}
}

wxString Manager::GetProjectExecutionCommand ( const wxString& projectName, wxString &wd, bool considerPauseWhenExecuting )
{
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( projectName, wxEmptyString );
	if ( !bldConf ) {
		wxLogMessage ( wxT ( "failed to find project configuration for project '" ) + projectName + wxT ( "'" ) );
		return wxEmptyString;
	}

	//expand variables
	wxString cmd = bldConf->GetCommand();
	cmd = ExpandVariables ( cmd, GetProject ( projectName ) );

	wxString cmdArgs = bldConf->GetCommandArguments();
	cmdArgs = ExpandVariables ( cmdArgs, GetProject ( projectName ) );

	//execute command & cmdArgs
	wxString execLine ( cmd + wxT ( " " ) + cmdArgs );
	wd = bldConf->GetWorkingDirectory();
	wd = ExpandVariables ( wd, GetProject ( projectName ) );

	//change directory to the working directory
	if ( considerPauseWhenExecuting ) {
		ProjectPtr proj = GetProject ( projectName );

#if defined(__WXMAC__)
		execLine = wxString ( wxT ( "osascript -e 'tell application \"Terminal\"'" ) ) +
		           wxT ( " -e   'activate'" ) +
		           wxT ( " -e   'do script with command \"cd " ) + proj->GetFileName().GetPath() + wxT ( " && cd " ) + wd + wxT ( " && " ) + execLine + wxT ( "\"'" ) +
		           wxT ( " -e  'end tell'" );

#elif defined(__WXGTK__)
		//set a console to the execute target
		wxString term;
		term << wxT ( "xterm -title " );
		term << wxT ( "'" ) << execLine << wxT ( "'" );
		term << wxT ( " -e " );

		if ( bldConf->GetPauseWhenExecEnds() ) {
			wxString ld_lib_path;
			wxFileName exePath ( wxStandardPaths::Get().GetExecutablePath() );
			wxFileName exeWrapper ( exePath.GetPath(), wxT ( "le_exec.sh" ) );

			if ( wxGetEnv ( wxT ( "LD_LIBRARY_PATH" ), &ld_lib_path ) && ld_lib_path.IsEmpty() == false ) {
				term << wxT ( "/bin/sh -f " ) << exeWrapper.GetFullPath() << wxT ( " LD_LIBRARY_PATH=" ) << ld_lib_path << wxT ( " " );
			} else {
				term << wxT ( "/bin/sh -f " ) << exeWrapper.GetFullPath() << wxT ( " " );
			}
		}

		term << execLine;
		execLine = term;
#elif defined (__WXMSW__)

		if ( bldConf->GetPauseWhenExecEnds() ) {
			execLine.Prepend ( CL_EXEC_WRAPPER + wxT ( " " ) );
		}
#endif
	}
	return execLine;
}

void Manager::GetDefaultAcceleratorMap ( MenuItemDataMap& accelMap )
{
	//use the default settings
	wxString fileName = GetStarupDirectory() + wxT ( "/config/accelerators.conf.default" );
	wxArrayString files;
	files.Add ( fileName );

	// append the content of all '*.accelerators' from the plugins
	// resources table
	wxDir::GetAllFiles ( GetInstallDir() + wxT ( "/plugins/resources/" ), &files, wxT ( "*.accelerators" ), wxDIR_FILES );
	LoadAcceleratorTable ( files, accelMap );
}

void Manager::ReloadWorkspace()
{
	// if no workspace is currently opened, return
	if ( !IsWorkspaceOpen() ) {
		return;
	}

	//close any debugging session that is currently opened
	DbgStop();

	WorkspaceST::Get()->ReloadWorkspace();

	DoSetupWorkspace ( WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath() );
}

void Manager::DoSetupWorkspace ( const wxString &path )
{
	// update status bar
	wxString errMsg;
	wxString dbfile = WorkspaceST::Get()->GetStringProperty ( wxT ( "Database" ), errMsg );

	wxFileName fn ( dbfile );
	// load ctags options
	wxBusyCursor cursor;

	//update the 'Recent Workspace' history
	AddToRecentlyOpenedWorkspaces ( path );

	SendCmdEvent ( wxEVT_WORKSPACE_LOADED );

	// load the session like it was saved
	if ( Frame::Get()->GetFrameGeneralInfo().GetFlags() & CL_LOAD_LAST_SESSION ) {
		SessionEntry session;
		if ( SessionManager::Get().FindSession ( path, session ) ) {

			// set this session as the active one
			SessionManager::Get().SetLastWorkspaceName ( path );

			//restore notebook tabs
			Frame::Get()->GetMainBook()->RestoreSession(session);
		}
	}
}

void Manager::ClearFileHistory()
{
	size_t count = m_recentFiles.GetCount();
	for ( size_t i=0; i<count; i++ ) {
		m_recentFiles.RemoveFileFromHistory ( 0 );
	}

	count = m_recentWorkspaces.GetCount();
	for ( size_t i=0; i<count; i++ ) {
		m_recentWorkspaces.RemoveFileFromHistory ( 0 );
	}

	// synchronized the configuration file as well
	wxArrayString files;
	EditorConfig *cfg = EditorConfigST::Get();

	cfg->SetRecentlyOpenedFies ( files );
	cfg->SetRecentlyOpenedWorkspaces ( files );
}

bool Manager::HasHistory() const
{
	return m_recentFiles.GetCount() >0 || m_recentWorkspaces.GetCount() >0;
}

void Manager::PushQueueCommand ( const QueueCommand& buildInfo )
{
	m_buildQueue.push_back ( buildInfo );
}

void Manager::ProcessCommandQueue()
{
	if ( m_buildQueue.empty() ) {
		return;
	}

	// pop the next build build and process it
	QueueCommand qcmd = m_buildQueue.front();
	m_buildQueue.pop_front();

	if ( qcmd.GetCheckBuildSuccess() && !IsBuildEndedSuccessfully() ) {
		// build failed, remove command from the queue
		return;
	}

	switch ( qcmd.GetKind() ) {
	case QueueCommand::CustomBuild:
		DoCustomBuild ( qcmd );
		break;
	case QueueCommand::Clean:
		DoCleanProject ( qcmd );
		break;
	case QueueCommand::Build:
		DoBuildProject ( qcmd );
		break;
	case QueueCommand::Debug:
		DbgStart ( wxNOT_FOUND );
		break;
	}
}

void Manager::DoBuildProject ( const QueueCommand& buildInfo )
{
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		return;
	}

	if ( m_shellProcess ) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}

	//save all files before compiling, but dont saved new documents
	if (!Frame::Get()->GetMainBook()->SaveAll(false, false))
		return;

	//If a debug session is running, stop it.
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() ) {
		if ( wxMessageBox ( _ ( "This would terminate the current debug session, continue?" ),  wxT ( "Confirm" ), wxICON_QUESTION|wxYES_NO|wxCANCEL ) != wxYES ) {
			return;
		} else {
			DbgStop();
		}
	}
	m_shellProcess = new CompileRequest ( GetMainFrame(), buildInfo );
	m_shellProcess->Process();
}

void Manager::DoCleanProject ( const QueueCommand& buildInfo )
{
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		return;
	}

	if ( m_shellProcess ) {
		delete m_shellProcess;
	}

	// TODO :: replace the construction of CleanRequest to include the proper build configuration
	m_shellProcess = new CleanRequest ( GetMainFrame(), buildInfo );
	m_shellProcess->Process();
}

bool Manager::IsPaneVisible ( const wxString& pane_name )
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane ( pane_name );
	if ( info.IsOk() && info.IsShown() ) {
		return true;
	}
	return false;
}

void Manager::BuildWorkspace()
{
	DoCmdWorkspace ( QueueCommand::Build );
	// start the build process
	ProcessCommandQueue();
}

void Manager::CleanWorkspace()
{
	DoCmdWorkspace ( QueueCommand::Clean );
	// start the build process
	ProcessCommandQueue();
}

void Manager::DoCmdWorkspace ( int cmd )
{
	// get list of projects
	wxArrayString projects;
	wxArrayString optimizedList;

	ManagerST::Get()->GetProjectList ( projects );

	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		ProjectPtr p = GetProject ( projects.Item ( i ) );
		BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf ( projects.Item ( i ), wxEmptyString );
		if ( p && buildConf ) {
			wxArrayString deps = p->GetDependencies ( buildConf->GetName() );
			for ( size_t j=0; j<deps.GetCount(); j++ ) {
				// add a project only if it does not exist yet
				if ( optimizedList.Index ( deps.Item ( j ) ) == wxNOT_FOUND ) {
					optimizedList.Add ( deps.Item ( j ) );
				}
			}
			// add the project itself now, again only if it is not included yet
			if ( optimizedList.Index ( projects.Item ( i ) ) == wxNOT_FOUND ) {
				optimizedList.Add ( projects.Item ( i ) );
			}
		}
	}

	// add a build/clean project only command for every project in the optimized list
	for ( size_t i=0; i<optimizedList.GetCount(); i++ ) {
		BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf ( optimizedList.Item ( i ), wxEmptyString );
		if ( buildConf ) {
			QueueCommand bi ( optimizedList.Item ( i ), buildConf->GetName(), true, cmd );
			if ( buildConf->IsCustomBuild() ) {
				bi.SetKind ( QueueCommand::CustomBuild );
				switch ( cmd ) {
				case QueueCommand::Build:
					bi.SetCustomBuildTarget ( wxT ( "Build" ) );
					break;
				case QueueCommand::Clean:
					bi.SetCustomBuildTarget ( wxT ( "Clean" ) );
					break;
				}
			}
			bi.SetCleanLog ( i == 0 );
			PushQueueCommand ( bi );
		}
	}
}

void Manager::RebuildWorkspace()
{
	DoCmdWorkspace ( QueueCommand::Clean );
	DoCmdWorkspace ( QueueCommand::Build );
	ProcessCommandQueue();
}

void Manager::DoGetAccelFiles ( wxArrayString& files )
{
	//try to locate the user's settings
	wxString fileName ( GetStarupDirectory() + wxT ( "/config/accelerators.conf" ) );
	if ( !wxFileName::FileExists ( GetStarupDirectory() + wxT ( "/config/accelerators.conf" ) ) ) {

		//use the default settings
		fileName = GetStarupDirectory() + wxT ( "/config/accelerators.conf.default" );
		files.Add ( fileName );

		// append the content of all '*.accelerators' from the plugins
		// resources table
		wxDir::GetAllFiles ( GetInstallDir() + wxT ( "/plugins/resources/" ), &files, wxT ( "*.accelerators" ), wxDIR_FILES );
	} else {
		files.Add ( fileName );
	}
}

void Manager::DoCustomBuild ( const QueueCommand& buildInfo )
{
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		return;
	}

	if ( m_shellProcess ) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}

	//save all files before compiling, but dont saved new documents
	if (!Frame::Get()->GetMainBook()->SaveAll(false, false))
		return;

	//If a debug session is running, stop it.
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() ) {
		if ( wxMessageBox ( _ ( "This would terminate the current debug session, continue?" ),  wxT ( "Confirm" ), wxICON_QUESTION|wxYES_NO|wxCANCEL ) != wxYES ) {
			return;
		} else {
			DbgStop();
		}
	}
	m_shellProcess = new CustomBuildRequest ( GetMainFrame(), buildInfo, wxEmptyString );
	m_shellProcess->Process();
}

void Manager::UpdateRemoteTargetConnected(const wxString& line)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning() && IsWorkspaceOpen()) {
		// we currently do not support this feature when debugging using 'Quick debug'
		wxString errMsg;
		ProjectPtr proj = WorkspaceST::Get()->FindProjectByName ( GetActiveProjectName(), errMsg );
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( proj->GetName(), wxEmptyString );
		if ( bldConf ) {
			wxArrayString dbg_cmds = wxStringTokenize ( bldConf->GetDebuggerPostRemoteConnectCmds(), wxT ( "\n" ), wxTOKEN_STRTOK );
			for(size_t i=0; i<dbg_cmds.GetCount(); i++){
				dbgr->ExecuteCmd(dbg_cmds.Item(i));
			}
		}
	}
	// log the line
	UpdateAddLine(line);
}

wxFileName Manager::FindFile ( const wxString &filename, const wxString &project )
{
	wxFileName fn ( filename );

	if ( !fn.FileExists() ) {
		// try to open the file as is
		fn.Clear();
	}
	if ( !fn.IsOk() && !project.IsEmpty() ) {
		// try to open the file in context of its project
		wxArrayString project_files;
		GetProjectFiles ( project, project_files );
		fn = FindFile ( project_files, filename );
	}
	if ( !fn.IsOk() ) {
		// no luck there.  try the whole workspace
		wxArrayString workspace_files;
		GetWorkspaceFiles ( workspace_files );
		fn = FindFile ( workspace_files, filename );
	}
	return fn;
}

// Please do not change this code!
wxFileName Manager::FindFile ( const wxArrayString& files, const wxFileName &fn )
{
	// Iterate over the files twice:
	// first, try to full path
	// if the first iteration failes, iterate the files again
	// and compare full name only
	if ( fn.IsAbsolute() && !fn.GetFullPath().Contains ( wxT ( ".." ) ) ) {
		return fn;
	}

	// try to convert it to absolute path
	wxFileName f1 ( fn );
	if ( f1.MakeAbsolute() && f1.FileExists() && !f1.GetFullPath().Contains ( wxT ( ".." ) ) ) {
		return f1;
	}

	for ( size_t i=0; i< files.GetCount(); i++ ) {
		wxFileName tmpFileName ( files.Item ( i ) );
		if ( tmpFileName.GetFullPath() == fn.GetFullPath() ) {
			wxFileName tt ( tmpFileName );
			if ( tt.MakeAbsolute() ) {
				return tt;
			} else {
				return tmpFileName;
			}
		}
	}

	std::vector<wxFileName> matches;

	for ( size_t i=0; i< files.GetCount(); i++ ) {
		wxFileName tmpFileName ( files.Item ( i ) );
		if ( tmpFileName.GetFullName() == fn.GetFullName() ) {
			matches.push_back ( tmpFileName );
		}
	}

	wxString lastDir;
	wxArrayString dirs = fn.GetDirs();
	if ( dirs.GetCount() > 0 ) {
		lastDir = dirs.Last();
	}

	if ( matches.size() == 1 ) {
		wxFileName tt ( matches.at ( 0 ) );
		if ( tt.MakeAbsolute() ) {
			return tt;
		} else {
			return matches.at ( 0 );
		}

	} else if ( matches.size() > 1 ) {
		// take the best match
		std::vector<wxFileName> betterMatches;
		for ( size_t i=0; i<matches.size(); i++ ) {

			wxFileName filename ( matches.at ( i ) );
			wxArrayString tmpdirs = filename.GetDirs();
			if ( tmpdirs.GetCount() > 0 ) {
				if ( tmpdirs.Last() == lastDir ) {
					betterMatches.push_back ( filename );
				}
			}
		}

		if ( betterMatches.size() == 1 ) {
			wxFileName tt ( betterMatches.at ( 0 ) );
			if ( tt.MakeAbsolute() ) {
				return tt;
			} else {
				return betterMatches.at ( 0 );
			}
		} else {
			// open the first match
			wxFileName tt ( matches.at ( 0 ) );
			if ( tt.MakeAbsolute() ) {
				return tt;
			} else {
				return matches.at ( 0 );
			}
		}
	}
	return wxFileName();
}

