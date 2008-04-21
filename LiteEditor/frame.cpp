#include "precompiled_header.h"
#include "acceltabledlg.h"
#include "drawingutils.h"
#include "fileexplorertree.h"
#include "newprojectdlg.h"
#include "newworkspacedlg.h"
#include "replaceinfilespanel.h"
#include "pluginmgrdlg.h"
#include "wx/clipbrd.h"
#include "wx/numdlg.h"
#include "environmentconfig.h"
#include "shell_window.h"
#include "findresultstab.h"
#include "buidltab.h"
#include "frame.h"
#include "splashscreen.h"
#include "wx/stdpaths.h"
#include <wx/xrc/xmlres.h>
#include "symbol_tree.h"
#include <wx/splitter.h>
#include "cpp_symbol_tree.h" 
#include "plugin.h"
#include "language.h"
#include "editor_config.h"
#include "manager.h"
#include "menumanager.h"
#include <wx/aboutdlg.h>
#include "findinfilesdlg.h"
#include "search_thread.h"
#include "project.h"
#include "fileview.h"
#include "wx/aui/framemanager.h"
#include "options_base_dlg.h"
#include "configuration_manager_dlg.h"
#include "filedroptarget.h"
#include "advanced_settings.h"
#include "build_settings_config.h"
#include "list"
#include "macros.h"
#include "editor_creator.h"
#include "async_executable_cmd.h"
#include "open_resouce_dlg.h"
#include "open_type_dlg.h"
#include "workspace_pane.h"
#include "extdbwizard.h"
#include "navigationmanager.h"
#include "keyvaluetabledlg.h"
#include "debuggermanager.h"
#include "breakpointdlg.h"
#include "generalinfo.h"
#include "debuggersettingsdlg.h"
#include "quickoutlinedlg.h"
#include "debuggerpane.h"
#include "wx/ffile.h"
#include "sessionmanager.h"
#include "pluginmanager.h"
#include "wx/dir.h"
#include "exelocator.h"
#include "builder.h"
#include "buildmanager.h"
#include "debuggerconfigtool.h"
#include "openwindowspanel.h"
#include "workspace_pane.h"
#include "globals.h"
#include "fileexplorer.h"
#include "custom_notebook.h"

#ifdef __WXGTK20__
#include <gtk-2.0/gtk/gtk.h>
#endif

typedef int (*_GCC_COLOUR_FUNC_PTR)(int, const char*, size_t&, size_t&);

extern const wxChar *SvnRevision;
extern char *cubes_xpm[];
extern unsigned char cubes_alpha[];
extern time_t GetFileModificationTime(const wxString &fileName);
extern void SetGccColourFunction(_GCC_COLOUR_FUNC_PTR func);
static int FrameTimerId = wxNewId();

//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_IDLE(Frame::OnIdle)
	EVT_ACTIVATE(Frame::OnAppActivated)
	EVT_SYMBOLTREE_ADD_ITEM(wxID_ANY, Frame::OnAddSymbols)
	EVT_SYMBOLTREE_DELETE_ITEM(wxID_ANY, Frame::OnDeleteSymbols)
	EVT_SYMBOLTREE_UPDATE_ITEM(wxID_ANY, Frame::OnUpdateSymbols)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED, Frame::OnSearchThread)

	//build/debugger events
	EVT_COMMAND(wxID_ANY, wxEVT_BUILD_ADDLINE, Frame::OnBuildEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_BUILD_STARTED, Frame::OnBuildEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_BUILD_ENDED, Frame::OnBuildEvent)

	EVT_MENU(XRCID("close_other_tabs"), Frame::OnCloseAllButThis)
	EVT_MENU(XRCID("copy_file_name"), Frame::OnCopyFilePath)
	EVT_MENU(XRCID("copy_file_path"), Frame::OnCopyFilePathOnly)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE, Frame::OnOutputWindowEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED, Frame::OnOutputWindowEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, Frame::OnOutputWindowEvent)
	EVT_MENU(wxID_CLOSE_ALL, Frame::OnFileCloseAll)
	EVT_MENU(XRCID("fix_ext_database"), Frame::OnFixDatabasePaths)
	EVT_MENU(XRCID("exit_app"), Frame::OnQuit)
	EVT_MENU(XRCID("save_file"), Frame::OnSave)
	EVT_MENU(XRCID("save_file_as"), Frame::OnSaveAs)
	EVT_MENU(XRCID("about"), Frame::OnAbout)
	EVT_MENU(XRCID("new_file"), Frame::OnFileNew)
	EVT_MENU(XRCID("open_file"), Frame::OnFileOpen)
	EVT_MENU(XRCID("close_file"), Frame::OnFileClose)
	EVT_MENU(XRCID("save_all"), Frame::OnFileSaveAll)
	EVT_MENU(wxID_CUT, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_COPY, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_PASTE, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_UNDO, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_REDO, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_SELECTALL, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_DUPLICATE, Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("refresh_file"), Frame::OnFileReload)
	EVT_MENU(XRCID("select_to_brace"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("match_brace"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_next"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous"), Frame::DispatchCommandEvent)
	EVT_MENU(wxID_FIND, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_REPLACE, Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_linenumber"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("previous_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("removeall_bookmarks"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_definition"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_previous_definition"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_in_files"), Frame::OnFindInFiles)
	EVT_MENU(XRCID("new_workspace"), Frame::OnProjectNewWorkspace)
	EVT_MENU(XRCID("new_project"), Frame::OnProjectNewProject)
	EVT_MENU(XRCID("switch_to_workspace"), Frame::OnSwitchWorkspace)
	EVT_MENU(XRCID("close_workspace"), Frame::OnCloseWorkspace)
	EVT_MENU(XRCID("add_project"), Frame::OnProjectAddProject)
	EVT_MENU(XRCID("manage_plugins"), Frame::OnManagePlugins)
	EVT_UPDATE_UI(XRCID("add_project"), Frame::OnWorkspaceMenuUI)
	EVT_UPDATE_UI(XRCID("save_all"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("copy_file_name"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("copy_file_path"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_CUT, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_COPY, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_PASTE, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_UNDO, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_REDO, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_SELECTALL, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_DUPLICATE, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("select_to_brace"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("match_brace"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("find_next"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("find_previous"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("goto_linenumber"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_FIND, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_REPLACE, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("toggle_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("next_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(viewAsSubMenuID, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("previous_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("removeall_bookmarks"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("new_project"), Frame::OnWorkspaceMenuUI)
	EVT_UPDATE_UI(XRCID("close_workspace"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("view_as_menu"), Frame::OnFileExistUpdateUI)
	EVT_MENU(XRCID("complete_word"), Frame::OnCompleteWord)
	EVT_MENU(XRCID("tags_options"), Frame::OnCtagsOptions)
	EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchCommandEvent)
	EVT_UPDATE_UI_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchUpdateUIEvent)
	EVT_MENU(XRCID("options"), Frame::OnViewOptions)
	EVT_UPDATE_UI(XRCID("word_wrap"), Frame::DispatchUpdateUIEvent)
	EVT_MENU(XRCID("word_wrap"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("configuration_manager"), Frame::OnConfigurationManager)
	EVT_UPDATE_UI(XRCID("configuration_manager"), Frame::OnWorkspaceOpen)
	EVT_MENU(XRCID("toggle_panes"), Frame::OnTogglePanes)
	EVT_MENU(XRCID("add_envvar"), Frame::OnAddEnvironmentVariable)
	EVT_MENU(XRCID("advance_settings"), Frame::OnAdvanceSettings)
	EVT_MENU(XRCID("build_active_project"), Frame::OnBuildProject)
	EVT_MENU(XRCID("compile_active_file"), Frame::OnCompileFile)
	EVT_MENU(XRCID("clean_active_project"), Frame::OnCleanProject)
	EVT_MENU(XRCID("build_n_run_active_project"), Frame::OnBuildAndRunProject)
	EVT_MENU(XRCID("rebuild_active_project"), Frame::OnRebuildProject)
	EVT_MENU(XRCID("stop_active_project_build"), Frame::OnStopBuild)
	EVT_MENU(XRCID("stop_executed_program"), Frame::OnStopExecutedProgram)
	EVT_UPDATE_UI(XRCID("stop_active_project_build"), Frame::OnStopBuildUI)
	EVT_UPDATE_UI(XRCID("stop_executed_program"), Frame::OnStopExecutedProgramUI)
	EVT_UPDATE_UI(XRCID("clean_active_project"), Frame::OnCleanProjectUI)
	EVT_MENU(XRCID("execute_no_debug"), Frame::OnExecuteNoDebug)
	EVT_MENU(XRCID("next_error"), Frame::OnNextBuildError)
	EVT_MENU(XRCID("create_ext_database"), Frame::OnBuildExternalDatabase)
	EVT_MENU(XRCID("open_ext_database"), Frame::OnUseExternalDatabase)
	EVT_MENU(XRCID("close_ext_database"), Frame::OnCloseExternalDatabase)
	EVT_MENU(XRCID("find_resource"), Frame::OnFindResource)
	EVT_MENU(XRCID("find_type"), Frame::OnFindType)
	EVT_MENU(XRCID("find_symbol"), Frame::OnQuickOutline)
	EVT_MENU(XRCID("attach_debugger"), Frame::OnDebugAttach)
	EVT_MENU(XRCID("add_project"), Frame::OnProjectAddProject)
	//	EVT_MENU(XRCID("import_from_makefile"), Frame::OnImportMakefile)
	EVT_MENU(XRCID("import_from_msvs"), Frame::OnImportMSVS)
	//	EVT_UPDATE_UI(XRCID("import_from_makefile"), Frame::OnImportMakefileUI)
	EVT_CLOSE(Frame::OnClose)
	EVT_TIMER(FrameTimerId, Frame::OnTimer)
	EVT_MENU_RANGE(RecentFilesSubMenuID, RecentFilesSubMenuID + 10, Frame::OnRecentFile)
	EVT_MENU_RANGE(RecentWorkspaceSubMenuID, RecentWorkspaceSubMenuID + 10, Frame::OnRecentWorkspace)
	EVT_UPDATE_UI(wxID_FORWARD, Frame::OnBackwardForwardUI)
	EVT_MENU(wxID_FORWARD, Frame::OnBackwardForward)
	EVT_UPDATE_UI(wxID_BACKWARD, Frame::OnBackwardForwardUI)
	EVT_MENU(wxID_BACKWARD, Frame::OnBackwardForward)
	EVT_MENU(XRCID("start_debugger"), Frame::OnDebug)
	EVT_MENU(XRCID("stop_debugger"), Frame::OnDebugStop)
	EVT_MENU(XRCID("insert_breakpoint"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_breakpoint"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("pause_debugger"), Frame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_stepin"), Frame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_stepout"), Frame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_next"), Frame::OnDebugCmd)
	EVT_MENU(XRCID("debuger_settings"), Frame::OnDebuggerSettings)
	EVT_UPDATE_UI(XRCID("pause_debugger"), Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("stop_debugger"), Frame::OnDebugStopUI)
	EVT_UPDATE_UI(XRCID("start_debugger"), Frame::OnDebugUI)
	EVT_HTML_LINK_CLICKED(wxID_ANY, Frame::OnLinkClicked)
	EVT_UPDATE_UI(XRCID("load_last_session"), Frame::OnLoadLastSessionUI)
	EVT_MENU(XRCID("load_last_session"), Frame::OnLoadLastSession)
	EVT_MENU(XRCID("view_welcome_page"), Frame::OnShowWelcomePage)
	EVT_UPDATE_UI(XRCID("view_welcome_page"), Frame::OnShowWelcomePageUI)
	EVT_MENU(XRCID("view_welcome_page_at_startup"), Frame::OnLoadWelcomePage)
	EVT_UPDATE_UI(XRCID("view_welcome_page_at_startup"), Frame::OnLoadWelcomePageUI)

	EVT_MENU(XRCID("line_end_cr"), Frame::OnViewEolCR)
	EVT_MENU(XRCID("line_end_lf"), Frame::OnViewEolLF)
	EVT_MENU(XRCID("line_end_crlf"), Frame::OnViewEolCRLF)
	EVT_UPDATE_UI(XRCID("line_end_cr"), Frame::OnViewEolCR_UI)
	EVT_UPDATE_UI(XRCID("line_end_lf"), Frame::OnViewEolLF_UI)
	EVT_UPDATE_UI(XRCID("line_end_crlf"), Frame::OnViewEolCRLF_UI)
	EVT_UPDATE_UI(XRCID("line_end_chars_menu"), Frame::OnViewEolUI)
	EVT_MENU(XRCID("convert_eol"), Frame::OnConvertEol)
	EVT_UPDATE_UI(XRCID("convert_eol"), Frame::OnConvertEolUI)
	EVT_MENU(XRCID("display_eol"), Frame::OnViewDisplayEOL)
	EVT_UPDATE_UI(XRCID("display_eol"), Frame::OnViewDisplayEOL_UI)

	//-----------------------------------------------------------------
	//C++ context menu
	//-----------------------------------------------------------------
	EVT_MENU(XRCID("swap_files"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("comment_selection"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("comment_line"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("find_decl"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("find_impl"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("insert_doxy_comment"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("move_impl"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_impl"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_multi_impl"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("setters_getters"), Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_include_file"), Frame::OnCppContextMenu)
	
	EVT_MENU(XRCID("configure_accelerators"), Frame::OnConfigureAccelerators)
	
//	#if defined (__WXMSW__) || defined (__WXMAC__)
	EVT_UPDATE_UI(XRCID("save_file"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("complete_word"), Frame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("execute_no_debug"), Frame::OnExecuteNoDebugUI)
	EVT_UPDATE_UI(XRCID("dbg_stepin"), Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("dbg_stepout"), Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("dbg_next"), Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("save_file_as"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("build_active_project"), Frame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("compile_active_file"), Frame::OnCompileFileUI)
	EVT_UPDATE_UI(XRCID("build_n_run_active_project"), Frame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("rebuild_active_project"), Frame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("refresh_file"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("find_type"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("find_symbol"), Frame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("goto_definition"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("goto_previous_definition"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("find_resource"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("insert_breakpoint"), Frame::OnDebugManageBreakpointsUI)
	EVT_UPDATE_UI(XRCID("delete_breakpoint"), Frame::OnDebugManageBreakpointsUI)
	EVT_UPDATE_UI(XRCID("next_error"), Frame::OnNextBuildErrorUI)
	EVT_UPDATE_UI(XRCID("close_file"), Frame::OnFileCloseUI)
//	#endif

END_EVENT_TABLE()
Frame* Frame::m_theFrame = NULL;

Frame::Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
		: wxFrame(pParent, id, title, pos, size, style)
		, m_findInFilesDlg(NULL)
		, m_buildInRun(false)
		, m_rebuild(false)
		, m_doingReplaceInFiles(false)
		, m_cppMenu(NULL)
{
#if  defined(__WXGTK20__)
	// A rather ugly hack here.  GTK V2 insists that F10 should be the
	// accelerator for the menu bar.  We don't want that.  There is
	// no sane way to turn this off, but we *can* get the same effect
	// by setting the "menu bar accelerator" property to the name of a
	// function key that is apparently legal, but doesn't really exist.
	// (Or if it does, it certainly isn't a key we use.)
	gtk_settings_set_string_property (gtk_settings_get_default (),
	                                  "gtk-menu-bar-accel", "F15", "foo");

#endif

	CreateGUIControls();

	ManagerST::Get();	// Dummy call

	//allow the main frame to receive files by drag and drop
	SetDropTarget( new FileDropTarget() );

	// Start the search thread
	SearchThreadST::Get()->SetNotifyWindow(this);
	SearchThreadST::Get()->Start(WXTHREAD_MIN_PRIORITY);

	//start the editor creator thread
	EditorCreatorST::Get()->SetParent(GetNotebook());
	m_timer = new wxTimer(this, FrameTimerId);
	m_timer->Start(100);
}

Frame::~Frame(void)
{
	delete m_timer;
	ManagerST::Free();
	// uninitialize AUI manager
	m_mgr.UnInit();
	//#ifdef __WXMSW__
	//	force exit!
	//	wxTheApp->ExitMainLoop();
	//#endif
}

void Frame::Initialize(bool loadLastSession)
{
	//set the revision number in the frame title
	wxString title(wxT("CodeLite - Revision: "));
	title << SvnRevision;

	//initialize the environment variable configuration manager
	EnvironmentConfig::Instance()->Load();

	EditorConfig *cfg = EditorConfigST::Get();
	GeneralInfo inf;
	cfg->ReadObject(wxT("GeneralInfo"), &inf);
	m_theFrame = new Frame(	NULL,
	                        wxID_ANY,
	                        title,
	                        inf.GetFramePosition(),
	                        inf.GetFrameSize(),
	                        wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

	m_theFrame->m_frameGeneralInfo = inf;
	m_theFrame->Maximize(m_theFrame->m_frameGeneralInfo.GetFlags() & CL_MAXIMIZE_FRAME ? true : false);

	// upgrade: change all .db files under the startup directory to be
	// .tags
	m_theFrame->UpgradeExternalDbExt();

	//add the welcome page
	if (m_theFrame->m_frameGeneralInfo.GetFlags() & CL_SHOW_WELCOME_PAGE) {
		m_theFrame->CreateWelcomePage();
	}

	//plugins must be loaded before the file explorer
	m_theFrame->LoadPlugins();

	//time to create the file explorer
	m_theFrame->GetFileExplorer()->Scan();

	m_theFrame->GetWorkspacePane()->GetNotebook()->SetAuiManager( &m_theFrame->GetDockingManager(), wxT("Workspace") );
	//load last session?
	if (m_theFrame->m_frameGeneralInfo.GetFlags() & CL_LOAD_LAST_SESSION && loadLastSession) {
		m_theFrame->LoadSession(wxT("Default"));
	}

	SetGccColourFunction( BuildTab::ColourGccLine );
}

Frame* Frame::Get()
{
	return m_theFrame;
}

void Frame::CreateGUIControls(void)
{
#ifdef __WXMSW__
	SetIcon(wxICON(aaaaa));
#else
	wxImage img(cubes_xpm);
	img.SetAlpha(cubes_alpha, true);
	wxBitmap bmp(img);
	wxIcon icon;
	icon.CopyFromBitmap(bmp);
	SetIcon(icon);
#endif

	// tell wxAuiManager to manage this frame
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_TRANSPARENT_DRAG);

#if defined (__WXGTK__) || defined (__WXMAC__)
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE);
#else
//	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
//	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
//	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
#endif

	//initialize debugger configuration tool
	DebuggerConfigTool::Get()->Load(ManagerST::Get()->GetStarupDirectory() + wxT("/config/debuggers.xml"), wxT("DebuggerSettings"));
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_TRANSPARENT_DRAG);

// On wx2.8.7, AUI dragging is broken but this happens only in debug build & on GTK
#if defined (__WXGTK__) && defined (__WXDEBUG__)
	m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING|wxAUI_MGR_ALLOW_ACTIVE_PANE|wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_RECTANGLE_HINT);
#endif

#ifdef __WXGTK__
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_VERTICAL);
#else
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);
#endif
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 6);

	// Load the menubar from XRC and set this frame's menubar to it.
	SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));

	//---------------------------------------------
	// Add docking windows
	//---------------------------------------------
	m_outputPane = new OutputPane(this, wxT("Output"));
	wxAuiPaneInfo paneInfo;
	m_mgr.AddPane(m_outputPane, paneInfo.Name(wxT("Output")).Caption(wxT("Output")).Bottom().Layer(1).Position(1).CloseButton(true).MinimizeButton());
	RegisterDockWindow(XRCID("output_pane"), wxT("Output"));

	// Add the explorer pane
	m_workspacePane = new WorkspacePane(this, wxT("Workspace"));
	m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo().
	              Name(m_workspacePane->GetCaption()).Caption(m_workspacePane->GetCaption()).
	              Left().BestSize(250, 300).Layer(2).Position(0).CloseButton(true));
	RegisterDockWindow(XRCID("workspace_pane"), wxT("Workspace"));

	//add the debugger locals tree, make it hidden by default
	m_debuggerPane = new DebuggerPane(this, wxT("Debugger"));
	m_mgr.AddPane(m_debuggerPane,
	              wxAuiPaneInfo().Name(m_debuggerPane->GetCaption()).Caption(m_debuggerPane->GetCaption()).Bottom().Layer(1).Position(1).CloseButton(true).Hide());
	RegisterDockWindow(XRCID("debugger_pane"), wxT("Debugger"));

	// Create the notebook for all the files
	long style = wxVB_TOP|wxVB_HAS_X|wxVB_BORDER|wxVB_TAB_DECORATION|wxVB_MOUSE_MIDDLE_CLOSE_TAB;
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	m_mgr.AddPane(m_book, wxAuiPaneInfo().Name(wxT("Editor")).
	              CenterPane().PaneBorder(true));

	// Connect the main notebook events
	GetNotebook()->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED, NotebookEventHandler(Frame::OnPageChanged), NULL, this);
	GetNotebook()->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSING, NotebookEventHandler(Frame::OnFileClosing), NULL, this);
	GetNotebook()->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSED, NotebookEventHandler(Frame::OnPageClosed), NULL, this);

	CreateViewAsSubMenu();
	CreateRecentlyOpenedFilesMenu();
	CreateRecentlyOpenedWorkspacesMenu();
	BuildSettingsConfigST::Get()->Load();

	//load dialog properties
	EditorConfigST::Get()->ReadObject(wxT("FindInFilesData"), &m_data);
	EditorConfigST::Get()->ReadObject(wxT("FindAndReplaceData"), &LEditor::GetFindReplaceData());
	EditorConfigST::Get()->ReadObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);

	TagsManager *tagsManager = TagsManagerST::Get();
#if defined (__WXMSW__) || defined (__WXGTK__)
	//start ctags process
	tagsManager->StartCtagsProcess();
#else
	// On Mac OSX, search the ctags-le in the correct path
	tagsManager->SetCtagsPath(wxStandardPaths::Get().GetDataDir());
#endif

	//--------------------------------------------------------------------------------------
	// Start the parsing thread, the parsing thread and the SymbolTree (or its derived)
	// Are connected. The constructor of SymbolTree, calls ParseThreadST::Get()->SetNotifyWindow(this)
	// to allows it to receive events for gui changes.
	//
	// If you wish to connect another object for it, simply call ParseThreadST::Get()->SetNotifyWindow(this)
	// with another object as 'this'
	//--------------------------------------------------------------------------------------
	ParseThreadST::Get()->Start();

	// Connect this tree to the parse thread
	ParseThreadST::Get()->SetNotifyWindow( this );

	// And finally create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(4);
	SetStatusBar(statusBar);

	GetStatusBar()->SetStatusText(wxT("Ready"));


	//update ctags options
	TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);

	//load windows perspective
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	if (options) {
		if (options->GetIconsSize() == 16) {
			CreateToolbars16();
		} else {
			CreateToolbars24();
		}
	} else {
		CreateToolbars24();
	}

	// load notebooks style
//	long book_style = 0;
//	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("Editor"));
//	if (book_style != wxNOT_FOUND) {
//		GetNotebook()->SetWindowStyleFlag(book_style);
//	}

//	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("OutputPane"));
//	if (book_style != wxNOT_FOUND) {
//		m_outputPane->GetNotebook()->SetWindowStyleFlag(book_style);
//	}

//	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("WorkspacePane"));
//	if (book_style != wxNOT_FOUND) {
//		m_workspacePane->GetNotebook()->SetWindowStyleFlag(book_style);
//	}

//	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("DebuggerPane"));
//	if (book_style != wxNOT_FOUND) {
//		m_debuggerPane->GetNotebook()->SetWindowStyleFlag(book_style);
//	}

	//load the tab right click menu
	m_tabRightClickMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));
	GetNotebook()->SetRightClickMenu(m_tabRightClickMenu);

	m_mgr.Update();
	SetAutoLayout (true);

	//load debuggers
	DebuggerMgr::Get().Initialize(this, EnvironmentConfig::Instance(), ManagerST::Get()->GetInstallDir());
	DebuggerMgr::Get().LoadDebuggers();

	wxString sessConfFile;
	sessConfFile << ManagerST::Get()->GetStarupDirectory() << wxT("/config/sessions.xml");
	SessionManager::Get().Load(sessConfFile);

	//try to locate the build tools
	///////////////////////////////////////////////////////

	//TODO:: temporarly disable to automatic tools update
	//due to bug in overriding user settings
	ManagerST::Get()->UpdateBuildTools();

	Layout();
}

void Frame::CreateViewAsSubMenu()
{
	//get the 'View As' menu

	int idx = GetMenuBar()->FindMenu(wxT("View"));
	if (idx != wxNOT_FOUND) {
		wxMenu *menu = GetMenuBar()->GetMenu(idx);
		wxMenu *submenu = new wxMenu();

		//create a view as sub menu and attach it
		wxMenuItem *item(NULL);

		int minId = viewAsMenuItemID;

		//load all lexers
		// load generic lexers
		EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
		for (; iter != EditorConfigST::Get()->LexerEnd(); iter++) {
			LexerConfPtr lex = iter->second;
			item = new wxMenuItem(submenu, minId, lex->GetName(), wxEmptyString, wxITEM_CHECK);
			m_viewAsMap[minId] = lex->GetName();
			minId++;
			submenu->Append(item);
		}
		menu->Append(viewAsSubMenuID, wxT("View As"), submenu);
	}
}

wxString Frame::GetViewAsLanguageById(int id) const
{
	if (m_viewAsMap.find(id) == m_viewAsMap.end()) {
		return wxEmptyString;
	}
	return m_viewAsMap.find(id)->second;
}

void Frame::CreateMenuBar()
{
	wxMenuBar *mb = new wxMenuBar();
	//File Menu
	wxMenu *menu = new wxMenu();
	menu->Append(XRCID("new_file"));
	menu->Append(XRCID("open_file"));
	menu->Append(XRCID("refresh_file"));
	menu->AppendSeparator();
	menu->Append(XRCID("save_file"));
	menu->Append(XRCID("save_file_as"));
	mb->Append(menu, wxT("&File"));

	SetMenuBar(mb);
}


void Frame::CreateToolbars24()
{
	wxAuiPaneInfo info;
	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(24, 24));
	tb->AddTool(XRCID("new_file"), wxT("New"), wxXmlResource::Get()->LoadBitmap(wxT("page_new24")), wxT("New File"));
	tb->AddTool(XRCID("open_file"), wxT("Open"), wxXmlResource::Get()->LoadBitmap(wxT("folder24")), wxT("Open File"));
	tb->AddTool(XRCID("refresh_file"), wxT("Reload"), wxXmlResource::Get()->LoadBitmap(wxT("reload24")), wxT("Reload File"));
	tb->AddSeparator();
	tb->AddTool(XRCID("save_file"), wxT("Save"), wxXmlResource::Get()->LoadBitmap(wxT("page_save24")), wxT("Save"));
	tb->AddTool(XRCID("save_file_as"), wxT("Save As"), wxXmlResource::Get()->LoadBitmap(wxT("save_as24")), wxT("Save As"));
	tb->AddTool(XRCID("save_all"), wxT("Save All"), wxXmlResource::Get()->LoadBitmap(wxT("save_all24")), wxT("Save All"));
	tb->AddSeparator();
	tb->AddTool(XRCID("close_file"), wxT("Close"), wxXmlResource::Get()->LoadBitmap(wxT("page_close24")), wxT("Close File"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT, wxT("Cut"), wxXmlResource::Get()->LoadBitmap(wxT("cut24")), wxT("Cut"));
	tb->AddTool(wxID_COPY, wxT("Copy"), wxXmlResource::Get()->LoadBitmap(wxT("copy24")), wxT("Copy"));
	tb->AddTool(wxID_PASTE, wxT("Paste"), wxXmlResource::Get()->LoadBitmap(wxT("paste24")), wxT("Paste"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO, wxT("Undo"), wxXmlResource::Get()->LoadBitmap(wxT("undo24")), wxT("Undo"));
	tb->AddTool(wxID_REDO, wxT("Redo"), wxXmlResource::Get()->LoadBitmap(wxT("redo24")), wxT("Redo"));
	tb->AddTool(wxID_BACKWARD, wxT("Backward"), wxXmlResource::Get()->LoadBitmap(wxT("arrow_back24")), wxT("Backward"));
	tb->AddTool(wxID_FORWARD, wxT("Forward"), wxXmlResource::Get()->LoadBitmap(wxT("arrow_next24")), wxT("Forward"));
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), wxXmlResource::Get()->LoadBitmap(wxT("bookmark24")), wxT("Toggle Bookmark"));

#if defined (__WXMAC__)
	tb->AddSeparator();
#endif

	tb->Realize();
	m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());

	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	info = wxAuiPaneInfo();
	tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(24, 24));

	tb->AddTool(wxID_FIND, wxT("Find"), wxXmlResource::Get()->LoadBitmap(wxT("find_and_replace24")), wxT("Find"));
	tb->AddTool(wxID_REPLACE, wxT("Replace"), wxXmlResource::Get()->LoadBitmap(wxT("refresh24")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"), wxXmlResource::Get()->LoadBitmap(wxT("find_in_files24")), wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_resource24")), wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"), wxT("Find Type In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_type24")), wxT("Find Type In Workspace"));
	tb->AddTool(XRCID("find_symbol"), wxT("Quick Outline"), wxXmlResource::Get()->LoadBitmap(wxT("outline24")), wxT("Show Current File Outline"));
#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();
	m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Search")).ToolbarPane().Top());

	//----------------------------------------------
	//create the build toolbar
	//----------------------------------------------
	tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(24, 24));

	tb->AddTool(XRCID("build_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("build_active_project24")), wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_build24")), wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("clean24")), wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("execute24")), wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_executed_program24")), wxT("Stop Running Program"));
#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();
	info = wxAuiPaneInfo();
	m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Build")).ToolbarPane().Top().Row(1));

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	m_debuggerTb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	m_debuggerTb->SetToolBitmapSize(wxSize(24, 24));
	m_debuggerTb->AddTool(XRCID("start_debugger"), wxT("Start / Continue debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_start24")), wxT("Start / Continue debugger"));
	m_debuggerTb->AddTool(XRCID("stop_debugger"), wxT("Stop debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stop24")), wxT("Stop debugger"));
	m_debuggerTb->AddTool(XRCID("pause_debugger"), wxT("Pause debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_pause24")), wxT("Pause debugger"));
	m_debuggerTb->AddSeparator();
	m_debuggerTb->AddTool(XRCID("dbg_stepin"), wxT("Step Into"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepin24")), wxT("Step In"));
	m_debuggerTb->AddTool(XRCID("dbg_next"), wxT("Next"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_next24")), wxT("Next"));
	m_debuggerTb->AddTool(XRCID("dbg_stepout"), wxT("Step Out"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepout24")), wxT("Step Out"));
#if defined (__WXMAC__)
	m_debuggerTb->AddSeparator();
#endif
	m_debuggerTb->Realize();
	info = wxAuiPaneInfo();
	m_mgr.AddPane(m_debuggerTb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Debug")).ToolbarPane().Top().Row(1));

	RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
	RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
	RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
	RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
	RegisterToolbar(XRCID("show_nav_toolbar"), wxT("Navigation Toolbar"));

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(800, -1), wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(24, 24));
	wxArrayString chcs;
	wxChoice *cbScope = new wxChoice(tb, wxID_ANY, wxDefaultPosition, wxSize(200, -1), chcs);
	tb->AddControl(cbScope);

	wxChoice *cbFunc = new wxChoice(tb, wxID_ANY, wxDefaultPosition, wxSize(600, -1), chcs);
	tb->AddControl(cbFunc);
#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();
	info = wxAuiPaneInfo();
	m_mgr.AddPane(tb, info.Name(wxT("Navigation Toolbar")).LeftDockable(false).RightDockable(false).Caption(wxT("Navigation Toolbar")).ToolbarPane().Top().Row(2));
	m_mainBook = new MainBook(cbFunc, cbScope);
}

void Frame::CreateToolbars16()
{
	wxAuiPaneInfo info;
	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(16, 16));
	tb->AddTool(XRCID("new_file"), wxT("New"), wxXmlResource::Get()->LoadBitmap(wxT("page_new16")), wxT("New File"));
	tb->AddTool(XRCID("open_file"), wxT("Open"), wxXmlResource::Get()->LoadBitmap(wxT("folder16")), wxT("Open File"));
	tb->AddTool(XRCID("refresh_file"), wxT("Reload"), wxXmlResource::Get()->LoadBitmap(wxT("reload16")), wxT("Reload File"));
	tb->AddSeparator();
	tb->AddTool(XRCID("save_file"), wxT("Save"), wxXmlResource::Get()->LoadBitmap(wxT("page_save16")), wxT("Save"));
	tb->AddTool(XRCID("save_file_as"), wxT("Save As"), wxXmlResource::Get()->LoadBitmap(wxT("save_as16")), wxT("Save As"));
	tb->AddTool(XRCID("save_all"), wxT("Save All"), wxXmlResource::Get()->LoadBitmap(wxT("save_all16")), wxT("Save All"));
	tb->AddSeparator();
	tb->AddTool(XRCID("close_file"), wxT("Close"), wxXmlResource::Get()->LoadBitmap(wxT("page_close16")), wxT("Close File"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT, wxT("Cut"), wxXmlResource::Get()->LoadBitmap(wxT("cut16")), wxT("Cut"));
	tb->AddTool(wxID_COPY, wxT("Copy"), wxXmlResource::Get()->LoadBitmap(wxT("copy16")), wxT("Copy"));
	tb->AddTool(wxID_PASTE, wxT("Paste"), wxXmlResource::Get()->LoadBitmap(wxT("paste16")), wxT("Paste"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO, wxT("Undo"), wxXmlResource::Get()->LoadBitmap(wxT("undo16")), wxT("Undo"));
	tb->AddTool(wxID_REDO, wxT("Redo"), wxXmlResource::Get()->LoadBitmap(wxT("redo16")), wxT("Redo"));
	tb->AddTool(wxID_BACKWARD, wxT("Backward"), wxXmlResource::Get()->LoadBitmap(wxT("arrow_back16")), wxT("Backward"));
	tb->AddTool(wxID_FORWARD, wxT("Forward"), wxXmlResource::Get()->LoadBitmap(wxT("arrow_next16")), wxT("Forward"));
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), wxXmlResource::Get()->LoadBitmap(wxT("bookmark16")), wxT("Toggle Bookmark"));

#if defined (__WXMAC__)
	tb->AddSeparator();
#endif

	tb->Realize();
	m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());

	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	info = wxAuiPaneInfo();
	tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(16, 16));

	tb->AddTool(wxID_FIND, wxT("Find"), wxXmlResource::Get()->LoadBitmap(wxT("find_and_replace16")), wxT("Find"));
	tb->AddTool(wxID_REPLACE, wxT("Replace"), wxXmlResource::Get()->LoadBitmap(wxT("refresh16")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"), wxXmlResource::Get()->LoadBitmap(wxT("find_in_files16")), wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_resource16")), wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"), wxT("Find Type In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_type16")), wxT("Find Type In Workspace"));
	tb->AddTool(XRCID("find_symbol"), wxT("Quick Outline"), wxXmlResource::Get()->LoadBitmap(wxT("outline16")), wxT("Show Current File Outline"));
#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();
	m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Search")).ToolbarPane().Top());

	//----------------------------------------------
	//create the build toolbar
	//----------------------------------------------
	tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(16, 16));

	tb->AddTool(XRCID("build_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("build_active_project16")), wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_build16")), wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("clean16")), wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("execute16")), wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_executed_program16")), wxT("Stop Running Program"));
#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();
	info = wxAuiPaneInfo();
	m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Build")).ToolbarPane().Top().Row(1));

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	m_debuggerTb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	m_debuggerTb->SetToolBitmapSize(wxSize(16, 16));
	m_debuggerTb->AddTool(XRCID("start_debugger"), wxT("Start / Continue debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_start16")), wxT("Start / Continue debugger"));
	m_debuggerTb->AddTool(XRCID("stop_debugger"), wxT("Stop debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stop16")), wxT("Stop debugger"));
	m_debuggerTb->AddTool(XRCID("pause_debugger"), wxT("Pause debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_pause16")), wxT("Pause debugger"));
	m_debuggerTb->AddSeparator();
	m_debuggerTb->AddTool(XRCID("dbg_stepin"), wxT("Step Into"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepin16")), wxT("Step In"));
	m_debuggerTb->AddTool(XRCID("dbg_next"), wxT("Next"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_next16")), wxT("Next"));
	m_debuggerTb->AddTool(XRCID("dbg_stepout"), wxT("Step Out"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepout16")), wxT("Step Out"));
#if defined (__WXMAC__)
	m_debuggerTb->AddSeparator();
#endif
	m_debuggerTb->Realize();
	info = wxAuiPaneInfo();
	m_mgr.AddPane(m_debuggerTb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Debug")).ToolbarPane().Top().Row(1));

	RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
	RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
	RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
	RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
	RegisterToolbar(XRCID("show_nav_toolbar"), wxT("Navigation Toolbar"));

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(800, -1), wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(16, 16));
	wxArrayString chcs;
	wxChoice *cbScope = new wxChoice(tb, wxID_ANY, wxDefaultPosition, wxSize(200, -1), chcs);
	tb->AddControl(cbScope);

	wxChoice *cbFunc = new wxChoice(tb, wxID_ANY, wxDefaultPosition, wxSize(600, -1), chcs);
	tb->AddControl(cbFunc);
#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();
	info = wxAuiPaneInfo();
	m_mgr.AddPane(tb, info.Name(wxT("Navigation Toolbar")).LeftDockable(false).RightDockable(false).Caption(wxT("Navigation Toolbar")).ToolbarPane().Top().Row(2));
	m_mainBook = new MainBook(cbFunc, cbScope);
}

void Frame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void Frame::DispatchCommandEvent(wxCommandEvent &event)
{
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(GetNotebook()->GetSelection()));
	if ( !editor ) {
		return;
	}
	if (event.GetId() >= viewAsMenuItemID && event.GetId() <= viewAsMenuItemMaxID) {
		//keep the old id as int and override the value set in the event object
		//to trick the event system
		event.SetInt(event.GetId());
		event.SetId(viewAsMenuItemID);
	}
	editor->OnMenuCommand(event);
}

void Frame::DispatchUpdateUIEvent(wxUpdateUIEvent &event)
{
	if ( GetNotebook()->GetPageCount() == 0 ) {
		event.Enable(false);
		return;
	}
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(GetNotebook()->GetSelection()));
	if ( !editor ) {
		event.Enable(false);
		return;
	}
	if (event.GetId() >= viewAsMenuItemID && event.GetId() <= viewAsMenuItemMaxID) {
		//keep the old id as int and override the value set in the event object
		//to trick the event system
		event.SetInt(event.GetId());
		event.SetId(viewAsMenuItemID);
	}
	editor->OnUpdateUI(event);
}

void Frame::OnFileExistUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(GetNotebook()->GetSelection()));
	if ( !editor ) {
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}

void Frame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxT("CodeLite"));

	wxString svnInfo;
	svnInfo << wxT("SVN Build, Revision: ") << SvnRevision;
	info.SetVersion(svnInfo);
	info.SetDescription(wxT("A lightweight cross-platform IDE for C/C++"));
	info.SetCopyright(wxT("(C) 2007-2008 By Eran Ifrah <eran.ifrah@gmail.com>"));
	info.SetWebSite(wxT("http://codelite.org/"));
	wxAboutBox(info);
}

void Frame::OnClose(wxCloseEvent& event)
{
	// Stop the search thread
	ManagerST::Get()->KillProgram();
	ManagerST::Get()->DbgStop();
	SearchThreadST::Get()->StopSearch();
	EditorConfigST::Get()->SavePerspective(wxT("Default"), m_mgr.SavePerspective());
	EditorConfigST::Get()->SaveNotebookStyle(wxT("Editor"), GetNotebook()->GetWindowStyleFlag());
	EditorConfigST::Get()->SaveNotebookStyle(wxT("OutputPane"), m_outputPane->GetNotebook()->GetWindowStyleFlag());
//	EditorConfigST::Get()->SaveNotebookStyle(wxT("WorkspacePane"), m_workspacePane->GetNotebook()->GetWindowStyleFlag());
	EditorConfigST::Get()->SaveNotebookStyle(wxT("DebuggerPane"), m_debuggerPane->GetNotebook()->GetWindowStyleFlag());
	EditorConfigST::Get()->SaveLexers();

	//save general information
	m_frameGeneralInfo.SetFrameSize(this->GetSize());
	m_frameGeneralInfo.SetFramePosition(this->GetScreenPosition());

	SetFrameFlag(IsMaximized(), CL_MAXIMIZE_FRAME);
	EditorConfigST::Get()->WriteObject(wxT("GeneralInfo"), &m_frameGeneralInfo);

	//save the 'find and replace' information
	if (m_findInFilesDlg) {
		EditorConfigST::Get()->WriteObject(wxT("FindInFilesData"), &(m_findInFilesDlg->GetData()));
	}
	if (LEditor::GetFindReplaceDialog()) {
		EditorConfigST::Get()->WriteObject(wxT("FindAndReplaceData"), &(LEditor::GetFindReplaceDialog()->GetData()));
	}

	//save the current session before closing
	SessionEntry session;
	session.SetSelectedTab(GetNotebook()->GetSelection());
	session.SetWorkspaceName(WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath());

	//loop over the open editors, and get their file name
	wxArrayString files;
	for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage((size_t)i));
		if (editor) {
			files.Add(editor->GetFileName().GetFullPath());
		}
	}

	session.SetTabs(files);
	SessionManager::Get().Save(wxT("Default"), session);

	//make sure there are no 'unsaved documents'
	ManagerST::Get()->CloseAll();
	event.Skip();
}

void Frame::LoadSession(const wxString &sessionName)
{
	SessionEntry session;
	if (!SessionManager::Get().FindSession(sessionName, session)) {
		return;
	}

	//load the workspace first (if any was opened)
	wxString wspFile = session.GetWorkspaceName();
	if (wspFile.IsEmpty() == false) {
		ManagerST::Get()->OpenWorkspace(wspFile);
	}

	//restore notebook tabs
	const wxArrayString &files = session.GetTabs();
	for (size_t i=0; i<files.GetCount(); i++) {
		ManagerST::Get()->OpenFile(files.Item(i), wxEmptyString);
	}

	//set selected tab
	int selection = session.GetSelectedTab();
	if (selection >= 0 && selection < (int)GetNotebook()->GetPageCount()) {
		GetNotebook()->SetSelection(selection);
	}
}

void Frame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	if (!GetNotebook()->GetCurrentPage())
		return;

	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetCurrentPage());
	if ( !editor )
		return;

	// SaveFile contains the logic of "Untiltled" files
	editor->SaveFile();
}

void Frame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	if (!GetNotebook()->GetCurrentPage())
		return;

	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetCurrentPage());
	if ( !editor )
		return;

	editor->SaveFileAs();
}

void Frame::OnFileReload(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (!GetNotebook()->GetCurrentPage())
		return;

	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetCurrentPage());
	if ( !editor )
		return;

	editor->ReloadFile();
}

void Frame::OnCloseWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen()) {
		ManagerST::Get()->CloseWorkspace();
		GetConfigChoice()->Enable(false);
	}
}

void Frame::OnSwitchWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	const wxString ALL(wxT("CodeLite Workspace files (*.workspace)|*.workspace|")
	                   wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Workspace"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		ManagerST::Get()->OpenWorkspace(dlg->GetPath());
	}
	dlg->Destroy();
}

//------------------------------------------------------
// Complete word, complete a word from the current caret
// position.
// The list of words, is constructed from the database &
// from the local scope
//------------------------------------------------------
void Frame::OnCompleteWord(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetCurrentPage());
	if ( !editor )
		return;

	editor->CompleteWord();
}

void Frame::OnBuildExternalDatabase(wxCommandEvent& WXUNUSED(event))
{
	DoBuildExternalDatabase();
}

void Frame::DoBuildExternalDatabase()
{
	ExtDbData data;
	ExtDbWizard *wiz = new ExtDbWizard(this, wxID_ANY);
	if (wiz->Run(data)) {
		// build the external database
		wxFileName dbname(ManagerST::Get()->GetStarupDirectory() + PATH_SEP + data.dbName);
		data.dbName = dbname.GetFullPath();
		TagsManagerST::Get()->BuildExternalDatabase(data);

		if (data.attachDb) {
			ManagerST::Get()->SetExternalDatabase(dbname);
		}
	}
	wiz->Destroy();
}

void Frame::OnCloseExternalDatabase(wxCommandEvent& WXUNUSED(event))
{
	ManagerST::Get()->CloseExternalDatabase();
}

void Frame::OnUseExternalDatabase(wxCommandEvent& WXUNUSED(event))
{
	const wxString ALL(	wxT("Tags Database File (*.tags)|*.tags|")
	                    wxT("All Files (*)|*"));

	//set the default current directory to the working directory of
	//lite editor
	wxFileDialog *dlg = new wxFileDialog(this,
	                                     wxT("Select an external database:"),
	                                     ManagerST::Get()->GetStarupDirectory(),
	                                     wxEmptyString,
	                                     ALL,
	                                     wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE ,
	                                     wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK) {
		// get the path
		wxFileName dbname(dlg->GetPath());
		ManagerST::Get()->SetExternalDatabase(dbname);
	}
	dlg->Destroy();
}

// Open new file
void Frame::OnFileNew(wxCommandEvent &event)
{
	static int fileCounter = 0;
	wxUnusedVar(event);

	wxString fileNameStr(wxT("Untitled"));
	fileNameStr << ++fileCounter;
	wxFileName fileName(fileNameStr);

	GetNotebook()->Freeze();
	//allocate new editor instance using the creator
	//this is done due to low performance on GTK
	LEditor *editor = EditorCreatorST::Get()->NewInstance();
	editor->SetFileName(fileName);

	GetNotebook()->AddPage(editor, fileName.GetFullName(), wxNullBitmap, true);
	GetNotebook()->Thaw();
	editor->Show(true);
	editor->SetActive();
}

void Frame::OnFileOpen(wxCommandEvent & WXUNUSED(event))
{
	const wxString ALL(	wxT("All Files (*)|*"));
	static wxString s_openPath(wxEmptyString);

	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open File"), s_openPath, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg->GetPaths(paths);
		for (size_t i=0; i<paths.GetCount(); i++) {
			ManagerST::Get()->OpenFile(paths.Item(i), wxEmptyString);
		}

		if (paths.GetCount() > 0) {
			wxFileName fn(paths.Item(0));
			s_openPath = fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
		}
	}
	dlg->Destroy();
	GetOpenWindowsPane()->UpdateList();
}

void Frame::OnFileClose(wxCommandEvent &event)
{
	wxUnusedVar( event );
	RemoveCppMenu();
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetCurrentPage());
	if ( editor ) {
		bool veto;
		ClosePage(editor, false, GetNotebook()->GetSelection(), true, veto);
	} else {
		GetNotebook()->DeletePage((size_t) GetNotebook()->GetSelection());
	}
	
	//this function does not send notification about page deletion, so we need to manually add the CppMenu()
	AddCppMenu();
	
	GetOpenWindowsPane()->UpdateList();
	GetMainBook()->Clear();
}

void Frame::OnFileClosing(NotebookEvent &event)
{
	// get the page that is now closing

	//always remove the c++ menu
	RemoveCppMenu();

	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(event.GetSelection()));
	if ( !editor )
		return;

	bool veto;
	ClosePage(editor, true, event.GetSelection(), false, veto);
	if ( veto ) {
		event.Veto();
	} else {
		//update the symbol view
		if (!editor->GetProject().IsEmpty()) {
			GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
		}
	}

	//update the titlebar
	wxString title(wxT("CodeLite - Revision: "));
	title << SvnRevision;
	SetTitle(title);
	event.Skip();
}

void Frame::OnPageChanged(NotebookEvent &event)
{
	// pass the event to the editor
	wxString title(wxT("CodeLite - Revision: "));
	title << SvnRevision;

	LEditor *editor = dynamic_cast<LEditor*>( GetNotebook()->GetPage(event.GetSelection()) );
	if ( !editor ) {
		SetTitle(title);
		RemoveCppMenu();
		return;
	}

	if ( editor->GetContext()->GetName() == wxT("C++") ) {
		AddCppMenu();
	} else {
		RemoveCppMenu();
	}

	//update the symbol view as well in case we are in a workspace context
	if (!editor->GetProject().IsEmpty()) {
		GetWorkspacePane()->DisplaySymbolTree(editor->GetFileName());
		GetWorkspacePane()->GetFileViewTree()->ExpandToPath(editor->GetProject(), editor->GetFileName());
		if (GetFileExplorer()->GetIsLinkedToEditor()) {
			GetFileExplorer()->GetFileTree()->ExpandToPath(editor->GetFileName());
		}
	}

	title << wxT(" - ") << editor->GetFileName().GetFullPath();
	SetTitle(title);

	editor->SetActive();
	GetOpenWindowsPane()->SyncSelection();
	event.Skip();
}

void Frame::OnPageClosed(NotebookEvent &event)
{
	wxUnusedVar(event);
	GetOpenWindowsPane()->UpdateList();
	//clean the navigation bar
	GetMainBook()->Clear();
}

void Frame::OnFileSaveAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->SaveAll();
}

void Frame::OnCompleteWordUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = ManagerST::Get()->GetActiveEditor();

	// This menu item is enabled only if the current editor
	// belongs to a project
	event.Enable(editor && ManagerST::Get()->IsWorkspaceOpen());
}

void Frame::ClosePage(LEditor *editor, bool notify, size_t index, bool doDelete, bool &veto)
{
	veto = false;
	if(index == Notebook::npos) return;
	
	if ( editor->GetModify() ) {
		// prompt user to save file
		wxString msg;
		msg << wxT("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
		int answer = wxMessageBox(msg, wxT("Confirm"),wxYES_NO | wxCANCEL);
		switch ( answer ) {
		case wxYES: {
			// try to save the file, if an error occured, return without
			// closing the tab
			if ( !editor->SaveFile() ) {
				//we faild in saving the file, dont allow the tab removal
				//process to continue
				veto = true;
				return;
			} else {
				if ( doDelete ) {
					GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
					GetNotebook()->DeletePage(index, notify);
				}
			}
		}
		break;
		case wxCANCEL:
			veto = true;
			return; // do nothing
		case wxNO:
			// just delete the tab without saving the changes
			if ( doDelete ) {
				GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
				GetNotebook()->DeletePage(index, notify);
			}
			break;
		}
	} else {
		// file is not modified, just remove the tab
		if ( doDelete ) {
			GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
			GetNotebook()->DeletePage(index, notify);
		} // if( doDelete )
	}
}

void Frame::OnSearchThread(wxCommandEvent &event)
{
	if (m_doingReplaceInFiles) {
		//Incase this flag is on, it means that the find in files dialog
		//was invoked with 'find replace candidates
		//which requires different handling code

		//the only event that reallty interesting us, is the match find
		if (event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHCANCELED || event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHEND) {
			m_doingReplaceInFiles = false;
		} else if (event.GetEventType() == wxEVT_SEARCH_THREAD_MATCHFOUND) {
			//add an entry to the replace panel
			SearchResultList *res = (SearchResultList*)event.GetClientData();

			//res will be deleted by AddResult
			GetOutputPane()->GetReplaceResultsTab()->AddResults(res);

		} else if (event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHSTARTED) {
			ManagerST::Get()->ShowOutputPane(OutputPane::REPLACE_IN_FILES);
			GetOutputPane()->GetReplaceResultsTab()->Clear();
		}
	} else {

		m_outputPane->GetFindResultsTab()->CanFocus(false);
		if ( event.GetEventType() == wxEVT_SEARCH_THREAD_MATCHFOUND) {
			SearchResultList *res = (SearchResultList*)event.GetClientData();
			SearchResultList::iterator iter = res->begin();

			wxString msg;
			for (; iter != res->end(); iter++) {
				msg.Append((*iter).GetMessage() + wxT("\n"));
			}
			m_outputPane->GetFindResultsTab()->AppendText(msg);
			delete res;
		} else if (event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHCANCELED) {
			m_outputPane->GetFindResultsTab()->AppendText(event.GetString() + wxT("\n"));
		} else if (event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHSTARTED) {
			// make sure that the output pane is visible and selection
			// is set to the 'Find In Files' tab
			ManagerST::Get()->ShowOutputPane(OutputPane::FIND_IN_FILES_WIN);

			//set the request find results tab to use
			GetOutputPane()->SetFindResultsTab(event.GetInt());

			m_outputPane->GetFindResultsTab()->Clear();
			m_outputPane->GetFindResultsTab()->AppendText(event.GetString() + wxT("\n"));

		} else if (event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHEND) {

			SearchSummary *summary = (SearchSummary*)event.GetClientData();
			m_outputPane->GetFindResultsTab()->AppendText(summary->GetMessage() + wxT("\n"));
			delete summary;

		}
		m_outputPane->GetFindResultsTab()->CanFocus(true);
	}
}

void Frame::OnFindInFiles(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if ( m_findInFilesDlg == NULL ) {
		m_findInFilesDlg = new FindInFilesDialog(this, m_data);
	}

	if (m_doingReplaceInFiles) {
		wxMessageBox(wxT("The search thread is currently busy in 'replace in files' operations"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
		return;
	}

	m_findInFilesDlg->SetEventOwner(GetEventHandler());
	if ( m_findInFilesDlg->IsShown() ) {
		// make sure that dialog has focus and that this instace
		m_findInFilesDlg->SetFocus();
		return;
	}

	//if we have an open editor, and a selected text, make this text
	//the search string
	if (GetNotebook()->GetPageCount() > 0) {
		LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage((size_t)GetNotebook()->GetSelection()));
		if (editor) {
			wxString selText = editor->GetSelectedText();
			if (selText.IsEmpty() == false) {
				m_findInFilesDlg->GetData().SetFindString(selText);
			}
		}
	}
	m_findInFilesDlg->Show();
}

void Frame::OnWorkspaceOpen(wxUpdateUIEvent &event)
{
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

// Project->New Workspace
void Frame::OnProjectNewWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	NewWorkspaceDlg *dlg = new NewWorkspaceDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
		wxString fullname = dlg->GetFilePath();

		wxFileName fn(fullname);
		ManagerST::Get()->CreateWorkspace(fn.GetName(), fn.GetPath());
	}
	dlg->Destroy();
}

// Project->New Project
void Frame::OnProjectNewProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	NewProjectDlg *dlg = new NewProjectDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
		ProjectData data = dlg->GetProjectData();
		ManagerST::Get()->CreateProject(data);
	}
	dlg->Destroy();
}

void Frame::OnProjectAddProject(wxCommandEvent &event)
{
	wxUnusedVar(event);

	// Prompt user for project path
	const wxString ALL(	wxT("CodeLite Projects (*.project)|*.project|")
	                    wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Project"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		// Open it
		ManagerST::Get()->AddProject(dlg->GetPath());
	}
	dlg->Destroy();
}

void Frame::OnCtagsOptions(wxCommandEvent &event)
{
	wxUnusedVar(event);

	bool colVars(false);
	bool colTags(false);
	bool newColVars(false);
	bool newColTags(false);

	colVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
	colTags = (m_tagsOptionsData.GetFlags() & CC_COLOUR_PROJ_TAGS ? true : false);

	TagsOptionsDlg *dlg = new TagsOptionsDlg(this, m_tagsOptionsData);
	if (dlg->ShowModal() == wxID_OK) {
		TagsManager *tagsMgr = TagsManagerST::Get();
		m_tagsOptionsData = dlg->GetData();

		newColVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
		newColTags = (m_tagsOptionsData.GetFlags() & CC_COLOUR_PROJ_TAGS ? true : false);

		tagsMgr->SetCtagsOptions(m_tagsOptionsData);
		EditorConfigST::Get()->WriteObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);

		//do we need to colourise?
		if (newColTags != colTags || newColVars != colVars) {
			for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
				LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(i));
				if (editor) {
					editor->UpdateColours();
				}
			}
		}
	}
	dlg->Destroy();
}

void Frame::RegisterToolbar(int menuItemId, const wxString &name)
{
	m_toolbars[menuItemId] = name;
	Connect(menuItemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnViewToolbar), NULL, this);
	Connect(menuItemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Frame::OnViewToolbarUI), NULL, this);
}

void Frame::RegisterDockWindow(int menuItemId, const wxString &name)
{
	m_panes[menuItemId] = name;
	Connect(menuItemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnViewPane), NULL, this);
	Connect(menuItemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Frame::OnViewPaneUI), NULL, this);
}

void Frame::OnViewToolbar(wxCommandEvent &event)
{
	std::map<int, wxString>::iterator iter = m_toolbars.find(event.GetId());
	if (iter != m_toolbars.end()) {
		ViewPane(iter->second, event);
	}
}

void Frame::OnViewToolbarUI(wxUpdateUIEvent &event)
{
	std::map<int, wxString>::iterator iter = m_toolbars.find(event.GetId());
	if (iter != m_toolbars.end()) {
		ViewPaneUI(iter->second, event);
	}
}

void Frame::OnViewPane(wxCommandEvent &event)
{
	std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
	if (iter != m_panes.end()) {
		ViewPane(iter->second, event);
	}
}

void Frame::OnViewPaneUI(wxUpdateUIEvent &event)
{
	std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
	if (iter != m_panes.end()) {
		ViewPaneUI(iter->second, event);
	}
}

void Frame::ViewPane(const wxString &paneName, wxCommandEvent &event)
{
	wxAuiPaneInfo &info = m_mgr.GetPane(paneName);
	if (info.IsOk()) {
		if ( event.IsChecked() ) {
			info.Show();
		} else {
			info.Hide();
		}
		m_mgr.Update();
	}

}

void Frame::ViewPaneUI(const wxString &paneName, wxUpdateUIEvent &event)
{
	wxAuiPaneInfo &info = m_mgr.GetPane(paneName);
	if (info.IsOk()) {
		event.Check(info.IsShown());
	}
}

void Frame::OnViewOptions(wxCommandEvent & WXUNUSED( event))
{
	OptionsDlg *dlg = new OptionsDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnConfigurationManager(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ShowBuildConfigurationManager();
}

void Frame::OnTogglePanes(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->TogglePanes();
}

void Frame::OnAddEnvironmentVariable(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarsTableDlg *dlg = new EnvVarsTableDlg(this);
	dlg->ShowModal();

	if (ManagerST::Get()->IsWorkspaceOpen()) {
		//mark all the projects as dirty
		wxArrayString projects;
		WorkspaceST::Get()->GetProjectList( projects );
		for ( size_t i=0; i< projects.size(); i++ ) {
			ProjectPtr proj = ManagerST::Get()->GetProject( projects.Item(i) );
			if ( proj ) {
				proj->SetModified( true );
			}
		}
	}

	dlg->Destroy();
}

void Frame::OnAdvanceSettings(wxCommandEvent &event)
{
	size_t selected_page(0);
	if (event.GetInt() == 1) {
		selected_page = 1;
	}

	AdvancedDlg *dlg = new AdvancedDlg(this, selected_page);
	if (dlg->ShowModal() == wxID_OK) {
		//mark the whole workspace as dirty so makefile generation will take place
		//force makefile generation upon configuration change
		if (ManagerST::Get()->IsWorkspaceOpen()) {
			wxArrayString projs;
			ManagerST::Get()->GetProjectList(projs);
			for ( size_t i=0; i< projs.GetCount(); i++ ) {
				ProjectPtr proj = ManagerST::Get()->GetProject( projs.Item(i) );
				if ( proj ) {
					proj->SetModified(true);
				}
			}
		}
	}
	dlg->Destroy();
}

void Frame::OnBuildEvent(wxCommandEvent &event)
{
	// make sure that the output pane is visible and selection
	// is set to the 'Find In Files' tab
	m_outputPane->GetBuildTab()->CanFocus(true);
	if (event.GetEventType() == wxEVT_BUILD_STARTED) {
		ManagerST::Get()->ShowOutputPane(OutputPane::BUILD_WIN);
		m_outputPane->GetBuildTab()->Clear();
		//read settings for the build output tab
		m_outputPane->GetBuildTab()->ReloadSettings();
		m_outputPane->GetBuildTab()->AppendText(wxT("Building: "));
	} else if (event.GetEventType() == wxEVT_BUILD_ADDLINE) {
		m_outputPane->GetBuildTab()->AppendText(event.GetString());
	} else if (event.GetEventType() == wxEVT_BUILD_ENDED) {
		m_outputPane->GetBuildTab()->AppendText(BUILD_END_MSG);
		m_outputPane->GetBuildTab()->OnBuildEnded();

		//If the build process was part of a 'Build and Run' command, check whether an erros
		//occured during build process, if non, launch the output
		if (m_buildInRun) {
			m_buildInRun = false;
			if (!ManagerST::Get()->IsBuildEndedSuccessfully()) {
				//build ended with errors
				if (wxMessageBox(wxT("Build ended with errors. Continue?"), wxT("Confirm"), wxYES_NO| wxICON_QUESTION) == wxYES) {
					ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
				}
			} else {
				//no errors, execute!
				ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
			}
		} else if (m_rebuild) {
			//are we rebuilding?
			m_rebuild = false;
			ManagerST::Get()->BuildProject(ManagerST::Get()->GetActiveProjectName());
		}

		//give back the focus to the editor
		LEditor *editor = ManagerST::Get()->GetActiveEditor();
		if (editor) {
			editor->SetActive();
		}
	}
}

void Frame::OnOutputWindowEvent(wxCommandEvent &event)
{
	// make sure that the output pane is visible and selection
	// is set to the 'Find In Files' tab
	ManagerST::Get()->ShowOutputPane(OutputPane::OUTPUT_WIN);
	if (event.GetEventType() == wxEVT_ASYNC_PROC_STARTED) {
		m_outputPane->GetOutputWindow()->Clear();
		m_outputPane->GetOutputWindow()->AppendLine(event.GetString());
	} else if (event.GetEventType() == wxEVT_ASYNC_PROC_ADDLINE) {
		m_outputPane->GetOutputWindow()->AppendLine(event.GetString());
	} else if (event.GetEventType() == wxEVT_ASYNC_PROC_ENDED) {
		m_outputPane->GetOutputWindow()->AppendLine(event.GetString());
	}
}

// Build operations
void Frame::OnBuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {
		ManagerST::Get()->BuildProject(ManagerST::Get()->GetActiveProjectName());
	}
}

void Frame::OnBuildAndRunProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {
		m_buildInRun = true;
		ManagerST::Get()->BuildProject(ManagerST::Get()->GetActiveProjectName());
	}
}

void Frame::OnRebuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {
		m_rebuild = true;
		ManagerST::Get()->CleanProject(ManagerST::Get()->GetActiveProjectName());
	}
}

void Frame::OnBuildProjectUI(wxUpdateUIEvent &event)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	event.Enable(enable);
}

void Frame::OnStopExecutedProgramUI(wxUpdateUIEvent &event)
{
	Manager *mgr = ManagerST::Get();
	bool enable = mgr->IsProgramRunning();
	event.Enable(enable);
}

void Frame::OnStopBuildUI(wxUpdateUIEvent &event)
{
	Manager *mgr = ManagerST::Get();
	bool enable = mgr->IsBuildInProgress();
	event.Enable(enable);
}

void Frame::OnStopBuild(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsBuildInProgress()) {
		mgr->StopBuild();
	}
}

void Frame::OnStopExecutedProgram(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsProgramRunning()) {
		mgr->KillProgram();
	}
}

void Frame::OnCleanProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->CleanProject(ManagerST::Get()->GetActiveProjectName());
}

void Frame::OnCleanProjectUI(wxUpdateUIEvent &event)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	event.Enable(enable);
}

void Frame::OnExecuteNoDebug(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString projectName;

	projectName = ManagerST::Get()->GetActiveProjectName();
	if (projectName.IsEmpty() == false) {
		ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
	}
}

void Frame::OnExecuteNoDebugUI(wxUpdateUIEvent &event)
{
	event.Enable(ManagerST::Get()->GetActiveProjectName().IsEmpty() == false
	             &&
	             !ManagerST::Get()->IsBuildInProgress()
	             &&
	             !ManagerST::Get()->IsProgramRunning());
}

void Frame::OnWorkspaceConfigChanged(wxCommandEvent &event)
{
	wxString selectionStr = event.GetString();

	//update the workspace configuration file
	ManagerST::Get()->SetWorkspaceConfigurationName(selectionStr);

	//force makefile generation upon configuration change
	wxArrayString projs;
	ManagerST::Get()->GetProjectList(projs);
	for ( size_t i=0; i< projs.GetCount(); i++ ) {
		ProjectPtr proj = ManagerST::Get()->GetProject( projs.Item(i) );
		if ( proj ) {
			proj->SetModified(true);
		}
	}
}

void Frame::OnTimer(wxTimerEvent &event)
{
	static bool first(true);
	//Attach external database symbol
	if (first) {
		first = false;

		if (m_tagsOptionsData.GetFlags() & CC_LOAD_EXT_DB) {
			//load the recently opened external database
			wxString tagDb = EditorConfigST::Get()->GetTagsDatabase();
			if (tagDb.IsEmpty() == false) {
				wxFileName dbname(tagDb);

				//as part of the change from the .db to .tags, rename the entry in the
				//configuratin file as well
				if (dbname.GetExt() == wxT("db")) {
					dbname.SetExt(wxT("tags"));
					EditorConfigST::Get()->SetTagsDatabase(dbname.GetFullPath());
				}

				//if the database will be loaded to memory, display a busy dialog
				ManagerST::Get()->SetExternalDatabase(dbname);
			}
			AutoLoadExternalDb();
		}
		//send initialization end event
		SendCmdEvent(wxEVT_INIT_DONE);
	}

	//clear navigation queue
	if (GetNotebook()->GetPageCount() == 0) {
		NavMgr::Get()->Clear();
	}
	event.Skip();
}

void Frame::OnFileCloseAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->CloseAll();

	//update the title bar
	wxString title(wxT("CodeLite - Revision: "));
	title << SvnRevision;
	SetTitle(title);

	GetMainBook()->Clear();
	GetOpenWindowsPane()->UpdateList();
	GetWorkspacePane()->CollpaseAll();
	
	RemoveCppMenu();
}

void Frame::OnQuickOutline(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen() == false)
		return;

	if (!ManagerST::Get()->GetActiveEditor())
		return;

	if (ManagerST::Get()->GetActiveEditor()->GetProject().IsEmpty())
		return;

	QuickOutlineDlg *dlg = new QuickOutlineDlg(this, ManagerST::Get()->GetActiveEditor()->GetFileName().GetFullPath());
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnFindType(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen() == false)
		return;

	OpenTypeDlg *dlg = new OpenTypeDlg(this, TagsManagerST::Get());
	if (dlg->ShowModal() == wxID_OK) {
		TagEntryPtr tag = dlg->GetSelectedTag();
		if (tag && tag->IsOk()) {
			ManagerST::Get()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
		}
	}
	dlg->Destroy();
}

void Frame::OnFindResource(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen() == false)
		return;

	OpenResourceDlg *dlg = new OpenResourceDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
		wxString fileName = dlg->GetFileName();
		if (fileName.IsEmpty() == false) {
			wxString projectName = ManagerST::Get()->GetProjectNameByFile(fileName);
			ManagerST::Get()->OpenFile(fileName, projectName);
		}
	}
	dlg->Destroy();
}

/*
void Frame::OnImportMakefileUI(wxUpdateUIEvent &event)
{
	event.Enable(false);
}

void Frame::OnImportMakefile(wxCommandEvent &event)
{
	wxUnusedVar(event);
	const wxString ALL(wxT("Import makefile files (Makefile) |Makefile|")
	                   wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Makefile"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		ManagerST::Get()->ImportFromMakefile(dlg->GetPath());
	}
	dlg->Destroy();
}
*/

void Frame::OnAddSymbols(SymbolTreeEvent &event)
{
	SymbolTree *tree = GetWorkspacePane()->GetSymbolTree();
	if (tree)
		tree->AddSymbols(event);
}

void Frame::OnDeleteSymbols(SymbolTreeEvent &event)
{
	SymbolTree *tree = GetWorkspacePane()->GetSymbolTree();
	if (tree)
		tree->DeleteSymbols(event);
}

void Frame::OnUpdateSymbols(SymbolTreeEvent &event)
{
	SymbolTree *tree = GetWorkspacePane()->GetSymbolTree();
	if (tree)
		tree->UpdateSymbols(event);
}

wxString Frame::CreateWorkspaceTable()
{
	wxString html;
	wxArrayString files;
	Manager *mgr = ManagerST::Get();
	mgr->GetRecentlyOpenedWorkspaces(files);

	wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	bgclr = DrawingUtils::LightColour(bgclr, 70);

	html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
	if (files.GetCount() == 0) {
		html << wxT("<tr><td><font size=2 face=\"Verdana\">");
		html << wxT("No workspaces found.") << wxT("</font></td></tr>");
	} else {
		for (size_t i=0; i<files.GetCount(); i++) {
			html << wxT("<tr><td><font size=2 face=\"Verdana\">");
			html << wxT("<a href=\"action:open-file:") << files.Item(i) << wxT("\" >") << files.Item(i) << wxT("</a></font></td></tr>");
		}
	}
	html << wxT("</table>");
	return html;
}

wxString Frame::CreateFilesTable()
{
	wxString html;
	wxArrayString files;
	Manager *mgr = ManagerST::Get();
	mgr->GetRecentlyOpenedFiles(files);

	wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	bgclr = DrawingUtils::LightColour(bgclr, 70);
	html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
	if (files.GetCount() == 0) {
		html << wxT("<tr><td><font size=2 face=\"Verdana\">");
		html << wxT("No files found.") << wxT("</font></td></tr>");
	} else {
		for (size_t i=0; i<files.GetCount(); i++) {
			html << wxT("<tr><td><font size=2 face=\"Verdana\">");
			html << wxT("<a href=\"action:open-file:") << files.Item(i) << wxT("\" >") << files.Item(i) << wxT("</a></font></td></tr>");
		}
	}
	html << wxT("</table>");
	return html;
}

void Frame::CreateRecentlyOpenedFilesMenu()
{
	wxArrayString files;
	Manager *mgr = ManagerST::Get();
	FileHistory &hs = mgr->GetRecentlyOpenedFilesClass();
	mgr->GetRecentlyOpenedFiles(files);

	int idx = GetMenuBar()->FindMenu(wxT("File"));
	if (idx != wxNOT_FOUND) {
		wxMenu *menu = GetMenuBar()->GetMenu(idx);
		wxMenu *submenu = NULL;
		wxMenuItem *item = menu->FindItem(XRCID("recent_files"));
		if (item) {
			submenu = item->GetSubMenu();
		}

		if (submenu) {
			for (size_t i=0; i<files.GetCount(); i++) {
				hs.AddFileToHistory(files.Item(i));
			}
			//set this menu as the recent file menu
			hs.SetBaseId(RecentFilesSubMenuID+1);
			hs.UseMenu(submenu);
			hs.AddFilesToMenu();
		}
	}
}

void Frame::CreateRecentlyOpenedWorkspacesMenu()
{
	wxArrayString files;
	Manager *mgr = ManagerST::Get();
	FileHistory &hs = mgr->GetRecentlyOpenedWorkspacesClass();
	mgr->GetRecentlyOpenedWorkspaces(files);

	int idx = GetMenuBar()->FindMenu(wxT("File"));
	if (idx != wxNOT_FOUND) {
		wxMenu *menu = GetMenuBar()->GetMenu(idx);
		wxMenu *submenu = NULL;
		wxMenuItem *item = menu->FindItem(XRCID("recent_workspaces"));
		if (item) {
			submenu = item->GetSubMenu();
		}

		if (submenu) {
			for (size_t i=0; i<files.GetCount(); i++) {
				hs.AddFileToHistory(files.Item(i));
			}
			//set this menu as the recent file menu
			hs.SetBaseId(RecentWorkspaceSubMenuID+1);
			hs.UseMenu(submenu);
			hs.AddFilesToMenu();
		}
	}
}

void Frame::OnRecentFile(wxCommandEvent &event)
{
	size_t idx = event.GetId() - (RecentFilesSubMenuID+1);
	Manager *mgr = ManagerST::Get();
	FileHistory &fh = mgr->GetRecentlyOpenedFilesClass();

	wxArrayString files;
	fh.GetFiles(files);

	if (idx < files.GetCount()) {
		wxString projectName = mgr->GetProjectNameByFile(files.Item(idx));
		mgr->OpenFile(files.Item(idx), projectName);
	}
}

void Frame::OnRecentWorkspace(wxCommandEvent &event)
{
	size_t idx = event.GetId() - (RecentWorkspaceSubMenuID+1);
	Manager *mgr = ManagerST::Get();
	FileHistory &fh = mgr->GetRecentlyOpenedWorkspacesClass();

	wxArrayString files;
	fh.GetFiles(files);

	if (idx < files.GetCount()) {
		mgr->OpenWorkspace(files.Item(idx));
	}
}

void Frame::OnBackwardForward(wxCommandEvent &event)
{
	switch (event.GetId()) {
	case wxID_FORWARD: {
		BrowseRecord rec = NavMgr::Get()->GetNextRecord();
		ManagerST::Get()->OpenFile(rec);
	}
	break;
	case wxID_BACKWARD:
		NavMgr::Get()->NavigateBackward();
		break;
	default:
		break;
	}
}

void Frame::OnBackwardForwardUI(wxUpdateUIEvent &event)
{
	if (event.GetId() == wxID_FORWARD) {
		event.Enable(NavMgr::Get()->CanNext());
	} else if (event.GetId() == wxID_BACKWARD) {
		event.Enable(NavMgr::Get()->CanPrev());
	} else {
		event.Skip();
	}
}

void Frame::OnFixDatabasePaths(wxCommandEvent &event)
{
	wxUnusedVar(event);
	const wxString ALL(	wxT("Tags Database File (*.tags)|*.tags|")
	                    wxT("All Files (*)|*"));

	wxFileDialog *fdlg = new wxFileDialog(this,
	                                      wxT("Select an external database:"),
	                                      ManagerST::Get()->GetStarupDirectory(),
	                                      wxEmptyString,
	                                      ALL,
	                                      wxOPEN | wxFILE_MUST_EXIST,
	                                      wxDefaultPosition);

	if (fdlg->ShowModal() == wxID_OK) {
		// get the path
		wxFileName dbname(fdlg->GetPath());
		TagsDatabase db;
		db.OpenDatabase(dbname);

		KeyValueTableDlg *dlg = new KeyValueTableDlg(this, &db);
		dlg->ShowModal();

		//notify tags manager to reload the file paths from the database
		TagsManagerST::Get()->ReloadExtDbPaths();

		dlg->Destroy();
	}
	fdlg->Destroy();
}

void Frame::OnFixDatabasePathsUI(wxUpdateUIEvent &event)
{
	event.Enable(true);
}

void Frame::CreateWelcomePage()
{
	Manager *mgr = ManagerST::Get();
	//load the template
	wxFileName fn(mgr->GetStarupDirectory(), wxT("index.html"));
	wxFFile file(fn.GetFullPath(), wxT("r"));
	if (!file.IsOpened()) {
		return;
	}
	m_welcomePage = new wxHtmlWindow(GetNotebook(), wxID_ANY);

	wxString content;
	file.ReadAll(&content);
	file.Close();

	//replace $(InstallPath)
	content.Replace(wxT("$(InstallPath)"), mgr->GetStarupDirectory());

	//replace the $(FilesTable) & $(WorkspaceTable)
	wxString workspaceTable = CreateWorkspaceTable();
	wxString filesTable = CreateFilesTable();

	content.Replace(wxT("$(WorkspaceTable)"), workspaceTable);
	content.Replace(wxT("$(FilesTable)"), filesTable);
	
	//replace the HTML colours with platfroms correct colours
	wxColour active_caption 	= wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
	wxColour active_caption_txt = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	
	active_caption = DrawingUtils::LightColour(active_caption, 50);
	
	content.Replace(wxT("$(ACTIVE_CAPTION)"), active_caption.GetAsString());
	content.Replace(wxT("$(ACTIVE_CAPTION_TEXT)"), active_caption_txt.GetAsString());
	
	m_welcomePage->SetPage(content);
	GetNotebook()->AddPage(m_welcomePage, wxT("Welcome!"), wxNullBitmap, true);
}

void Frame::OnImportMSVS(wxCommandEvent &e)
{
	wxUnusedVar(e);
	const wxString ALL(wxT("MS Visual Studio Solution File (*.sln)|*.sln|")
	                   wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open MS Solution File"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		ManagerST::Get()->ImportMSVSSolution(dlg->GetPath());
	}
	dlg->Destroy();
}

void Frame::OnDebug(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Manager *mgr = ManagerST::Get();

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		//debugger is already running -> probably a continue command
		mgr->DbgStart();
	} else if (mgr->IsWorkspaceOpen()) {
		//debugger is not running, but workspace is opened -> start debug session
		mgr->DbgStart();
	}
}

void Frame::OnDebugUI(wxUpdateUIEvent &e)
{
	e.Enable(true);
}

void Frame::OnDebugStop(wxCommandEvent &e)
{
	wxUnusedVar(e);
	ManagerST::Get()->DbgStop();
}

void Frame::OnDebugStopUI(wxUpdateUIEvent &e)
{
	e.Enable(DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning());
}

void Frame::OnDebugManageBreakpointsUI(wxUpdateUIEvent &e)
{
	e.Enable(true);
}

void Frame::OnDebugCmd(wxCommandEvent &e)
{
	int cmd(wxNOT_FOUND);
	if (e.GetId() == XRCID("pause_debugger")) {
		cmd = DBG_PAUSE;
	} else if (e.GetId() == XRCID("dbg_stepin")) {
		cmd = DBG_STEPIN;
	} else if (e.GetId() == XRCID("dbg_stepout")) {
		cmd = DBG_STEPOUT;
	} else if (e.GetId() == XRCID("dbg_next")) {
		cmd = DBG_NEXT;
	}

	if (cmd != wxNOT_FOUND) {
		ManagerST::Get()->DbgDoSimpleCommand(cmd);
	}
}

void Frame::OnDebugCmdUI(wxUpdateUIEvent &e)
{
	if (e.GetId() == XRCID("pause_debugger") ||
	        e.GetId() == XRCID("dbg_stepin") ||
	        e.GetId() == XRCID("dbg_stepout") ||
	        e.GetId() == XRCID("dbg_next") ) {
		e.Enable(DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning());
	}
}

void Frame::OnDebuggerSettings(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DebuggerSettingsDlg *dlg = new DebuggerSettingsDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnIdle(wxIdleEvent &e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		dbgr->Poke();
		e.RequestMore();
	}
}

void Frame::OnNextBuildError(wxCommandEvent &event)
{
	GetOutputPane()->GetBuildTab()->ProcessEvent(event);
}

void Frame::OnNextBuildErrorUI(wxUpdateUIEvent &event)
{
	bool isShown(false);
	wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Output"));
	if (info.IsOk()) {
		isShown = info.IsShown();
	}
	event.Enable(ManagerST::Get()->IsWorkspaceOpen() && isShown);
}

void Frame::OnLinkClicked(wxHtmlLinkEvent &e)
{
	wxHtmlLinkInfo info = e.GetLinkInfo();
	wxString action = info.GetHref();

	if (action.StartsWith(wxT("action:"))) {
		action = action.AfterFirst(wxT(':'));
		wxString command = action.BeforeFirst(wxT(':'));
		if (command == wxT("open-file")) {
			//Open file
			wxString fileName = action.AfterFirst(wxT(':'));
			wxFileName fn(fileName);
			if (fn.GetExt() == wxT("workspace")) {
				//workspace file
				ManagerST::Get()->OpenWorkspace(fileName);
			} else {
				//other file
				ManagerST::Get()->OpenFile(fileName, wxEmptyString);
			}
		} else if (command == wxT("create-workspace")) {
			//Create workspace
			wxCommandEvent dummy;
			OnProjectNewWorkspace(e);

		} else if (command == wxT("import-msvs-solution")) {
			//Import microsoft visual studio solution
			wxCommandEvent dummy;
			OnImportMSVS(e);

		} else if (command == wxT("open-workspace")) {
			wxCommandEvent dummy;
			OnSwitchWorkspace(e);
		} else {
			e.Skip();
		}
	} else {
		//do the default, which is open the link in the default
		//browser
		wxLaunchDefaultBrowser(info.GetHref());
	}
}

void Frame::OnLoadLastSessionUI(wxUpdateUIEvent &event)
{
	event.Check(m_frameGeneralInfo.GetFlags() & CL_LOAD_LAST_SESSION);
}

void Frame::SetFrameFlag(bool set, int flag)
{
	if (set) {
		m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() | flag);
	} else {
		m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() & ~(flag));
	}
}

void Frame::OnLoadLastSession(wxCommandEvent &event)
{
	SetFrameFlag(event.IsChecked(), CL_LOAD_LAST_SESSION);
}

void Frame::OnShowWelcomePageUI(wxUpdateUIEvent &event)
{
	for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
		if (GetNotebook()->GetPage((size_t)i) == m_welcomePage) {
			event.Enable(false);
			return;
		}
	}
	event.Enable(true);
}

void Frame::OnShowWelcomePage(wxCommandEvent &event)
{
	//check if the welcome page is not 'on'
	for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
		if (GetNotebook()->GetPage((size_t)i) == m_welcomePage) {
			GetNotebook()->SetSelection((size_t)i);
			return;
		}
	}
	CreateWelcomePage();
}

void Frame::LoadPlugins()
{
	PluginManager::Get()->Load();

	//after the plugin are loaded, it is time to load the saved
	//perspective
	long loadIt(1);
	EditorConfigST::Get()->GetLongValue(wxT("LoadSavedPrespective"), loadIt);
	if (loadIt) {
		wxString pers = EditorConfigST::Get()->LoadPerspective(wxT("Default"));
		if ( pers.IsEmpty() == false && EditorConfigST::Get()->GetRevision() == SvnRevision) {
			m_mgr.LoadPerspective(pers);
		} else {
			EditorConfigST::Get()->SetRevision(SvnRevision);
		}
	}
	EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);
}

void Frame::OnAppActivated(wxActivateEvent &e)
{
	if (!m_theFrame || e.GetActive() == false) {
		e.Skip();
		return;
	}

	//check if the welcome page is not 'on'
	std::vector<wxFileName> files;
	for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage((size_t)i));
		if (editor) {
			//test to see if the file was modified
			time_t diskTime = GetFileModificationTime(editor->GetFileName().GetFullPath());
			time_t editTime = editor->GetEditorLastModifiedTime();
			if (diskTime > editTime) {
				//set the editor time to match the disk time
				editor->SetEditorLastModifiedTime(diskTime);

				wxString msg;
				msg << wxT("The File '");
				msg << editor->GetFileName().GetFullName();
				msg << wxT("' was modified\n");
				msg << wxT("outside of the editor, would you like to reload it?");
				if (wxMessageBox(msg, wxT("Confirm"), wxYES_NO | wxCANCEL | wxICON_QUESTION) == wxYES) {
					editor->ReloadFile();
				}

				if (editor->GetProject().IsEmpty() == false && ManagerST::Get()->IsWorkspaceOpen()) {
					//retag the file
					files.push_back(editor->GetFileName());
				}
			}
		}
	}

	if (files.empty() == false) {
		TagsManagerST::Get()->RetagFiles( files );
	}

	e.Skip();
}

void Frame::UpgradeExternalDbExt()
{
	wxString fileSpec( wxT( "*.db" ) );
	wxArrayString files;
	wxDir::GetAllFiles( ManagerST::Get()->GetStarupDirectory(), &files, fileSpec, wxDIR_FILES );
	for ( size_t i=0; i<files.GetCount(); i++ ) {
		wxFileName fn(files.Item(i));
		fn.SetExt(wxT("tags"));
		::wxRenameFile(files.Item(i), fn.GetFullPath());

		wxString logMsg;
		logMsg << wxT("Renaming: ") << files.Item(i) << wxT(" to ") << fn.GetFullPath();
		wxLogMessage(logMsg);
	}
}

void Frame::AutoLoadExternalDb()
{
	wxString fileSpec( wxT( "*.tags" ) );
	wxArrayString files;

	//check to see if an external database is loaded
	wxString tagDb = EditorConfigST::Get()->GetTagsDatabase();
	if (tagDb.IsEmpty()) {

		//fetch list of files
		wxDir::GetAllFiles( ManagerST::Get()->GetStarupDirectory(), &files, fileSpec, wxDIR_FILES );
		if (files.GetCount() == 1) {
			wxString dbname = files.Item(0);

			//Only one database was found, attach it
			EditorConfigST::Get()->SetTagsDatabase(dbname);
			ManagerST::Get()->SetExternalDatabase(wxFileName(dbname));

		} else if (files.GetCount() > 1) {

			wxString message;
			message << wxT("CodeLite detected that there is no external symbols database attached.\n");
			message << wxT("However, several databases were found, would you like to attach one now?\n");
			message << wxT("(attaching external symbols database improves CodeCompletion significantly)");

			if (wxMessageBox(message, wxT("Attach symbols database"), wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES) {
				wxString dbname = wxGetSingleChoice(wxT("Select extenal database symbols to attach:"), wxT("Select symbols database"), files);
				if (dbname.IsEmpty() == false) {
					EditorConfigST::Get()->SetTagsDatabase(dbname);
					ManagerST::Get()->SetExternalDatabase(wxFileName(dbname));
				}
			}
		} else {

			//no databases were found at all.
			//suggest user to create one
			wxString message;
			message << wxT("CodeLite detected that there is no external symbols database attached,\n");
			message << wxT("nor it can not find any. Would you like to create one?\n");
			message << wxT("(attaching external symbols database improves CodeCompletion significantly)");
			if (wxMessageBox(message, wxT("Create symbols database"), wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES) {
				DoBuildExternalDatabase();
			}
		}
	}
}

void Frame::OnCompileFile(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
		//get the current active docuemnt
		size_t curpage = GetNotebook()->GetSelection();
		if (curpage != Notebook::npos) {
			LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage((size_t)curpage));
			if (editor && editor->GetProject().IsEmpty() == false) {
				mgr->CompileFile(editor->GetProject(), editor->GetFileName().GetFullPath());
			}
		}
	}
}

void Frame::OnCompileFileUI(wxUpdateUIEvent &e)
{
	Manager *mgr = ManagerST::Get();
	if (mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
		//get the current active docuemnt
		size_t curpage = GetNotebook()->GetSelection();
		if (curpage != Notebook::npos) {
			LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage((size_t)curpage));
			if (editor && editor->GetProject().IsEmpty() == false) {
				e.Enable(true);
				return;
			}
		}
	}
	e.Enable(false);
}

void Frame::OnDebugAttach(wxCommandEvent &event)
{
	wxUnusedVar(event);
	//Start the debugger
	Manager *mgr = ManagerST::Get();
	mgr->DbgStart(1);
}

void Frame::OnCloseAllButThis(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//Start the debugger
	Manager *mgr = ManagerST::Get();

	size_t sel = GetNotebook()->GetSelection();
	if (sel != Notebook::npos) {
		mgr->CloseAllButThis(GetNotebook()->GetPage((size_t)sel));
	}
}

OpenWindowsPanel *Frame::GetOpenWindowsPane()
{
	return GetWorkspacePane()->GetOpenedWindows();
}

FileExplorer *Frame::GetFileExplorer()
{
	return GetWorkspacePane()->GetFileExplorer();
}

void Frame::OnLoadWelcomePage(wxCommandEvent &event)
{
	SetFrameFlag(event.IsChecked(), CL_SHOW_WELCOME_PAGE);
}

void Frame::OnLoadWelcomePageUI(wxUpdateUIEvent &event)
{
	event.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_WELCOME_PAGE ? true : false);
}

wxComboBox *Frame::GetConfigChoice()
{
	return GetWorkspacePane()->GetConfigCombBox();
}

void Frame::OnFileCloseUI(wxUpdateUIEvent &event)
{
	event.Enable(GetNotebook()->GetPageCount() > 0 ? true : false);
}

void Frame::OnViewEolCR(wxCommandEvent &e)
{
	size_t flags = m_frameGeneralInfo.GetFlags();
	flags &= ~(CL_USE_EOL_CR);
	flags &= ~(CL_USE_EOL_LF);
	flags &= ~(CL_USE_EOL_CRLF);
	m_frameGeneralInfo.SetFlags(flags | CL_USE_EOL_CR);
}

void Frame::OnViewEolLF(wxCommandEvent &e)
{
	size_t flags = m_frameGeneralInfo.GetFlags();
	flags &= ~(CL_USE_EOL_CR);
	flags &= ~(CL_USE_EOL_LF);
	flags &= ~(CL_USE_EOL_CRLF);
	m_frameGeneralInfo.SetFlags(flags | CL_USE_EOL_LF);
}

void Frame::OnViewEolCRLF(wxCommandEvent &e)
{
	size_t flags = m_frameGeneralInfo.GetFlags();
	flags &= ~(CL_USE_EOL_CR);
	flags &= ~(CL_USE_EOL_LF);
	flags &= ~(CL_USE_EOL_CRLF);
	m_frameGeneralInfo.SetFlags(flags | CL_USE_EOL_CRLF);
}

void Frame::OnViewEolUI(wxUpdateUIEvent &e)
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	e.Enable(editor ? true : false);
}

void Frame::OnConvertEol(wxCommandEvent &e)
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	if (editor) {
		size_t flags = m_frameGeneralInfo.GetFlags();
		int eolMode (wxSCI_EOL_LF);	//default it to unix
		if (flags & CL_USE_EOL_CR) {
			eolMode = wxSCI_EOL_CR;
		} else if (flags & CL_USE_EOL_CRLF) {
			eolMode = wxSCI_EOL_CRLF;
		}

		editor->ConvertEOLs(eolMode);
		//editor->Colourise(0, wxSCI_INVALID_POSITION);
	}
}

void Frame::OnConvertEolUI(wxUpdateUIEvent &e)
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	e.Enable(editor ? true : false);
}

void Frame::OnViewEolCRLF_UI(wxUpdateUIEvent &e)
{
	e.Check(m_frameGeneralInfo.GetFlags() & CL_USE_EOL_CRLF ? true : false);
}

void Frame::OnViewEolCR_UI(wxUpdateUIEvent &e)
{
	e.Check(m_frameGeneralInfo.GetFlags() & CL_USE_EOL_CR ? true : false);
}

void Frame::OnViewEolLF_UI(wxUpdateUIEvent &e)
{
	e.Check(m_frameGeneralInfo.GetFlags() & CL_USE_EOL_LF ? true : false);
}

void Frame::OnViewDisplayEOL(wxCommandEvent &e)
{
	bool visible;
	size_t frame_flags = m_frameGeneralInfo.GetFlags();
	if (e.IsChecked()) {
		frame_flags |= CL_SHOW_EOL;
		visible = true;
	} else {
		frame_flags &= ~CL_SHOW_EOL;
		visible = false;
	}

	m_frameGeneralInfo.SetFlags(frame_flags);
	for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage((size_t)i));
		if (editor) {
			editor->SetViewEOL(visible);
		}
	}
}

void Frame::OnViewDisplayEOL_UI(wxUpdateUIEvent &e)
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	bool hasEditor = editor ? true : false;
	if (!hasEditor) {
		e.Enable(false);
		return;
	}

	e.Enable(true);
	e.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_EOL ? true : false);
}

OutputTabWindow* Frame::FindOutputTabWindowByPtr(wxWindow *win)
{
	Notebook *book = GetOutputPane()->GetNotebook();
	for (size_t i=0; i< (size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == OutputPane::BUILD_WIN) {
			OutputTabWindow *tabWin = dynamic_cast< OutputTabWindow* > ( book->GetPage(i) );
			if (tabWin && tabWin->GetInternalWindow() == win) {
				return tabWin;
			}
		}

		if (book->GetPageText(i) == OutputPane::FIND_IN_FILES_WIN) {
			if (GetOutputPane()->GetFindResultsTab()->GetInternalWindow() == win) {
				return GetOutputPane()->GetFindResultsTab();
			}
		}
	}
	return NULL;
}

void Frame::OnCopyFilePath(wxCommandEvent &event)
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	if (editor) {
		wxString fileName = editor->GetFileName().GetFullPath();
#if wxUSE_CLIPBOARD
		if (wxTheClipboard->Open()) {
			wxTheClipboard->UsePrimarySelection(false);
			if (!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
				//wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
			}
			wxTheClipboard->Close();
		} else {
			wxPrintf(wxT("Failed to open the clipboard"));
		}
#endif
	}
}
void Frame::OnCopyFilePathOnly(wxCommandEvent &event)
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	if (editor) {
		wxString fileName = editor->GetFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
#if wxUSE_CLIPBOARD
		if (wxTheClipboard->Open()) {
			wxTheClipboard->UsePrimarySelection(false);
			if (!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
				//wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
			}
			wxTheClipboard->Close();
		} else {
			wxPrintf(wxT("Failed to open the clipboard"));
		}
#endif
	}
}

void Frame::OnWorkspaceMenuUI(wxUpdateUIEvent &e)
{
	if (ManagerST::Get()->IsWorkspaceOpen() == false) {
		e.Enable(false);
		return;
	}
	if (ManagerST::Get()->IsBuildInProgress()) {
		e.Enable(false);
		return;
	}
	e.Enable(true);
}

void Frame::OnManagePlugins(wxCommandEvent &e)
{
	PluginMgrDlg *dlg = new PluginMgrDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
		wxMessageBox(wxT("Changes will take place after restart of CodeLite"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
	}
	dlg->Destroy();
}

void Frame::DoReplaceAll()
{
	m_doingReplaceInFiles = true;
}

void Frame::ShowBuildConfigurationManager()
{
	ConfigurationManagerDlg *dlg = new ConfigurationManagerDlg(this);
	dlg->ShowModal();
	dlg->Destroy();

	//force makefile generation upon configuration change
	if (ManagerST::Get()->IsWorkspaceOpen()) {
		wxArrayString projs;
		ManagerST::Get()->GetProjectList(projs);
		for ( size_t i=0; i< projs.GetCount(); i++ ) {
			ProjectPtr proj = ManagerST::Get()->GetProject( projs.Item(i) );
			if ( proj ) {
				proj->SetModified(true);
			}
		}
	}
}

void Frame::AddCppMenu()
{
	size_t selection = GetNotebook()->GetSelection();
	if( selection == Notebook::npos ) {
		return;
	}
	
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(selection));
	if ( !editor ) {
		return;
	}
	
	if(editor->GetContext()->GetName() != wxT("C++")){
		return;
	}

	//load the C++ menu and append one to the menu bar
	if (GetMenuBar()->FindMenu(wxT("C++")) == wxNOT_FOUND) {
		wxMenu *menu = wxXmlResource::Get()->LoadMenu(wxT("editor_right_click"));
		GetMenuBar()->Append(menu, wxT("&C++"));
	}
}

void Frame::RemoveCppMenu()
{
	int idx = GetMenuBar()->FindMenu(wxT("C++"));
	if ( idx != wxNOT_FOUND ) {
		wxMenu *menu = GetMenuBar()->Remove(idx);
		delete menu;
	}
}

void Frame::OnCppContextMenu(wxCommandEvent &e)
{
	wxUnusedVar(e);
	
	size_t selection = GetNotebook()->GetSelection();
	if( selection == Notebook::npos ) {
		return;
	}
	
	//get the active editor
	LEditor* editor = dynamic_cast<LEditor*>(GetNotebook()->GetPage(selection));
	if ( !editor ) {
		return;
	}

	editor->GetContext()->ProcessEvent( e );
}

void Frame::OnConfigureAccelerators(wxCommandEvent &e)
{
	AccelTableDlg *dlg = new AccelTableDlg(this);
//	wxString ll;
//	ll << XRCID("add_include_file");
//	wxMessageBox( ll );
	
	dlg->ShowModal();
	dlg->Destroy();
}
