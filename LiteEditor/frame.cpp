//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : frame.cpp
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
#include "bitmap_loader.h"
#include <wx/wupdlock.h>
#include "cl_aui_tb_are.h"
#include "open_resource_dialog.h" // New open resource
#include <wx/busyinfo.h>
#include "tags_parser_search_path_dlg.h"
#include "includepathlocator.h"
#include "localstable.h"
#include "console_frame.h"
#include "outputviewcontrolbar.h"
#include "clauidockart.h"

#include <set>
#include <list>
#include <wx/clipbrd.h>
#include <wx/numdlg.h>
#include <wx/stdpaths.h>
#include <wx/xrc/xmlres.h>
#include <wx/splitter.h>
#include <wx/aboutdlg.h>
#include <wx/aui/framemanager.h>
#include <wx/ffile.h>
#include <wx/dir.h>

#ifdef __WXGTK20__
#include <gtk-2.0/gtk/gtk.h>
#endif

#include "imanager.h"
#include "newversiondlg.h"
#include "quickdebugdlg.h"
#include "syntaxhighlightdlg.h"
#include "dirsaver.h"
#include "batchbuilddlg.h"
#include "detachedpanesinfo.h"
#include "dockablepanemenumanager.h"
#include "dockablepane.h"
#include "webupdatethread.h"
#include "aboutdlg.h"
#include "buildtabsettingsdata.h"
#include "singleinstancethreadjob.h"
#include "refactorindexbuildjob.h"
#include "customstatusbar.h"
#include "jobqueue.h"
#include "threebuttondlg.h"
#include "acceltabledlg.h"
#include "drawingutils.h"
#include "fileexplorertree.h"
#include "newprojectdlg.h"
#include "newworkspacedlg.h"
#include "replaceinfilespanel.h"
#include "pluginmgrdlg.h"
#include "environmentconfig.h"
#include "findresultstab.h"
#include "buidltab.h"
#include "errorstab.h"
#include "frame.h"
#include "symbol_tree.h"
#include "cpp_symbol_tree.h"
#include "plugin.h"
#include "language.h"
#include "editor_config.h"
#include "manager.h"
#include "menumanager.h"
#include "search_thread.h"
#include "project.h"
#include "fileview.h"
#include "configuration_manager_dlg.h"
#include "filedroptarget.h"
#include "advanced_settings.h"
#include "build_settings_config.h"
#include "macros.h"
#include "async_executable_cmd.h"
#include "workspace_pane.h"
#include "navigationmanager.h"
#include "debuggermanager.h"
#include "breakpointdlg.h"
#include "generalinfo.h"
#include "debuggersettingsdlg.h"
#include "quickoutlinedlg.h"
#include "debuggerpane.h"
#include "sessionmanager.h"
#include "pluginmanager.h"
#include "exelocator.h"
#include "builder.h"
#include "buildmanager.h"
#include "debuggerconfigtool.h"
#include "openwindowspanel.h"
#include "workspace_pane.h"
#include "globals.h"
#include "workspacetab.h"
#include "fileexplorer.h"
#include "notebook_ex.h"
#include "options_dlg2.h"
#include <wx/msgdlg.h>
#include "tabgroupdlg.h"
#include "tabgroupmanager.h"
#include "tabgroupspane.h"
#include "clang_code_completion.h"
#include "cl_defs.h"

// from auto-generated file svninfo.cpp:
extern wxString CODELITE_VERSION_STR;
extern const wxChar *SvnRevision;

static wxStopWatch gStopWatch;

// from iconsextra.cpp:
extern char *cubes_xpm[];
extern unsigned char cubes_alpha[];

static int FrameTimerId = wxNewId();

const wxEventType wxEVT_UPDATE_STATUS_BAR = XRCID("update_status_bar");
const wxEventType wxEVT_LOAD_PERSPECTIVE  = XRCID("load_perspective");

#define CHECK_SHUTDOWN() {\
		if(ManagerST::Get()->IsShutdownInProgress()){\
			return;\
		}\
	}
#ifdef __WXGTK__
#    define FACTOR_1 0.0
#    define FACTOR_2 0.0
#else
#    define FACTOR_1 2.0
#    define FACTOR_2 2.0
#endif

//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(clMainFrame, wxFrame)
	// ATTN: Any item that appears in more than one menu is
	//       only listed once here, the first time it occurs

	//---------------------------------------------------
	// System events
	//---------------------------------------------------
	//EVT_IDLE(Frame::OnIdle)
	EVT_ACTIVATE(clMainFrame::OnAppActivated)
	EVT_CLOSE(clMainFrame::OnClose)
	EVT_TIMER(FrameTimerId, clMainFrame::OnTimer)
	//	EVT_AUI_RENDER(Frame::OnAuiManagerRender)
	//	EVT_AUI_PANE_CLOSE(Frame::OnDockablePaneClosed)

	//---------------------------------------------------
	// File menu
	//---------------------------------------------------
	EVT_MENU(XRCID("new_file"),                 clMainFrame::OnFileNew)
	EVT_MENU(XRCID("open_file"),                clMainFrame::OnFileOpen)
	EVT_MENU(XRCID("refresh_file"),             clMainFrame::OnFileReload)
	EVT_MENU(XRCID("load_tab_group"),           clMainFrame::OnFileLoadTabGroup)
	EVT_MENU(XRCID("save_file"),                clMainFrame::OnSave)
	EVT_MENU(XRCID("save_file_as"),             clMainFrame::OnSaveAs)
	EVT_MENU(XRCID("save_all"),                 clMainFrame::OnFileSaveAll)
	EVT_MENU(XRCID("save_tab_group"),           clMainFrame::OnFileSaveTabGroup)
	EVT_MENU(XRCID("close_file"),               clMainFrame::OnFileClose)
	EVT_MENU(wxID_CLOSE_ALL,                    clMainFrame::OnFileCloseAll)
	EVT_MENU_RANGE(RecentFilesSubMenuID, RecentFilesSubMenuID + 10, clMainFrame::OnRecentFile)
	EVT_MENU_RANGE(RecentWorkspaceSubMenuID, RecentWorkspaceSubMenuID + 10, clMainFrame::OnRecentWorkspace)
	EVT_MENU(XRCID("load_last_session"),        clMainFrame::OnLoadLastSession)
	EVT_MENU(XRCID("exit_app"),                 clMainFrame::OnQuit)

	EVT_UPDATE_UI(XRCID("refresh_file"),        clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_file"),           clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_file_as"),        clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_all"),            clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_tab_group"),      clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("close_file"),          clMainFrame::OnFileCloseUI)
	EVT_UPDATE_UI(XRCID("load_last_session"),   clMainFrame::OnLoadLastSessionUI)

	//--------------------------------------------------
	// Edit menu
	//--------------------------------------------------
	EVT_MENU(wxID_UNDO,                         clMainFrame::DispatchCommandEvent)
	EVT_MENU(wxID_REDO,                         clMainFrame::DispatchCommandEvent)
	EVT_MENU(wxID_DUPLICATE,                    clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_line"),              clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_line_end"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_line_start"),        clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("transpose_lines"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("to_upper"),                 clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("to_lower"),                 clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("match_brace"),              clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("select_to_brace"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("complete_word"),            clMainFrame::OnCompleteWord)
	EVT_MENU(XRCID("function_call_tip"),        clMainFrame::OnFunctionCalltip)
	EVT_MENU(XRCID("convert_eol_win"),          clMainFrame::OnConvertEol)
	EVT_MENU(XRCID("convert_eol_unix"),         clMainFrame::OnConvertEol)
	EVT_MENU(XRCID("convert_eol_mac"),          clMainFrame::OnConvertEol)
	EVT_MENU(XRCID("move_line_down"),           clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("move_line_up"),             clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("center_line"),              clMainFrame::DispatchCommandEvent)

	EVT_UPDATE_UI(wxID_UNDO,                    clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_REDO,                    clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_DUPLICATE,               clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("delete_line"),         clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("delete_line_end"),     clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("delete_line_start"),   clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("transpose_lines"),     clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("to_upper"),            clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("to_lower"),            clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("match_brace"),         clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("select_to_brace"),     clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("complete_word"),       clMainFrame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("function_call_tip"),   clMainFrame::OnFunctionCalltipUI)
	EVT_UPDATE_UI(XRCID("convert_eol_win"),     clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("convert_eol_unix"),    clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("convert_eol_mac"),     clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("move_line_down"),      clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("move_line_up"),        clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("center_line"),         clMainFrame::OnFileExistUpdateUI)

	//-------------------------------------------------------
	// View menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("restore_layout"),           clMainFrame::OnRestoreDefaultLayout)
	EVT_MENU(XRCID("word_wrap"),                clMainFrame::OnViewWordWrap)
	EVT_MENU(XRCID("toggle_fold"),              clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("fold_all"),                 clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("display_eol"),              clMainFrame::OnViewDisplayEOL)
	EVT_MENU(XRCID("whitepsace_invisible"),     clMainFrame::OnShowWhitespace)
	EVT_MENU(XRCID("whitepsace_always"),        clMainFrame::OnShowWhitespace)
	EVT_MENU(XRCID("whitespace_visiable_after_indent"), clMainFrame::OnShowWhitespace)
	EVT_MENU(XRCID("whitespace_indent_only"),   clMainFrame::OnShowWhitespace)
	EVT_MENU(XRCID("full_screen"),              clMainFrame::OnShowFullScreen)
	EVT_MENU(XRCID("view_welcome_page"),        clMainFrame::OnShowWelcomePage)
	EVT_MENU(XRCID("view_welcome_page_at_startup"), clMainFrame::OnLoadWelcomePage)
	EVT_MENU(XRCID("show_nav_toolbar"),         clMainFrame::OnShowNavBar)
	EVT_MENU(XRCID("toggle_panes"),             clMainFrame::OnTogglePanes)
	EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, clMainFrame::DispatchCommandEvent)

	EVT_UPDATE_UI(XRCID("word_wrap"),           clMainFrame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("toggle_fold"),         clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("fold_all"),            clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("display_eol"),         clMainFrame::OnViewDisplayEOL_UI)
	EVT_UPDATE_UI(XRCID("whitepsace_invisible"),    clMainFrame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("whitepsace_always"),   clMainFrame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("whitespace_visiable_after_indent"),    clMainFrame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("whitespace_indent_only"), clMainFrame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("view_welcome_page"),   clMainFrame::OnShowWelcomePageUI)
	EVT_UPDATE_UI(XRCID("view_welcome_page_at_startup"),    clMainFrame::OnLoadWelcomePageUI)
	EVT_UPDATE_UI(XRCID("show_nav_toolbar"),    clMainFrame::OnShowNavBarUI)
	EVT_UPDATE_UI(viewAsSubMenuID,              clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, clMainFrame::DispatchUpdateUIEvent)

	//-------------------------------------------------------
	// Search menu
	//-------------------------------------------------------
	EVT_MENU(wxID_FIND,                         clMainFrame::DispatchCommandEvent)
	EVT_MENU(wxID_REPLACE,                      clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_function"),            clMainFrame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_macro"),               clMainFrame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_typedef"),             clMainFrame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_resource"),            clMainFrame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_type"),                clMainFrame::OnFindResourceXXX   )
	EVT_MENU(XRCID("incremental_search"),       clMainFrame::OnIncrementalSearch )
	EVT_MENU(XRCID("find_symbol"),              clMainFrame::OnQuickOutline      )
	EVT_MENU(XRCID("goto_definition"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_previous_definition"), clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_linenumber"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_bookmark"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_bookmark"),            clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("previous_bookmark"),        clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("removeall_bookmarks"),      clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_fif_match"),           clMainFrame::OnNextFiFMatch      )
	EVT_MENU(XRCID("previous_fif_match"),       clMainFrame::OnPreviousFiFMatch  )

	EVT_UPDATE_UI(wxID_FIND,                        clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(wxID_REPLACE,                     clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_next"),               clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_previous"),           clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_next_at_caret"),      clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_previous_at_caret"),  clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("incremental_search"),      clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_resource"),           clMainFrame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_type"),               clMainFrame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_function"),           clMainFrame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_macro"),              clMainFrame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_typedef"),            clMainFrame::OnWorkspaceOpen       )

	EVT_UPDATE_UI(XRCID("find_symbol"),             clMainFrame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("goto_definition"),         clMainFrame::DispatchUpdateUIEvent )
	EVT_UPDATE_UI(XRCID("goto_previous_definition"),clMainFrame::DispatchUpdateUIEvent )
	EVT_UPDATE_UI(XRCID("goto_linenumber"),         clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("toggle_bookmark"),         clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("next_bookmark"),           clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("previous_bookmark"),       clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("removeall_bookmarks"),     clMainFrame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("next_fif_match"),          clMainFrame::OnNextFiFMatchUI      )
	EVT_UPDATE_UI(XRCID("previous_fif_match"),      clMainFrame::OnPreviousFiFMatchUI  )

	//-------------------------------------------------------
	// Project menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("local_workspace_prefs"),    clMainFrame::OnWorkspaceEditorPreferences)
	EVT_MENU(XRCID("local_workspace_settings"), clMainFrame::OnWorkspaceSettings)
	EVT_MENU(XRCID("new_workspace"),            clMainFrame::OnProjectNewWorkspace)
	EVT_MENU(XRCID("switch_to_workspace"),      clMainFrame::OnSwitchWorkspace)
	EVT_MENU(XRCID("close_workspace"),          clMainFrame::OnCloseWorkspace)
	EVT_MENU(XRCID("reload_workspace"),         clMainFrame::OnReloadWorkspace)
	EVT_MENU(XRCID("import_from_msvs"),         clMainFrame::OnImportMSVS)
	EVT_MENU(XRCID("new_project"),              clMainFrame::OnProjectNewProject)
	EVT_MENU(XRCID("add_project"),              clMainFrame::OnProjectAddProject)
	EVT_MENU(XRCID("retag_workspace"),          clMainFrame::OnRetagWorkspace)
	EVT_MENU(XRCID("full_retag_workspace"),     clMainFrame::OnRetagWorkspace)
	EVT_MENU(XRCID("project_properties"),       clMainFrame::OnShowActiveProjectSettings)

	EVT_UPDATE_UI(XRCID("local_workspace_prefs"),    clMainFrame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("local_workspace_settings"), clMainFrame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("close_workspace"),          clMainFrame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("reload_workspace"),         clMainFrame::OnReloadWorkspaceUI)
	EVT_UPDATE_UI(XRCID("add_project"),              clMainFrame::OnWorkspaceMenuUI)
	EVT_UPDATE_UI(XRCID("retag_workspace"),          clMainFrame::OnRetagWorkspaceUI)
	EVT_UPDATE_UI(XRCID("full_retag_workspace"),     clMainFrame::OnRetagWorkspaceUI)
	EVT_UPDATE_UI(XRCID("project_properties"),       clMainFrame::OnShowActiveProjectSettingsUI)

	//-------------------------------------------------------
	// Build menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("execute_no_debug"),         clMainFrame::OnExecuteNoDebug)
	EVT_MENU(XRCID("stop_executed_program"),    clMainFrame::OnStopExecutedProgram)
	EVT_MENU(XRCID("build_active_project"),     clMainFrame::OnBuildProject)
	EVT_MENU(XRCID("compile_active_file"),      clMainFrame::OnCompileFile)
	EVT_MENU(XRCID("clean_active_project"),     clMainFrame::OnCleanProject)
	EVT_MENU(XRCID("stop_active_project_build"),    clMainFrame::OnStopBuild)
	EVT_MENU(XRCID("rebuild_active_project"),   clMainFrame::OnRebuildProject)
	EVT_MENU(XRCID("build_n_run_active_project"),   clMainFrame::OnBuildAndRunProject)
	EVT_MENU(XRCID("build_workspace"),          clMainFrame::OnBuildWorkspace)
	EVT_MENU(XRCID("clean_workspace"),          clMainFrame::OnCleanWorkspace)
	EVT_MENU(XRCID("rebuild_workspace"),        clMainFrame::OnReBuildWorkspace)
	EVT_MENU(XRCID("batch_build"),              clMainFrame::OnBatchBuild)

	EVT_UPDATE_UI(XRCID("execute_no_debug"),        clMainFrame::OnExecuteNoDebugUI)
	EVT_UPDATE_UI(XRCID("stop_executed_program"),   clMainFrame::OnStopExecutedProgramUI)
	EVT_UPDATE_UI(XRCID("build_active_project"),    clMainFrame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("compile_active_file"),     clMainFrame::OnCompileFileUI)
	EVT_UPDATE_UI(XRCID("clean_active_project"),    clMainFrame::OnCleanProjectUI)
	EVT_UPDATE_UI(XRCID("stop_active_project_build"),   clMainFrame::OnStopBuildUI)
	EVT_UPDATE_UI(XRCID("rebuild_active_project"),  clMainFrame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("build_n_run_active_project"),  clMainFrame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("build_workspace"),         clMainFrame::OnBuildWorkspaceUI)
	EVT_UPDATE_UI(XRCID("clean_workspace"),         clMainFrame::OnCleanWorkspaceUI)
	EVT_UPDATE_UI(XRCID("rebuild_workspace"),       clMainFrame::OnReBuildWorkspaceUI)
	EVT_UPDATE_UI(XRCID("batch_build"),             clMainFrame::OnBatchBuildUI)

	//-------------------------------------------------------
	// Debug menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("start_debugger"),           clMainFrame::OnDebug)
	EVT_MENU(XRCID("restart_debugger"),         clMainFrame::OnDebugRestart)
	EVT_MENU(XRCID("attach_debugger"),          clMainFrame::OnDebugAttach)
	EVT_MENU(XRCID("pause_debugger"),           clMainFrame::OnDebugCmd)
	EVT_MENU(XRCID("stop_debugger"),            clMainFrame::OnDebugStop)
	EVT_MENU(XRCID("dbg_stepin"),               clMainFrame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_stepout"),              clMainFrame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_next"),                 clMainFrame::OnDebugCmd)
	EVT_MENU(XRCID("show_cursor"),              clMainFrame::OnDebugCmd)
	EVT_MENU(XRCID("add_breakpoint"),						clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("insert_breakpoint"),        clMainFrame::DispatchCommandEvent) // Toggles
	EVT_MENU(XRCID("insert_temp_breakpoint"),   clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("insert_cond_breakpoint"),   clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("edit_breakpoint"),          clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("show_breakpoint_dlg"),      clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("insert_watchpoint"),        clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_breakpoint_enabled_status"),  clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("ignore_breakpoint"),        clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_breakpoint"),        clMainFrame::DispatchCommandEvent)
	EVT_MENU(XRCID("quick_debug"),              clMainFrame::OnQuickDebug)

	EVT_UPDATE_UI(XRCID("start_debugger"),      clMainFrame::OnDebugUI)
	EVT_UPDATE_UI(XRCID("restart_debugger"),    clMainFrame::OnDebugRestartUI)
	EVT_UPDATE_UI(XRCID("pause_debugger"),      clMainFrame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("stop_debugger"),       clMainFrame::OnDebugStopUI)
	EVT_UPDATE_UI(XRCID("dbg_stepin"),          clMainFrame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("dbg_stepout"),         clMainFrame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("dbg_next"),            clMainFrame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("show_cursor"),         clMainFrame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("insert_breakpoint"),   clMainFrame::OnDebugManageBreakpointsUI)
	EVT_UPDATE_UI(XRCID("quick_debug"),         clMainFrame::OnQuickDebugUI)

	//-------------------------------------------------------
	// Plugins menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("manage_plugins"),           clMainFrame::OnManagePlugins)

	//-------------------------------------------------------
	// Settings menu
	//-------------------------------------------------------
	EVT_MENU(wxID_PREFERENCES,                  clMainFrame::OnViewOptions)
	EVT_MENU(XRCID("syntax_highlight"),         clMainFrame::OnSyntaxHighlight)
	EVT_MENU(XRCID("configure_accelerators"),   clMainFrame::OnConfigureAccelerators)
	EVT_MENU(XRCID("add_envvar"),               clMainFrame::OnAddEnvironmentVariable)
	EVT_MENU(XRCID("advance_settings"),         clMainFrame::OnAdvanceSettings)
	EVT_MENU(XRCID("debuger_settings"),         clMainFrame::OnDebuggerSettings)
	EVT_MENU(XRCID("tags_options"),             clMainFrame::OnCtagsOptions)

	//-------------------------------------------------------
	// Help menu
	//-------------------------------------------------------
	EVT_MENU(wxID_ABOUT,                        clMainFrame::OnAbout)
	EVT_MENU(XRCID("check_for_update"),         clMainFrame::OnCheckForUpdate)

	//-----------------------------------------------------------------
	// Toolbar
	//-----------------------------------------------------------------
	EVT_MENU(wxID_FORWARD,                      clMainFrame::OnBackwardForward)
	EVT_MENU(wxID_BACKWARD,                     clMainFrame::OnBackwardForward)
	EVT_MENU(XRCID("highlight_word"),           clMainFrame::OnHighlightWord)

	EVT_UPDATE_UI(wxID_FORWARD,                 clMainFrame::OnBackwardForwardUI)
	EVT_UPDATE_UI(wxID_BACKWARD,                clMainFrame::OnBackwardForwardUI)

	//-------------------------------------------------------
	// Workspace Pane tab context menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("detach_wv_tab"),            clMainFrame::OnDetachWorkspaceViewTab)

	//-------------------------------------------------------
	// Debugger Pane tab context menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("detach_debugger_tab"),      clMainFrame::OnDetachDebuggerViewTab)

	//-------------------------------------------------------
	// Editor tab context menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("close_other_tabs"),                 clMainFrame::OnCloseAllButThis)
	EVT_MENU(XRCID("copy_file_name"),                   clMainFrame::OnCopyFilePath)
	EVT_MENU(XRCID("copy_file_path"),                   clMainFrame::OnCopyFilePathOnly)
	EVT_MENU(XRCID("copy_file_name_only"),              clMainFrame::OnCopyFileName)
	EVT_MENU(XRCID("open_shell_from_filepath"),         clMainFrame::OnOpenShellFromFilePath)

	EVT_UPDATE_UI(XRCID("copy_file_name"),              clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("copy_file_path"),              clMainFrame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("open_shell_from_filepath"),    clMainFrame::OnFileExistUpdateUI)

	//-----------------------------------------------------------------
	// Default editor context menu
	//-----------------------------------------------------------------
	EVT_MENU(wxID_DELETE,                       clMainFrame::DispatchCommandEvent)
	EVT_UPDATE_UI(wxID_DELETE,                  clMainFrame::DispatchUpdateUIEvent)

	//-----------------------------------------------------------------
	// C++ editor context menu
	//-----------------------------------------------------------------
	EVT_MENU(XRCID("add_include_file"),         clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("swap_files"),               clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("find_decl"),                clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("find_impl"),                clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("go_to_function_start"),     clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("go_to_next_function"),      clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("insert_doxy_comment"),      clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("setters_getters"),          clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("move_impl"),                clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("add_impl"),                 clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("add_multi_impl"),           clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("add_virtual_impl"),         clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("add_pure_virtual_impl"),    clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("rename_symbol"),            clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("find_references"),               clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("comment_selection"),        clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("comment_line"),             clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("retag_file"),               clMainFrame::OnCppContextMenu)
	EVT_MENU(XRCID("rename_local_variable"),    clMainFrame::OnCppContextMenu)

	//-----------------------------------------------------------------
	// Hyperlinks
	//-----------------------------------------------------------------
	EVT_HTML_LINK_CLICKED(wxID_ANY,             clMainFrame::OnLinkClicked)
	EVT_MENU(XRCID("link_action"),              clMainFrame::OnStartPageEvent)

	//-----------------------------------------------------------------
	// CodeLite-specific events
	//-----------------------------------------------------------------
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS, clMainFrame::OnParsingThreadDone   )
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_MESSAGE             , clMainFrame::OnParsingThreadMessage)
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE,     clMainFrame::OnClearTagsCache)
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_COMPLETED,  clMainFrame::OnRetaggingCompelted)
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS,   clMainFrame::OnRetaggingProgress)

	EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_STATUS_BAR,                 clMainFrame::OnSetStatusMessage)
	EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE,                   clMainFrame::OnDatabaseUpgrade )
	EVT_COMMAND(wxID_ANY, wxEVT_SHELL_COMMAND_PROCESS_ENDED,       clMainFrame::OnBuildEnded)
	EVT_MENU   (XRCID("update_num_builders_count"),                clMainFrame::OnUpdateNumberOfBuildProcesses)
	EVT_MENU   (XRCID("goto_codelite_download_url"),               clMainFrame::OnGotoCodeLiteDownloadPage)

	EVT_MENU   (XRCID("never_update_parser_paths"),                clMainFrame::OnNeverUpdateParserPath)
	EVT_MENU   (XRCID("update_parser_paths"),                      clMainFrame::OnUpdateParserPath)

	EVT_SYMBOLTREE_ADD_ITEM(wxID_ANY,    clMainFrame::OnAddSymbols)
	EVT_SYMBOLTREE_DELETE_ITEM(wxID_ANY, clMainFrame::OnDeleteSymbols)
	EVT_SYMBOLTREE_UPDATE_ITEM(wxID_ANY, clMainFrame::OnUpdateSymbols)

	EVT_COMMAND(wxID_ANY, wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, clMainFrame::OnSingleInstanceOpenFiles)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP,  clMainFrame::OnSingleInstanceRaise)

	EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_VERSION_AVAILABLE, clMainFrame::OnNewVersionAvailable)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_UPTODATE,      clMainFrame::OnNewVersionAvailable)

	EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_DOCKPANE,    clMainFrame::OnNewDetachedPane)
	EVT_COMMAND(wxID_ANY, wxEVT_LOAD_PERSPECTIVE,    clMainFrame::OnLoadPerspective)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_DELETE_DOCKPANE, clMainFrame::OnDestroyDetachedPane)

	EVT_MENU(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED,          clMainFrame::OnReloadExternallModified)
	EVT_MENU(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, clMainFrame::OnReloadExternallModifiedNoPrompt)
END_EVENT_TABLE()


clMainFrame* clMainFrame::m_theFrame = NULL;

clMainFrame::clMainFrame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
		: wxFrame(pParent, id, title, pos, size, style)
		, m_buildAndRun             (false)
		, m_cppMenu                 (NULL)
		, m_highlightWord           (false)
		, m_workspaceRetagIsRequired(false)
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

	long value(0);
	EditorConfigST::Get()->GetLongValue(wxT("highlight_word"), value);
	m_highlightWord = (bool)value;

	CreateGUIControls();

	ManagerST::Get();	// Dummy call

	//allow the main frame to receive files by drag and drop
	SetDropTarget( new FileDropTarget() );

	// Start the search thread
	SearchThreadST::Get()->SetNotifyWindow(this);
	SearchThreadST::Get()->Start(WXTHREAD_MIN_PRIORITY);

	// start the job queue
	JobQueueSingleton::Instance()->Start(6);

	// the single instance job is a presisstent job, so the pool will contain only 4 available threads
	JobQueueSingleton::Instance()->PushJob(new SingleInstanceThreadJob(this, ManagerST::Get()->GetStarupDirectory()));

	//start the editor creator thread
	m_timer = new wxTimer(this, FrameTimerId);

	// connect common edit events
	wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Connect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);

	wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Connect(wxID_CUT,       wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
}

clMainFrame::~clMainFrame(void)
{
	wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Disconnect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Disconnect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
	wxTheApp->Disconnect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Disconnect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Disconnect(wxID_CUT,       wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);

	delete m_timer;
	ManagerST::Free();
	delete m_DPmenuMgr;

	// uninitialize AUI manager
	m_mgr.UnInit();
}

void clMainFrame::Initialize(bool loadLastSession)
{
	//set the revision number in the frame title
	wxString title(wxT("CodeLite - Revision: "));
	title << SvnRevision;

	//initialize the environment variable configuration manager
	EnvironmentConfig::Instance()->Load();

	EditorConfig *cfg = EditorConfigST::Get();
	GeneralInfo inf;
	cfg->ReadObject(wxT("GeneralInfo"), &inf);

	int screenW = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int screenH = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

	// validate the frame loaded pos & size
	if (inf.GetFramePosition().x < 0 || inf.GetFramePosition().x > screenW) {
		inf.SetFramePosition(wxPoint(30, 3));
	}

	if (inf.GetFramePosition().y < 0 || inf.GetFramePosition().y > screenH) {
		inf.SetFramePosition(wxPoint(30, 3));
	}

	wxSize frameSize ( inf.GetFrameSize() );
	if ( inf.GetFrameSize().x < 400 || inf.GetFrameSize().x > screenW ) {
		frameSize.SetWidth( 400 );
	}
	if ( inf.GetFrameSize().y < 400 || inf.GetFrameSize().y > screenH ) {
		frameSize.SetHeight( 400 );
	}

	inf.SetFrameSize( frameSize );

	m_theFrame = new clMainFrame( NULL,
	                        wxID_ANY,
	                        title,
	                        inf.GetFramePosition(),
	                        inf.GetFrameSize(),
	                        wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

	m_theFrame->m_frameGeneralInfo = inf;

	m_theFrame->Maximize(m_theFrame->m_frameGeneralInfo.GetFlags() & CL_MAXIMIZE_FRAME ? true : false);

	//add the welcome page
	if (m_theFrame->m_frameGeneralInfo.GetFlags() & CL_SHOW_WELCOME_PAGE) {
		m_theFrame->CreateWelcomePage();
	}

	//plugins must be loaded before the file explorer
	m_theFrame->CompleteInitialization();

	//time to create the file explorer
	wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("go_home"));
	m_theFrame->GetFileExplorer()->GetEventHandler()->ProcessEvent(e);

	m_theFrame->SendSizeEvent();
	m_theFrame->StartTimer();

	// Keep the current layout before loading the perspective from the disk
	m_theFrame->m_defaultLayout = m_theFrame->m_mgr.SavePerspective();

	// After all the plugins / panes have been loaded,
	// its time to re-load the perspective
	wxCommandEvent evt(wxEVT_LOAD_PERSPECTIVE);
	m_theFrame->AddPendingEvent( evt );
}

clMainFrame* clMainFrame::Get()
{
	return m_theFrame;
}

void clMainFrame::CreateGUIControls(void)
{
	this->Freeze();

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
	m_mainPanel = new wxPanel(this);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	mainSizer->Add(m_mainPanel, 1, wxEXPAND);

	// tell wxAuiManager to manage this frame
	m_mgr.SetManagedWindow(m_mainPanel);
	m_mgr.SetArtProvider(new CLAuiDockArt());
	SetAUIManagerFlags();

	wxColour frameColor = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
	wxColour frameColor2;
	if(DrawingUtils::IsDark(frameColor)) {
		m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxT("WHITE"));
	} else {
		m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	}

#ifdef __WXMSW__
	frameColor2 = wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION);
#else
	frameColor2 = DrawingUtils::LightColour(frameColor, 2.0);
#endif

	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR,     wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT));
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR,   frameColor);
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR,            frameColor2);
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR,                      DrawingUtils::GetPanelBgColour());
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR,                DrawingUtils::GetPanelBgColour());

	//initialize debugger configuration tool
	DebuggerConfigTool::Get()->Load(wxT("config/debuggers.xml"), wxT("2.8.0"));
	WorkspaceST::Get()->SetStartupDir(ManagerST::Get()->GetStarupDirectory());

#if defined(__WXMAC__)
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
#else
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
#endif

	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);

	// Load the menubar from XRC and set this frame's menubar to it.
	SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));

	// Set up dynamic parts of menu.
	CreateViewAsSubMenu();
	CreateRecentlyOpenedWorkspacesMenu();
	m_DPmenuMgr = new DockablePaneMenuManager(GetMenuBar(), &m_mgr);

	//---------------------------------------------
	// Add docking windows
	//---------------------------------------------

	m_outputPane = new OutputPane(m_mainPanel, wxT("Output View"));
	wxAuiPaneInfo paneInfo;
	m_mgr.AddPane(m_outputPane,
	              paneInfo.Name(wxT("Output View")).Caption(wxT("Output View")).Bottom().Layer(0).Position(0).CaptionVisible(false).MinSize(200, 200));
	RegisterDockWindow(XRCID("output_pane"), wxT("Output View"));

	// Add the explorer pane
	m_workspacePane = new WorkspacePane(m_mainPanel, wxT("Workspace View"), &m_mgr);
	m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo().
	              Name(m_workspacePane->GetCaption()).Caption(m_workspacePane->GetCaption()).
	              Left().BestSize(250, 300).Layer(1).Position(0).CloseButton(true));
	RegisterDockWindow(XRCID("workspace_pane"), wxT("Workspace View"));

	//add the debugger locals tree, make it hidden by default
	m_debuggerPane = new DebuggerPane(m_mainPanel, wxT("Debugger"), &m_mgr);
	m_mgr.AddPane(m_debuggerPane,
	              wxAuiPaneInfo().Name(m_debuggerPane->GetCaption()).Caption(m_debuggerPane->GetCaption()).Bottom().Layer(1).Position(1).CloseButton(true).Hide());
	RegisterDockWindow(XRCID("debugger_pane"), wxT("Debugger"));

	m_mainBook = new MainBook(m_mainPanel);
	m_mgr.AddPane(m_mainBook, wxAuiPaneInfo().Name(wxT("Editor")).CenterPane().PaneBorder(false));
	CreateRecentlyOpenedFilesMenu();

	long show_nav(1);
	EditorConfigST::Get()->GetLongValue(wxT("ShowNavBar"), show_nav);
	if ( !show_nav ) {
		m_mainBook->ShowNavBar( false );
	}

	BuildSettingsConfigST::Get()->Load(wxT("2.0.6"));

	//load dialog properties
	EditorConfigST::Get()->ReadObject(wxT("FindAndReplaceData"), &LEditor::GetFindReplaceData());
	EditorConfigST::Get()->ReadObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);

	TagsManager *tagsManager = TagsManagerST::Get();

	//start ctags process
#if defined (__WXMAC__)
	// On Mac OSX, search the codelite_indexer in the correct path
	tagsManager->SetCodeLiteIndexerPath(wxStandardPaths::Get().GetDataDir());
#else
	// set the path to codelite_indexer
	wxFileName exePath( wxStandardPaths::Get().GetExecutablePath() );
	tagsManager->SetCodeLiteIndexerPath(exePath.GetPath());
	ManagerST::Get()->SetCodeLiteLauncherPath(exePath.GetPath());
#endif
	tagsManager->StartCodeLiteIndexer();

	//--------------------------------------------------------------------------------------
	// Start the parsing thread, the parsing thread and the SymbolTree (or its derived)
	// Are connected. The constructor of SymbolTree, calls ParseThreadST::Get()->SetNotifyWindow(this)
	// to allows it to receive events for gui changes.
	//
	// If you wish to connect another object for it, simply call ParseThreadST::Get()->SetNotifyWindow(this)
	// with another object as 'this'
	//--------------------------------------------------------------------------------------

	// Update the parser thread search paths
	ParseThreadST::Get()->SetCrawlerEnabeld(m_tagsOptionsData.GetParserEnabled());
	ParseThreadST::Get()->SetSearchPaths( m_tagsOptionsData.GetParserSearchPaths(), m_tagsOptionsData.GetParserExcludePaths() );

	ParseThreadST::Get()->Start();

	// Connect this tree to the parse thread
	ParseThreadST::Get()->SetNotifyWindow( this );

	// And finally create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	SetStatusBar(statusBar);
	m_status.resize(3);
	GetStatusBar()->SetFieldsCount(m_status.size());
	SetStatusMessage(wxT("Ready"), 0);
	SetStatusMessage(wxT("Done"), m_status.size()-1);

	// update ctags options
	TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);

	// set this frame as the event handler for any events
	TagsManagerST::Get()->SetEvtHandler( this );

	// We must do Layout() before loading the toolbars, otherwise they're broken in >=wxGTK-2.9
	Layout();

	// Create the toolbars
	// If we requested to create a single toolbar, create a native toolbar
	// otherwise, we create a multiple toolbars using wxAUI toolbar if possible
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	if (options) {
		bool nativeToolbar = !PluginManager::Get()->AllowToolbar();
		if (options->GetIconsSize() == 16) {
			nativeToolbar ? CreateNativeToolbar16() : CreateToolbars16();
		} else {
			nativeToolbar ? CreateNativeToolbar24() : CreateToolbars24();
		}
	} else {
		CreateToolbars24();
	}

	ClangCodeCompletion::Instance()->Initialize(PluginManager::Get());

	GetWorkspacePane()->GetNotebook()->SetRightClickMenu( wxXmlResource::Get()->LoadMenu(wxT("workspace_view_rmenu")) );
	GetDebuggerPane()->GetNotebook()->SetRightClickMenu(wxXmlResource::Get()->LoadMenu( wxT("debugger_view_rmenu") ) );

	m_mgr.Update();
	SetAutoLayout (true);

	//load debuggers
	DebuggerMgr::Get().Initialize(this, EnvironmentConfig::Instance(), ManagerST::Get()->GetInstallDir());
	DebuggerMgr::Get().LoadDebuggers();

	wxString sessConfFile;
	sessConfFile << wxStandardPaths::Get().GetUserDataDir() << wxT("/config/sessions.xml");
	SessionManager::Get().Load(sessConfFile);

	// Now the session's loaded, it's safe to fill the tabgroups tab
	GetWorkspacePane()->GetTabgroupsTab()->DisplayTabgroups();

	//try to locate the build tools

	long fix(1);
	EditorConfigST::Get()->GetLongValue(wxT("FixBuildToolOnStartup"), fix);
	if ( fix ) {
		UpdateBuildTools();
	}

#if wxVERSION_NUMBER >= 2900
	// This is needed in >=wxGTK-2.9, otherwise the auinotebook doesn't fully expand at first
	SendSizeEvent(wxSEND_EVENT_POST);
	// Ditto the workspace pane auinotebook
	GetWorkspacePane()->SendSizeEvent(wxSEND_EVENT_POST);
#endif
}

void clMainFrame::CreateViewAsSubMenu()
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
			wxString lexName = lex->GetName();
			lexName.Trim().Trim(false);

			if(lexName.IsEmpty())
				continue;

			item = new wxMenuItem(submenu, minId, lex->GetName(), wxEmptyString, wxITEM_CHECK);
			m_viewAsMap[minId] = lex->GetName();
			minId++;
			submenu->Append(item);
		}
		menu->Append(viewAsSubMenuID, wxT("View As"), submenu);
	}
}

wxString clMainFrame::GetViewAsLanguageById(int id) const
{
	if (m_viewAsMap.find(id) == m_viewAsMap.end()) {
		return wxEmptyString;
	}
	return m_viewAsMap.find(id)->second;
}

void clMainFrame::CreateToolbars24()
{
	wxAuiPaneInfo info;

#if !USE_AUI_TOOLBAR
	wxWindow *toolbar_parent (this);
	if (PluginManager::Get()->AllowToolbar()) {
		toolbar_parent = m_mainPanel;
	}
#else
	wxWindow *toolbar_parent (m_mainPanel);
#endif

	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	clToolBar *tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
	tb->SetToolBitmapSize(wxSize(24, 24));

#if USE_AUI_TOOLBAR
	tb->SetArtProvider(new CLMainAuiTBArt());
#endif

	BitmapLoader &bmpLoader = *(PluginManager::Get()->GetStdIcons());
	tb->AddTool(XRCID("new_file"),        wxT("New"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_new")),     wxT("New File"));
	tb->AddTool(XRCID("open_file"),       wxT("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_open")),    wxT("Open File"));
	tb->AddTool(XRCID("refresh_file"),    wxT("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_reload")),  wxT("Reload File"));
	tb->AddSeparator();
	tb->AddTool(XRCID("save_file"),       wxT("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save")),    wxT("Save"));
	tb->AddTool(XRCID("save_file_as"),    wxT("Save As"),         bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save_as")), wxT("Save As"));
	tb->AddSeparator();
	tb->AddTool(XRCID("close_file"),      wxT("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_close")),   wxT("Close File"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT,                 wxT("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/cut")),          wxT("Cut"));
	tb->AddTool(wxID_COPY,                wxT("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/copy")),         wxT("Copy"));
	tb->AddTool(wxID_PASTE,               wxT("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/paste")),        wxT("Paste"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO,                wxT("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/undo")),         wxT("Undo"));
	tb->AddTool(wxID_REDO,                wxT("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/redo")),         wxT("Redo"));
	tb->AddTool(wxID_BACKWARD,            wxT("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/24/standard/back")),         wxT("Backward"));
	tb->AddTool(wxID_FORWARD,             wxT("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/24/standard/forward")),      wxT("Forward"));
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/24/standard/bookmark")),     wxT("Toggle Bookmark"));


	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());
	}
	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		info = wxAuiPaneInfo();
		tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
		tb->SetArtProvider(new CLMainAuiTBArt());
#endif
		tb->SetToolBitmapSize(wxSize(24, 24));
	}

	tb->AddTool(wxID_FIND,              wxT("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/24/search/find")),             wxT("Find"));
	tb->AddTool(wxID_REPLACE,           wxT("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_and_replace")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_in_files")),    wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/24/search/open_resource")),    wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"),     wxT("Find Type In Workspace"),     bmpLoader.LoadBitmap(wxT("toolbars/24/search/open_type")),        wxT("Find Type In Workspace"));
	tb->AddSeparator();
	tb->AddTool(XRCID("highlight_word"), wxT("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/24/search/mark_word")),        wxT("Highlight Word"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
	tb->AddSeparator();

	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Search")).ToolbarPane().Top());
	}
	//----------------------------------------------
	//create the build toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
		tb->SetArtProvider(new CLMainAuiTBArt());
#endif
		tb->SetToolBitmapSize(wxSize(24, 24));
	}

	tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/build")),        wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/stop")),         wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/clean")),        wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute")),      wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute_stop")), wxT("Stop Running Program"));

	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Build")).ToolbarPane().Top().Row(1));
	}

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
		tb->SetArtProvider(new CLMainAuiTBArt());
#endif
		tb->SetToolBitmapSize(wxSize(24, 24));
	}

	tb->AddTool(XRCID("start_debugger"),   wxT("Start / Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/start")),             wxT("Start / Continue debugger"));
	tb->AddTool(XRCID("stop_debugger"),    wxT("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/stop")),              wxT("Stop debugger"));
	tb->AddTool(XRCID("pause_debugger"),   wxT("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/interrupt")),         wxT("Pause debugger"));
	tb->AddTool(XRCID("restart_debugger"), wxT("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/restart")),           wxT("Restart debugger"));
	tb->AddSeparator();
	tb->AddTool(XRCID("show_cursor"),      wxT("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/show_current_line")), wxT("Show Current Line"));
	tb->AddSeparator();
	tb->AddTool(XRCID("dbg_stepin"),       wxT("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_in")),           wxT("Step In"));
	tb->AddTool(XRCID("dbg_next"),         wxT("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/next")),              wxT("Next"));
	tb->AddTool(XRCID("dbg_stepout"),      wxT("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_out")),          wxT("Step Out"));
	tb->Realize();

	if (PluginManager::Get()->AllowToolbar()) {
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Debug")).ToolbarPane().Top().Row(1));

		RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
		RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
		RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
		RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
	} else {
#if !USE_AUI_TOOLBAR
		SetToolBar(tb);
#else
		m_mgr.AddPane(tb, info.Name(wxT("Main Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Main Toolbar")).ToolbarPane().Top().Row(1));
#endif
	}
}
void clMainFrame::CreateNativeToolbar16()
{
	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	BitmapLoader &bmpLoader = *(PluginManager::Get()->GetStdIcons());

	tb->SetToolBitmapSize(wxSize(16, 16));
	tb->AddTool(XRCID("new_file"),        wxT("New"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_new")),     wxT("New File"));
	tb->AddTool(XRCID("open_file"),       wxT("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_open")),    wxT("Open File"));
	tb->AddTool(XRCID("refresh_file"),    wxT("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_reload")),  wxT("Reload File"));
	tb->AddSeparator();
	tb->AddTool(XRCID("save_file"),       wxT("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save")),    wxT("Save"));
	tb->AddTool(XRCID("save_file_as"),    wxT("Save As"),         bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save_as")), wxT("Save As"));
	tb->AddSeparator();
	tb->AddTool(XRCID("close_file"),      wxT("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_close")),   wxT("Close File"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT,                 wxT("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/cut")),          wxT("Cut"));
	tb->AddTool(wxID_COPY,                wxT("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/copy")),         wxT("Copy"));
	tb->AddTool(wxID_PASTE,               wxT("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/paste")),        wxT("Paste"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO,                wxT("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/undo")),         wxT("Undo"));
	tb->AddTool(wxID_REDO,                wxT("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/redo")),         wxT("Redo"));
	tb->AddTool(wxID_BACKWARD,            wxT("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/16/standard/back")),         wxT("Backward"));
	tb->AddTool(wxID_FORWARD,             wxT("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/16/standard/forward")),      wxT("Forward"));
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/16/standard/bookmark")),     wxT("Toggle Bookmark"));


	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	tb->AddSeparator();
	tb->AddTool(wxID_FIND,              wxT("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/16/search/find")),             wxT("Find"));
	tb->AddTool(wxID_REPLACE,           wxT("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_and_replace")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_in_files")),    wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/16/search/open_resource")),    wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"),     wxT("Find Type In Workspace"),     bmpLoader.LoadBitmap(wxT("toolbars/16/search/open_type")),        wxT("Find Type In Workspace"));
	tb->AddSeparator();
	tb->AddTool(XRCID("highlight_word"), wxT("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/16/search/mark_word")),        wxT("Highlight Word"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
	tb->AddSeparator();


	//----------------------------------------------
	//create the build toolbar
	//----------------------------------------------
	tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/build")),        wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/stop")),         wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/clean")),        wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute")),      wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute_stop")), wxT("Stop Running Program"));
	tb->AddSeparator();

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	tb->AddTool(XRCID("start_debugger"),   wxT("Start / Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/start")),             wxT("Start / Continue debugger"));
	tb->AddTool(XRCID("stop_debugger"),    wxT("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/stop")),              wxT("Stop debugger"));
	tb->AddTool(XRCID("pause_debugger"),   wxT("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/interrupt")),         wxT("Pause debugger"));
	tb->AddTool(XRCID("restart_debugger"), wxT("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/restart")),           wxT("Restart debugger"));
	tb->AddSeparator();
	tb->AddTool(XRCID("show_cursor"),      wxT("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/show_current_line")), wxT("Show Current Line"));
	tb->AddSeparator();
	tb->AddTool(XRCID("dbg_stepin"),       wxT("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_in")),           wxT("Step In"));
	tb->AddTool(XRCID("dbg_next"),         wxT("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/next")),              wxT("Next"));
	tb->AddTool(XRCID("dbg_stepout"),      wxT("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_out")),          wxT("Step Out"));

	tb->Realize();
	SetToolBar(tb);
}

void clMainFrame::CreateNativeToolbar24()
{
	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	BitmapLoader &bmpLoader = *(PluginManager::Get()->GetStdIcons());

	tb->SetToolBitmapSize(wxSize(16, 16));
	tb->AddTool(XRCID("new_file"),        wxT("New"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_new")),     wxT("New File"));
	tb->AddTool(XRCID("open_file"),       wxT("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_open")),    wxT("Open File"));
	tb->AddTool(XRCID("refresh_file"),    wxT("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_reload")),  wxT("Reload File"));
	tb->AddSeparator();
	tb->AddTool(XRCID("save_file"),       wxT("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save")),    wxT("Save"));
	tb->AddTool(XRCID("save_file_as"),    wxT("Save As"),         bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save_as")), wxT("Save As"));
	tb->AddSeparator();
	tb->AddTool(XRCID("close_file"),      wxT("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_close")),   wxT("Close File"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT,                 wxT("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/cut")),          wxT("Cut"));
	tb->AddTool(wxID_COPY,                wxT("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/copy")),         wxT("Copy"));
	tb->AddTool(wxID_PASTE,               wxT("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/paste")),        wxT("Paste"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO,                wxT("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/undo")),         wxT("Undo"));
	tb->AddTool(wxID_REDO,                wxT("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/redo")),         wxT("Redo"));
	tb->AddTool(wxID_BACKWARD,            wxT("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/24/standard/back")),         wxT("Backward"));
	tb->AddTool(wxID_FORWARD,             wxT("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/24/standard/forward")),      wxT("Forward"));
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/24/standard/bookmark")),     wxT("Toggle Bookmark"));


	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	tb->AddSeparator();
	tb->AddTool(wxID_FIND,              wxT("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/24/search/find")),             wxT("Find"));
	tb->AddTool(wxID_REPLACE,           wxT("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_and_replace")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_in_files")),    wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/24/search/open_resource")),    wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"),     wxT("Find Type In Workspace"),     bmpLoader.LoadBitmap(wxT("toolbars/24/search/open_type")),        wxT("Find Type In Workspace"));
	tb->AddSeparator();
	tb->AddTool(XRCID("highlight_word"), wxT("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/24/search/mark_word")),        wxT("Highlight Word"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
	tb->AddSeparator();

	//----------------------------------------------
	//create the build toolbar
	//----------------------------------------------
	tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/build")),        wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/stop")),         wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/clean")),        wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute")),      wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute_stop")), wxT("Stop Running Program"));
	tb->AddSeparator();

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	tb->AddTool(XRCID("start_debugger"),   wxT("Start / Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/start")),             wxT("Start / Continue debugger"));
	tb->AddTool(XRCID("stop_debugger"),    wxT("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/stop")),              wxT("Stop debugger"));
	tb->AddTool(XRCID("pause_debugger"),   wxT("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/interrupt")),         wxT("Pause debugger"));
	tb->AddTool(XRCID("restart_debugger"), wxT("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/restart")),           wxT("Restart debugger"));
	tb->AddSeparator();
	tb->AddTool(XRCID("show_cursor"),      wxT("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/show_current_line")), wxT("Show Current Line"));
	tb->AddSeparator();
	tb->AddTool(XRCID("dbg_stepin"),       wxT("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_in")),           wxT("Step In"));
	tb->AddTool(XRCID("dbg_next"),         wxT("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/next")),              wxT("Next"));
	tb->AddTool(XRCID("dbg_stepout"),      wxT("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_out")),          wxT("Step Out"));

	tb->Realize();
	SetToolBar(tb);
}

void clMainFrame::CreateToolbars16()
{
	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
#if !USE_AUI_TOOLBAR
	wxWindow *toolbar_parent (this);
	if (PluginManager::Get()->AllowToolbar()) {
		toolbar_parent = m_mainPanel;
	}
#else
	wxWindow *toolbar_parent (m_mainPanel);
#endif

	clToolBar *tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
	tb->SetArtProvider(new CLMainAuiTBArt());
#endif
	wxAuiPaneInfo info;

	BitmapLoader &bmpLoader = *(PluginManager::Get()->GetStdIcons());

	tb->SetToolBitmapSize(wxSize(16, 16));
	tb->AddTool(XRCID("new_file"),        wxT("New"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_new")),     wxT("New File"));
	tb->AddTool(XRCID("open_file"),       wxT("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_open")),    wxT("Open File"));
	tb->AddTool(XRCID("refresh_file"),    wxT("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_reload")),  wxT("Reload File"));
	tb->AddSeparator();
	tb->AddTool(XRCID("save_file"),       wxT("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save")),    wxT("Save"));
	tb->AddTool(XRCID("save_file_as"),    wxT("Save As"),         bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save_as")), wxT("Save As"));
	tb->AddSeparator();
	tb->AddTool(XRCID("close_file"),      wxT("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_close")),   wxT("Close File"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT,                 wxT("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/cut")),          wxT("Cut"));
	tb->AddTool(wxID_COPY,                wxT("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/copy")),         wxT("Copy"));
	tb->AddTool(wxID_PASTE,               wxT("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/paste")),        wxT("Paste"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO,                wxT("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/undo")),         wxT("Undo"));
	tb->AddTool(wxID_REDO,                wxT("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/redo")),         wxT("Redo"));
	tb->AddTool(wxID_BACKWARD,            wxT("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/16/standard/back")),         wxT("Backward"));
	tb->AddTool(wxID_FORWARD,             wxT("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/16/standard/forward")),      wxT("Forward"));
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/16/standard/bookmark")),     wxT("Toggle Bookmark"));


	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());
	}

	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	info = wxAuiPaneInfo();

	if (PluginManager::Get()->AllowToolbar()) {
		tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
		tb->SetArtProvider(new CLMainAuiTBArt());
#endif
		tb->SetToolBitmapSize(wxSize(16, 16));
	}

	tb->AddTool(wxID_FIND,              wxT("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/16/search/find")),             wxT("Find"));
	tb->AddTool(wxID_REPLACE,           wxT("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_and_replace")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_in_files")),    wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/16/search/open_resource")),    wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"),     wxT("Find Type In Workspace"),     bmpLoader.LoadBitmap(wxT("toolbars/16/search/open_type")),        wxT("Find Type In Workspace"));
	tb->AddSeparator();
	tb->AddTool(XRCID("highlight_word"), wxT("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/16/search/mark_word")),        wxT("Highlight Word"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
	tb->AddSeparator();


	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Search")).ToolbarPane().Top());

		//----------------------------------------------
		//create the build toolbar
		//----------------------------------------------
		tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
		tb->SetArtProvider(new CLMainAuiTBArt());
#endif
		tb->SetToolBitmapSize(wxSize(16, 16));
	}

	tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/build")),        wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/stop")),         wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/clean")),        wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute")),      wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute_stop")), wxT("Stop Running Program"));

	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Build")).ToolbarPane().Top().Row(1));
	}

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
		tb->SetArtProvider(new CLMainAuiTBArt());
#endif
		tb->SetToolBitmapSize(wxSize(16, 16));
	}

	tb->AddTool(XRCID("start_debugger"),   wxT("Start / Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/start")),             wxT("Start / Continue debugger"));
	tb->AddTool(XRCID("stop_debugger"),    wxT("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/stop")),              wxT("Stop debugger"));
	tb->AddTool(XRCID("pause_debugger"),   wxT("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/interrupt")),         wxT("Pause debugger"));
	tb->AddTool(XRCID("restart_debugger"), wxT("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/restart")),           wxT("Restart debugger"));
	tb->AddSeparator();
	tb->AddTool(XRCID("show_cursor"),      wxT("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/show_current_line")), wxT("Show Current Line"));
	tb->AddSeparator();
	tb->AddTool(XRCID("dbg_stepin"),       wxT("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_in")),           wxT("Step In"));
	tb->AddTool(XRCID("dbg_next"),         wxT("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/next")),              wxT("Next"));
	tb->AddTool(XRCID("dbg_stepout"),      wxT("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_out")),          wxT("Step Out"));

	tb->Realize();
	if (PluginManager::Get()->AllowToolbar()) {
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Debug")).ToolbarPane().Top().Row(1));
		RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
		RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
		RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
		RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
	} else {
#if !USE_AUI_TOOLBAR
		SetToolBar(tb);
#else
		m_mgr.AddPane(tb, info.Name(wxT("Main Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Main Toolbar")).ToolbarPane().Top().Row(1));
#endif
	}
}

void clMainFrame::UpdateBuildTools()
{
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder ( wxT ( "GNU makefile for g++/gcc" ) );
	wxString tool = builder->GetBuildToolName();
	wxString origTool ( tool );

	//confirm that it exists...
	wxString path;
	bool is_ok ( true );

	// Apply the environment
	EnvSetter env;

	if ( tool.Contains ( wxT ( "$" ) ) ) {
		//expand
		tool = EnvironmentConfig::Instance()->ExpandVariables ( tool, false );
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
		return;
	}

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
	BuilderConfigPtr bsptr = BuildSettingsConfigST::Get()->GetBuilderConfig ( wxT ( "GNU makefile for g++/gcc" ) );
	if ( !bsptr ) {
		bsptr = new BuilderConfig ( NULL );
		bsptr->SetName ( wxT ( "GNU makefile for g++/gcc" ) );
	}

	bsptr->SetToolPath ( tool );
	BuildSettingsConfigST::Get()->SetBuildSystem ( bsptr );
}

void clMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}
//----------------------------------------------------
// Helper method for the event handling
//----------------------------------------------------

bool clMainFrame::IsEditorEvent(wxEvent &event)
{
#ifdef __WXGTK__
	MainBook *mainBook   = GetMainBook();
	if(!mainBook || !mainBook->GetActiveEditor()) {
		return false;
	}

	switch (event.GetId())
	{
	case wxID_CUT:
	case wxID_SELECTALL:
	case wxID_COPY:
	case wxID_PASTE: {
		bool isFocused = mainBook->GetActiveEditor()->IsFocused();
		return isFocused;
	}
	default:
		break;
	}

#else

	// Handle common edit events
	// if the focused window is *not* LEditor,
	// and the focused windows is of type
	// wxTextCtrl or wxScintilla, let the focused
	// Window handle the event
	wxWindow *focusWin = wxWindow::FindFocus();
	if ( focusWin ) {
		switch (event.GetId())
		{
		case wxID_CUT:
		case wxID_SELECTALL:
		case wxID_COPY:
		case wxID_PASTE: {
			LEditor *ed = dynamic_cast<LEditor*>(focusWin);
			if ( !ed ) {
				// let other controls handle it
				return false;
			}
			break;
		}
		default:
			break;
		}
	}
#endif
	return true;
}

void clMainFrame::DispatchCommandEvent(wxCommandEvent &event)
{
	if ( !IsEditorEvent(event) ) {
		event.Skip();
		return;
	}

	// Do the default and pass this event to the Editor
	LEditor* editor = GetMainBook()->GetActiveEditor();
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

void clMainFrame::DispatchUpdateUIEvent(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();

	if ( !IsEditorEvent(event) ) {
		event.Skip();
		return;
	}

	LEditor* editor = GetMainBook()->GetActiveEditor();
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

void clMainFrame::OnFileExistUpdateUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();

	LEditor* editor = GetMainBook()->GetActiveEditor();
	if ( !editor ) {
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}

void clMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString mainTitle;
	mainTitle = CODELITE_VERSION_STR;

	AboutDlg dlg(this, mainTitle);
	dlg.SetInfo(mainTitle);
	dlg.ShowModal();
}

void clMainFrame::OnClose(wxCloseEvent& event)
{
	ManagerST::Get()->SetShutdownInProgress(true);

	// Stop the search thread
	ManagerST::Get()->KillProgram();
	ManagerST::Get()->DbgStop();
	SearchThreadST::Get()->StopSearch();

	SaveLayoutAndSession();

	// In case we got some data in the clipboard, flush it so it will be available
	// after our process exits
	wxTheClipboard->Flush();
	event.Skip();
}

void clMainFrame::LoadSession(const wxString &sessionName)
{
	SessionEntry session;

	if (SessionManager::Get().FindSession(sessionName, session)) {
		wxString wspFile = session.GetWorkspaceName();
		if (wspFile.IsEmpty() == false && wspFile != wxT("Default")) {
			ManagerST::Get()->OpenWorkspace(wspFile);
		} else {
			// no workspace to open, so just restore any previously open editors
			GetMainBook()->RestoreSession(session);
		}
	}
}

void clMainFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->SaveFile();
	}
}

void clMainFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->SaveFileAs();
	}
}

void clMainFrame::OnFileLoadTabGroup(wxCommandEvent& WXUNUSED(event))
{
	wxArrayString previousgroups;
	EditorConfigST::Get()->GetRecentItems( previousgroups, wxT("RecentTabgroups") );

	// Check the previous items still exist
	for (int n = (int)previousgroups.GetCount()-1; n >= 0; --n) {
		if (!wxFileName::FileExists(previousgroups.Item(n)) ) {
			previousgroups.RemoveAt(n);
		}
	}
	EditorConfigST::Get()->SetRecentItems( previousgroups, wxT("RecentTabgroups") ); // In case any were deleted

	wxString path = ManagerST::Get()->IsWorkspaceOpen() ? WorkspaceST::Get()->GetWorkspaceFileName().GetPath() : wxGetHomeDir();
	LoadTabGroupDlg dlg(this, path, previousgroups);

	// Disable the 'Replace' checkbox if there aren't any editors to replace
	std::vector<LEditor*> editors;
	GetMainBook()->GetAllEditors(editors);
	dlg.EnableReplaceCheck(editors.size());

	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	wxString filepath = dlg.GetListBox()->GetStringSelection();
	wxString sessionFilepath = filepath.BeforeLast(wxT('.'));

	wxWindowUpdateLocker locker(this);
	TabGroupEntry session;
	if (SessionManager::Get().FindSession(sessionFilepath, session, wxString(wxT(".tabgroup")), tabgroupTag) ) {
		// We've 'loaded' the requested tabs. If required, delete any current ones
		if ( dlg.GetReplaceCheck() ) {
			GetMainBook()->CloseAll(true);
		}
		GetMainBook()->RestoreSession(session);

		// Remove any previous instance of this group from the history, then prepend it and save
		int index = previousgroups.Index(filepath);
		if (index != wxNOT_FOUND) {
			previousgroups.RemoveAt(index);
		}
		previousgroups.Insert(filepath, 0);
		EditorConfigST::Get()->SetRecentItems( previousgroups, wxT("RecentTabgroups") );
	}
}

void clMainFrame::OnFileReload(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		if ( editor->GetModify() ) {
			// Ask user if he really wants to lose all changes
			wxString msg;
			msg << wxT("The file '") << editor->GetFileName().GetFullName() << wxT("' has been altered.\n");
			msg << wxT("Are you sure you want to lose all changes?");
			if ( wxMessageBox(msg, wxT("Confirm"), wxYES_NO, this) != wxYES ) {
				return;
			}
		}
		editor->ReloadFile();
	}
}

void clMainFrame::OnCloseWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen()) {
		ManagerST::Get()->CloseWorkspace();
		ShowWelcomePage();
	}
}

void clMainFrame::OnSwitchWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);

	// now it is time to prompt user for new workspace to open
	const wxString ALL(wxT("CodeLite Workspace files (*.workspace)|*.workspace|")
	                   wxT("All Files (*)|*"));
	wxFileDialog dlg(this, wxT("Open Workspace"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE , wxDefaultPosition);
	if (dlg.ShowModal() == wxID_OK) {
		wxWindowUpdateLocker locker(this);
		ManagerST::Get()->OpenWorkspace(dlg.GetPath());
	}
}

void clMainFrame::OnCompleteWord(wxCommandEvent& event)
{
	wxUnusedVar(event);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->CompleteWord();
	}
}

void clMainFrame::OnFunctionCalltip(wxCommandEvent& event)
{
	wxUnusedVar(event);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->ShowFunctionTipFromCurrentPos();
	}
}

// Open new file
void clMainFrame::OnFileNew(wxCommandEvent &event)
{
	wxUnusedVar(event);
	GetMainBook()->NewEditor();
}

void clMainFrame::OnFileOpen(wxCommandEvent & WXUNUSED(event))
{
	const wxString ALL(	wxT("All Files (*)|*"));
	static wxString s_openPath(wxEmptyString);

	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open File"), s_openPath, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg->GetPaths(paths);
		for (size_t i=0; i<paths.GetCount(); i++) {
			GetMainBook()->OpenFile(paths.Item(i));
		}

		if (paths.GetCount() > 0) {
			wxFileName fn(paths.Item(0));
			s_openPath = fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
		}
	}
	dlg->Destroy();
}

void clMainFrame::OnFileClose(wxCommandEvent &event)
{
	wxUnusedVar( event );
	wxWindowUpdateLocker locker(this);
	GetMainBook()->ClosePage(GetMainBook()->GetCurrentPage());
}

void clMainFrame::OnFileSaveAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	GetMainBook()->SaveAll(false, true);
}

void clMainFrame::OnFileSaveTabGroup(wxCommandEvent& WXUNUSED(event))
{
	wxArrayString previousgroups;
	EditorConfigST::Get()->GetRecentItems( previousgroups, wxT("RecentTabgroups") );

	SaveTabGroupDlg dlg(this, previousgroups);

	std::vector<LEditor*> editors;
	wxArrayString filepaths;
	bool retain_order(true);
	GetMainBook()->GetAllEditors(editors, retain_order);	// We'll want the order of intArr to match the order in MainBook::SaveSession
	for (size_t i = 0; i < editors.size(); ++i) {
		filepaths.Add(editors[i]->GetFileName().GetFullPath());
	}
	dlg.SetListTabs(filepaths);

	while (true) {

		if (dlg.ShowModal() != wxID_OK) {
			return;
		}

		wxString sessionName = dlg.GetTabgroupName();
		if (sessionName.IsEmpty()) {
			if ( wxMessageBox(_("Please enter a name for the tab group"), wxT("CodeLite"), wxICON_ERROR|wxOK|wxCANCEL, this) != wxOK ) {
				return;
			} else {
				continue;
			}
		}

		wxString path = TabGroupsManager::Get()->GetTabgroupDirectory();

		if (path.Right(1) != wxFileName::GetPathSeparator()) {
			path << wxFileName::GetPathSeparator();
		}
		wxString filepath(path + sessionName + wxT(".tabgroup"));
		if (wxFileName::FileExists(filepath)) {
			if ( wxMessageBox(_("There is already a file with this name. Do you want to overwrite it?"), _("Are you sure?"), wxICON_EXCLAMATION|wxOK|wxCANCEL, this) != wxOK ) {
				return;
			}
		}

		wxArrayInt intArr;
		if ( dlg.GetChoices(intArr) ) { // Don't bother to save if no tabs were selected
			TabGroupEntry session;
			session.SetTabgroupName(path + sessionName);
			GetMainBook()->SaveSession(session, intArr);
			SessionManager::Get().Save(session.GetTabgroupName(), session, wxString(wxT(".tabgroup")), tabgroupTag);
			// Add the new tabgroup to the tabgroup manager and pane
			GetWorkspacePane()->GetTabgroupsTab()->AddNewTabgroupToTree(filepath);

			// Remove any previous instance of this group from the history, then prepend it and save
			int index = previousgroups.Index(filepath);
			if (index != wxNOT_FOUND) {
				previousgroups.RemoveAt(index);
			}
			previousgroups.Insert(filepath, 0);
			EditorConfigST::Get()->SetRecentItems( previousgroups, wxT("RecentTabgroups") );

			SetStatusMessage(wxT("Tab group saved"), 0);
		}

		return;
	}
}

void clMainFrame::OnCompleteWordUpdateUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();

	LEditor* editor = GetMainBook()->GetActiveEditor();
	// This menu item is enabled only if the current editor belongs to a project
	event.Enable(editor && ManagerST::Get()->IsWorkspaceOpen());
}

void clMainFrame::OnWorkspaceOpen(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

// Project->New Workspace
void clMainFrame::OnProjectNewWorkspace(wxCommandEvent &event)
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
void clMainFrame::OnProjectNewProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	NewProjectDlg *dlg = new NewProjectDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
		ProjectData data = dlg->GetProjectData();
		ManagerST::Get()->CreateProject(data);
	}
	dlg->Destroy();
}

void clMainFrame::OnProjectAddProject(wxCommandEvent &event)
{
	wxUnusedVar(event);

	// Prompt user for project path
	const wxString ALL(	wxT("CodeLite Projects (*.project)|*.project|")
	                    wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Project"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		// Open it
		ManagerST::Get()->AddProject(dlg->GetPath());
	}
	dlg->Destroy();
}

void clMainFrame::OnCtagsOptions(wxCommandEvent &event)
{
	wxUnusedVar(event);

	bool colVars(false);
	bool colTags(false);
	bool newColVars(false);
	bool newColTags(false);
	bool markFilesAsBold(false);
	bool newMarkFilesAsBold(false);

	size_t colourTypes(0);

	colVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
	colTags = (m_tagsOptionsData.GetFlags() & CC_COLOUR_WORKSPACE_TAGS ? true : false);
	markFilesAsBold = (m_tagsOptionsData.GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD ? true : false);

	colourTypes = m_tagsOptionsData.GetCcColourFlags();

	wxArrayString pathsBefore = m_tagsOptionsData.GetParserSearchPaths();
	TagsOptionsDlg dlg(this, m_tagsOptionsData);
	if (dlg.ShowModal() == wxID_OK) {
		m_tagsOptionsData = dlg.GetData();

		// We call here to ToString() only because ToString() internally
		// writes the content into the ctags.replacements file (used by
		// codelite_indexer)
		m_tagsOptionsData.ToString();

		wxArrayString pathsAfter = m_tagsOptionsData.GetParserSearchPaths();
		wxArrayString removedPaths;

		// Compare the paths
		for (size_t i=0; i<pathsBefore.GetCount(); i++) {
			int where = pathsAfter.Index(pathsBefore.Item(i));
			if (where == wxNOT_FOUND) {
				removedPaths.Add( pathsBefore.Item(i) );
			} else {
				pathsAfter.RemoveAt((size_t)where);
			}
		}

		if ( removedPaths.IsEmpty() == false ) {
			wxWindowDisabler disableAll;
			wxBusyInfo info(_T("Updating tags database, please wait..."), this);
			wxTheApp->Yield();

			// Remove all tags from the database which starts with the paths which were
			// removed from the parser include path
			ITagsStorage *db = TagsManagerST::Get()->GetDatabase();
			db->Begin();
			for (size_t i=0; i<removedPaths.GetCount(); i++) {
				db->DeleteByFilePrefix     (wxFileName(), removedPaths.Item(i));
				db->DeleteFromFilesByPrefix(wxFileName(), removedPaths.Item(i));
				wxTheApp->Yield();
			}
			db->Commit();
		}

		newColVars         = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS             ? true : false);
		newColTags         = (m_tagsOptionsData.GetFlags() & CC_COLOUR_WORKSPACE_TAGS   ? true : false);
		newMarkFilesAsBold = (m_tagsOptionsData.GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD ? true : false);

		TagsManagerST::Get()->SetCtagsOptions( m_tagsOptionsData );
		EditorConfigST::Get()->WriteObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);

		// We use this method 'UpdateParserPaths' since it will also update the parser
		// thread with any workspace search/exclude paths related
		ManagerST::Get()->UpdateParserPaths();

		TagsManagerST::Get()->GetDatabase()->SetMaxWorkspaceTagToColour( m_tagsOptionsData.GetMaxItemToColour() );

		//do we need to colourise?
		if (newColTags != colTags || newColVars != colVars || colourTypes != m_tagsOptionsData.GetCcColourFlags()) {
			GetMainBook()->UpdateColours();
		}

		// do we need to update the file tree to mark tags files
		// as bold?
		if (markFilesAsBold != newMarkFilesAsBold) {
			TagsManagerST::Get()->NotifyFileTree(newMarkFilesAsBold);
		}

		if (pathsAfter.IsEmpty() == false) {
			// a retagg is needed
			wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
			AddPendingEvent(e);
		}
	}
}

void clMainFrame::RegisterToolbar(int menuItemId, const wxString &name)
{
	m_toolbars[menuItemId] = name;
	Connect(menuItemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnViewToolbar), NULL, this);
	Connect(menuItemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::OnViewToolbarUI), NULL, this);
}

void clMainFrame::RegisterDockWindow(int menuItemId, const wxString &name)
{
	m_panes[menuItemId] = name;
	Connect(menuItemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnViewPane), NULL, this);
	Connect(menuItemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::OnViewPaneUI), NULL, this);
}

void clMainFrame::OnViewToolbar(wxCommandEvent &event)
{
	std::map<int, wxString>::iterator iter = m_toolbars.find(event.GetId());
	if (iter != m_toolbars.end()) {
		ViewPane(iter->second, event);
	}
}

void clMainFrame::OnViewToolbarUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	std::map<int, wxString>::iterator iter = m_toolbars.find(event.GetId());
	if (iter != m_toolbars.end()) {
		ViewPaneUI(iter->second, event);
	}
}

void clMainFrame::OnViewPane(wxCommandEvent &event)
{
	std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
	if (iter != m_panes.end()) {
		ViewPane(iter->second, event);
	}
}

void clMainFrame::OnViewPaneUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
	if (iter != m_panes.end()) {
		ViewPaneUI(iter->second, event);
	}
}

void clMainFrame::ViewPane(const wxString &paneName, wxCommandEvent &event)
{
	if (paneName == wxT("Output View")) {
		ManagerST::Get()->ToggleOutputPane( !event.IsChecked() );

	} else {
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

#if wxVERSION_NUMBER >= 2900
	// This is needed in >=wxGTK-2.9, otherwise output pane doesn't fully expand, or on closing the auinotebook doesn't occupy its space
	SendSizeEvent(wxSEND_EVENT_POST);
#endif
}

void clMainFrame::ViewPaneUI(const wxString &paneName, wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	wxAuiPaneInfo &info = m_mgr.GetPane(paneName);
	if (info.IsOk()) {
		event.Check(info.IsShown());
	}
}

void clMainFrame::OnViewOptions(wxCommandEvent & WXUNUSED( event))
{
	OptionsDlg2 dlg(this);
	dlg.ShowModal();

	if ( dlg.restartRquired ) {
		DoSuggestRestart();
	}
}

void clMainFrame::OnTogglePanes(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->TogglePanes();
}

void clMainFrame::OnAddEnvironmentVariable(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarsTableDlg dlg(this);
	dlg.ShowModal();
	SelectBestEnvSet();

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
}

void clMainFrame::OnAdvanceSettings(wxCommandEvent &event)
{
	size_t selected_page(0);
	if (event.GetInt() == 1) {
		selected_page = 1;
	}

	AdvancedDlg dlg(this, selected_page);
	if (dlg.ShowModal() == wxID_OK) {
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
	SelectBestEnvSet();
}

void clMainFrame::OnBuildEnded(wxCommandEvent &event)
{
	event.Skip();
	if (m_buildAndRun) {
		//If the build process was part of a 'Build and Run' command, check whether an erros
		//occured during build process, if non, launch the output
		m_buildAndRun = false;
		if ( ManagerST::Get()->IsBuildEndedSuccessfully() ||
		        wxMessageBox(_("Build ended with errors. Continue?"), wxT("Confirm"), wxYES_NO| wxICON_QUESTION, this) == wxYES) {
			ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
		}
	}
	// Process next command from the queue
	ManagerST::Get()->ProcessCommandQueue();
}

// Build operations
void clMainFrame::OnBuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {

		SetStatusMessage(wxT("Build starting..."), 0);

		wxString conf, projectName;
		projectName = ManagerST::Get()->GetActiveProjectName();

		// get the selected configuration to be built
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
		if (bldConf) {
			conf = bldConf->GetName();
		}

		QueueCommand info(projectName, conf, false, QueueCommand::Build);
		if (bldConf && bldConf->IsCustomBuild()) {
			info.SetKind(QueueCommand::CustomBuild);
			info.SetCustomBuildTarget(wxT("Build"));
		}
		ManagerST::Get()->PushQueueCommand( info );
		ManagerST::Get()->ProcessCommandQueue();

		SetStatusMessage(wxT("Done"), 0);
	}
}

void clMainFrame::OnBuildCustomTarget(wxCommandEvent& event)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {

		wxString projectName, targetName;
		// get the project name
		TreeItemInfo item = GetWorkspaceTab()->GetFileView()->GetSelectedItemInfo();
		if (item.m_itemType != ProjectItem::TypeProject) {
			return;
		}

		// set teh project name
		projectName = item.m_text;

		// get the selected configuration to be built
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
		if (bldConf) {
			std::map<wxString, wxString> targets = bldConf->GetCustomTargets();
			std::map<wxString, wxString>::iterator iter = targets.begin();
			for (; iter != targets.end(); iter++) {
				if (wxXmlResource::GetXRCID(iter->first.c_str()) == event.GetId()) {
					targetName = iter->first;
					break;
				}
			}

			if (targetName.IsEmpty()) {
				wxLogMessage(wxString::Format(wxT("Failed to find Custom Build Target for event ID=%d"), event.GetId()));
				return;
			}

			QueueCommand info(projectName, bldConf->GetName(), false, QueueCommand::CustomBuild);
			info.SetCustomBuildTarget(targetName);

			ManagerST::Get()->PushQueueCommand(info);
			ManagerST::Get()->ProcessCommandQueue();
		}
	}
}

void clMainFrame::OnBuildAndRunProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {
		m_buildAndRun = true;

		wxString projectName = ManagerST::Get()->GetActiveProjectName();
		wxString conf;
		// get the selected configuration to be built
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
		if (bldConf) {
			conf = bldConf->GetName();
		}

		QueueCommand info(projectName, conf, false, QueueCommand::Build);

		if (bldConf && bldConf->IsCustomBuild()) {
			info.SetKind(QueueCommand::CustomBuild);
			info.SetCustomBuildTarget(wxT("Build"));
		}

		ManagerST::Get()->PushQueueCommand( info );
		ManagerST::Get()->ProcessCommandQueue();
	}
}

void clMainFrame::OnRebuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	RebuildProject(ManagerST::Get()->GetActiveProjectName());
}

void clMainFrame::OnBuildProjectUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	event.Enable(enable);
}

void clMainFrame::OnStopExecutedProgramUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	Manager *mgr = ManagerST::Get();
	bool enable = mgr->IsProgramRunning();
	event.Enable(enable);
}

void clMainFrame::OnStopBuildUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	Manager *mgr = ManagerST::Get();
	bool enable = mgr->IsBuildInProgress();
	event.Enable(enable);
}

void clMainFrame::OnStopBuild(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsBuildInProgress()) {
		mgr->StopBuild();
	}
}

void clMainFrame::OnStopExecutedProgram(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsProgramRunning()) {
		mgr->KillProgram();
	}
}

void clMainFrame::OnCleanProject(wxCommandEvent &event)
{
	wxUnusedVar(event);

	wxString conf, projectName;
	projectName = ManagerST::Get()->GetActiveProjectName();

	// get the selected configuration to be built
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
	if (bldConf) {
		conf = bldConf->GetName();
	}

	QueueCommand buildInfo(projectName, conf, false, QueueCommand::Clean);
	if (bldConf && bldConf->IsCustomBuild()) {
		buildInfo.SetKind(QueueCommand::CustomBuild);
		buildInfo.SetCustomBuildTarget(wxT("Clean"));
	}
	ManagerST::Get()->PushQueueCommand(buildInfo);
	ManagerST::Get()->ProcessCommandQueue();
}

void clMainFrame::OnCleanProjectUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	event.Enable(enable);
}

void clMainFrame::OnExecuteNoDebug(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString projectName;

	projectName = ManagerST::Get()->GetActiveProjectName();
	if (projectName.IsEmpty() == false) {
		ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
	}
}

void clMainFrame::OnExecuteNoDebugUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	event.Enable(ManagerST::Get()->GetActiveProjectName().IsEmpty() == false
	             &&
	             !ManagerST::Get()->IsBuildInProgress()
	             &&
	             !ManagerST::Get()->IsProgramRunning());
}

void clMainFrame::OnTimer(wxTimerEvent &event)
{
	// since there is a bug in wxURL, which it can not be used while constucting a wxFrame,
	// it must be called *after* the frame constuction
	// add new version notification updater
	long check(1);
	long updatePaths(1);

	EditorConfigST::Get()->GetLongValue(wxT("CheckNewVersion"),   check);
	EditorConfigST::Get()->GetLongValue(wxT("UpdateParserPaths"), updatePaths);
	if ( check ) {
		JobQueueSingleton::Instance()->PushJob(new WebUpdateJob(this, false));
	}

	//update the build system to contain the number of CPUs
	int cpus = wxThread::GetCPUCount();
	if (cpus != wxNOT_FOUND) {
		//update the build system
		BuilderConfigPtr bs = BuildSettingsConfigST::Get()->GetBuilderConfig(wxT("GNU makefile for g++/gcc"));
		if ( bs ) {
			wxString jobs;
			jobs << cpus;

			if ( bs->GetToolJobs() != jobs ) {

				ButtonDetails btn1;
				btn1.buttonLabel = wxT("Update Number of Build Processes");
				btn1.commandId   = XRCID("update_num_builders_count");
				btn1.window      = this;

				GetMainBook()->ShowMessage(wxT("Should CodeLite adjust the number of concurrent build jobs to match the number of CPUs?"), true,
				PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/settings")), btn1);
			}
		}

		// enable/disable plugins toolbar functionality
		PluginManager::Get()->EnableToolbars();

		// Check that the user has some paths set in the parser
		EditorConfigST::Get()->ReadObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);

		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////

		// There are 2 conditions that we check here:
		// 1) if there are no search paths set
		// 2) there are search paths, but some or all of them are no longer exist on the system

		bool isUpdatePathRequired (false);
		bool allPathsExists       (true);
		bool hasSearchPath        (true);

		hasSearchPath = m_tagsOptionsData.GetParserSearchPaths().IsEmpty() == false;

		for (size_t i=0; i<m_tagsOptionsData.GetParserSearchPaths().GetCount(); i++) {
			if (wxFileName::DirExists(m_tagsOptionsData.GetParserSearchPaths().Item(i)) == false) {
				allPathsExists = false;
				break;
			}
		}
		isUpdatePathRequired = (!allPathsExists || !hasSearchPath);

		if ( isUpdatePathRequired ) {
			// Try to locate the paths automatically
			wxArrayString paths;
			wxArrayString excudePaths;
			IncludePathLocator locator(PluginManager::Get());
			locator.Locate( paths, excudePaths );

			if ( !hasSearchPath && paths.IsEmpty() && updatePaths) {
				GetMainBook()->ShowMessage(
				    wxT("CodeLite could not find any search paths set for the code completion parser\n")
				    wxT("This means that CodeLite will *NOT* be able to offer any code completion for non-workspace files (e.g. string.h).\n")
				    wxT("To fix this, please set search paths for the parser\n")
				    wxT("This can be done from the main menu: Settings > Tags Settings > Include Files"));

			} else {
				if (updatePaths) {

					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("update_parser_paths"));
					this->ProcessEvent(evt);
					GetMainBook()->ShowMessage( wxT("Code Completion search paths have been updated") );

				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////

		//send initialization end event
		SendCmdEvent(wxEVT_INIT_DONE);
	}

	//clear navigation queue
	if (GetMainBook()->GetCurrentPage() == 0) {
		NavMgr::Get()->Clear();
	}

	// Load last session?
	if (m_frameGeneralInfo.GetFlags() & CL_LOAD_LAST_SESSION) {
		wxWindowUpdateLocker locker(this);
		LoadSession(SessionManager::Get().GetLastSession());
	}

	// ReTag workspace database if needed (this can happen due to schema version changes)
	// It is important to place the retag code here since the retag workspace should take place after
	// the parser search patha have been updated (if needed)
	if( m_workspaceRetagIsRequired ) {
		m_workspaceRetagIsRequired = false;
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
		this->AddPendingEvent(evt);
		GetMainBook()->ShowMessage(wxT("Your workspace symbols file does not match the current version of CodeLite. CodeLite will perform a full retag of the workspace"));
	}

	// For some reason, under Linux we need to force the menu accelerator again
	// otherwise some shortcuts are getting lose (e.g. Ctrl-/ to comment line)
	ManagerST::Get()->UpdateMenuAccelerators();
	event.Skip();
}

void clMainFrame::OnFileCloseAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	GetMainBook()->CloseAll(true);
}

void clMainFrame::OnQuickOutline(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen() == false)
		return;

	if (!GetMainBook()->GetActiveEditor())
		return;

	if (GetMainBook()->GetActiveEditor()->GetProject().IsEmpty())
		return;

	QuickOutlineDlg dlg(this,
						GetMainBook()->GetActiveEditor()->GetFileName().GetFullPath(),
						wxID_ANY,
						wxT(""),
						wxDefaultPosition,
						wxSize(400, 400),
#if wxVERSION_NUMBER < 2900
						wxNO_BORDER
#else
						wxDEFAULT_DIALOG_STYLE
#endif
						);

	dlg.ShowModal();

#ifdef __WXMAC__
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->SetActive();
	}
#endif
}

void clMainFrame::OnAddSymbols(SymbolTreeEvent &event)
{
	// Notify the plugins
	ParseThreadEventData data;
	data.SetFileName(event.GetFileName());
	data.SetItems(event.GetItems());

	SendCmdEvent(wxEVT_SYNBOL_TREE_ADD_ITEM, (void*)&data);
}

void clMainFrame::OnDeleteSymbols(SymbolTreeEvent &event)
{
	// Notify the plugins
	ParseThreadEventData data;
	data.SetFileName(event.GetFileName());
	data.SetItems(event.GetItems());

	SendCmdEvent(wxEVT_SYNBOL_TREE_DELETE_ITEM, (void*)&data);

}

void clMainFrame::OnUpdateSymbols(SymbolTreeEvent &event)
{
	// Notify the plugins
	ParseThreadEventData data;
	data.SetFileName(event.GetFileName());
	data.SetItems(event.GetItems());

	SendCmdEvent(wxEVT_SYNBOL_TREE_UPDATE_ITEM, (void*)&data);
}

wxString clMainFrame::CreateWorkspaceTable()
{
	wxString html;
	wxArrayString files;
	Manager *mgr = ManagerST::Get();
	mgr->GetRecentlyOpenedWorkspaces(files);

	wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	bgclr = DrawingUtils::LightColour(bgclr, 7);

	html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
	if (files.GetCount() == 0) {
		html << wxT("<tr><td><font size=2 face=\"Verdana\">");
		html << wxT("No workspaces found.") << wxT("</font></td></tr>");
	} else {
		wxColour baseColour (232, 166, 101);
		wxColour firstColour  = DrawingUtils::LightColour(baseColour, 6);
		wxColour secondColour = DrawingUtils::LightColour(baseColour, 8);
		for (int i=(int)files.GetCount(); i>0; --i) {
			wxFileName fn( files.Item(i-1) );

			wxColour lineCol;
			if( i % 2 ) {
				lineCol = firstColour;
			} else {
				lineCol = secondColour;
			}

			html << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX)<< wxT("\">")
			<< wxT("<td><font size=2 face=\"Verdana\">")
			<< wxT("<a href=\"action:open-file:")
			<< fn.GetFullPath() << wxT("\" >")
			<< fn.GetName()
			<< wxT("</a></font></td>")
			<< wxT("<td><font size=2 face=\"Verdana\">") << fn.GetFullPath() << wxT("</font></td>")
			<< wxT("</tr>");
		}
	}

	html << wxT("</table>");
	return html;
}

wxString clMainFrame::CreateFilesTable()
{
	wxString html;
	wxArrayString files;
	GetMainBook()->GetRecentlyOpenedFiles(files);

	wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	bgclr = DrawingUtils::LightColour(bgclr, 10.0);
	html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
	if (files.GetCount() == 0) {
		html << wxT("<tr><td><font size=2 face=\"Verdana\">");
		html << wxT("No files found.") << wxT("</font></td></tr>");
	} else {
		wxColour baseColour (232, 166, 101);
		wxColour firstColour  = DrawingUtils::LightColour(baseColour, 6);
		wxColour secondColour = DrawingUtils::LightColour(baseColour, 8);
		for (int i=(int)files.GetCount(); i>0; --i) {

			wxFileName fn( files.Item(i-1) );
			wxColour lineCol;
			if( i % 2 ) {
				lineCol = firstColour;
			} else {
				lineCol = secondColour;
			}
			html << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX)<< wxT("\">")
			<< wxT("<td><font size=2 face=\"Verdana\">")
			<< wxT("<a href=\"action:open-file:")
			<< fn.GetFullPath() << wxT("\" >")
			<< fn.GetFullName()
			<< wxT("</a></font></td>")
			<< wxT("<td><font size=2 face=\"Verdana\">") << fn.GetFullPath() << wxT("</font></td>")
			<< wxT("</tr>");
		}
	}
	html << wxT("</table>");
	return html;
}

void clMainFrame::CreateRecentlyOpenedFilesMenu()
{
	wxArrayString files;
	FileHistory &hs = GetMainBook()->GetRecentlyOpenedFilesClass();
	GetMainBook()->GetRecentlyOpenedFiles(files);

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

void clMainFrame::CreateRecentlyOpenedWorkspacesMenu()
{
	wxArrayString files;
	FileHistory &hs = ManagerST::Get()->GetRecentlyOpenedWorkspacesClass();
	ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);

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
			// Clear any stale items
			wxMenuItemList items = submenu->GetMenuItems();
			wxMenuItemList::reverse_iterator lriter = items.rbegin();
			for (; lriter != items.rend(); ++lriter) {
				submenu->Delete(*lriter);
			}
			// Add entries without their .workspace extension
			hs.AddFilesToMenuWithoutExt();
		}
	}
}

void clMainFrame::OnRecentFile(wxCommandEvent &event)
{
	size_t idx = event.GetId() - (RecentFilesSubMenuID+1);
	FileHistory &fh = GetMainBook()->GetRecentlyOpenedFilesClass();

	wxArrayString files;
	fh.GetFiles(files);

	if (idx < files.GetCount()) {
		wxString projectName = ManagerST::Get()->GetProjectNameByFile(files.Item(idx));
		clMainFrame::Get()->GetMainBook()->OpenFile(files.Item(idx), projectName);
	}
}

void clMainFrame::OnRecentWorkspace(wxCommandEvent &event)
{
	size_t idx = event.GetId() - (RecentWorkspaceSubMenuID+1);
	FileHistory &fh = ManagerST::Get()->GetRecentlyOpenedWorkspacesClass();

	wxArrayString files;
	fh.GetFiles(files);

	if (idx < files.GetCount()) {
		wxString file_name (files.Item(idx));
		if ( file_name.EndsWith(wxT(".workspace")) == false ) {
			file_name << wxT(".workspace");
		}
		wxWindowUpdateLocker locker(this);
		ManagerST::Get()->OpenWorkspace( file_name );
	}
}

void clMainFrame::OnBackwardForward(wxCommandEvent &event)
{
	switch (event.GetId()) {
	case wxID_FORWARD:
		NavMgr::Get()->NavigateForward(PluginManager::Get());
		break;
	case wxID_BACKWARD:
		NavMgr::Get()->NavigateBackward(PluginManager::Get());
		break;
	}
}

void clMainFrame::OnBackwardForwardUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	if (event.GetId() == wxID_FORWARD) {
		event.Enable(NavMgr::Get()->CanNext());
	} else if (event.GetId() == wxID_BACKWARD) {
		event.Enable(NavMgr::Get()->CanPrev());
	} else {
		event.Skip();
	}
}

void clMainFrame::CreateWelcomePage()
{
	Manager *mgr = ManagerST::Get();
	//load the template
	wxFileName fn(mgr->GetStarupDirectory(), wxT("index.html"));
	wxFFile file(fn.GetFullPath(), wxT("r"));
	if (!file.IsOpened()) {
		return;
	}
	wxHtmlWindow *welcomePage = new wxHtmlWindow(GetMainBook(), wxID_ANY);

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

	active_caption = DrawingUtils::LightColour(active_caption, 11.0);

	content.Replace(wxT("$(ACTIVE_CAPTION)"), active_caption.GetAsString());
	content.Replace(wxT("$(ACTIVE_CAPTION_TEXT)"), active_caption_txt.GetAsString());

	welcomePage->SetPage(content);
	GetMainBook()->AddPage(welcomePage, wxT("Welcome!"), wxNullBitmap, true);
}

void clMainFrame::OnImportMSVS(wxCommandEvent &e)
{
	wxUnusedVar(e);
	const wxString ALL(wxT("MS Visual Studio Solution File (*.sln)|*.sln|")
	                   wxT("All Files (*)|*"));
	wxFileDialog dlg(this, wxT("Open MS Solution File"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
	if (dlg.ShowModal() == wxID_OK) {

		wxArrayString cmps;
		BuildSettingsConfigCookie cookie;
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
		while (cmp) {
			cmps.Add(cmp->GetName());
			cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
		}

		// Get the prefered compiler type
		wxString compilerName = wxGetSingleChoice(wxT("Select the compiler to use:"), wxT("Choose compiler"), cmps);
		ManagerST::Get()->ImportMSVSSolution(dlg.GetPath(), compilerName);
	}
}

void clMainFrame::OnDebug(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Manager *mgr = ManagerST::Get();

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		//debugger is already running -> probably a continue command
		mgr->DbgStart();
	} else if (mgr->IsWorkspaceOpen()) {

		if (WorkspaceST::Get()->GetActiveProjectName().IsEmpty()) {
			wxLogMessage(wxT("Attempting to debug workspace with no active project? Ignoring."));
			return;
		}

		// Debugger is not running, but workspace is opened -> start debug session
		long build_first(wxID_NO);
		bool answer(false);

		if (!EditorConfigST::Get()->GetLongValue(wxT("BuildBeforeDebug"), build_first)) {
			// value does not exist in the configuration file, prompt the user
			ThreeButtonDlg *dlg = new ThreeButtonDlg(this, wxT("Would you like to build the project before debugging it?"), wxT("CodeLite"));
			build_first = dlg->ShowModal();
			answer = dlg->GetDontAskMeAgain();
			dlg->Destroy();

			if (answer && build_first != wxID_CANCEL) {
				// save the answer
				EditorConfigST::Get()->SaveLongValue(wxT("BuildBeforeDebug"), build_first);
			}

		}

		// if build first is required, place a build command on the queue
		if (build_first == wxID_OK) {
			QueueCommand bldCmd(WorkspaceST::Get()->GetActiveProjectName(), wxEmptyString, false, QueueCommand::Build);

			// handle custom builds
			BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(WorkspaceST::Get()->GetActiveProjectName(), wxEmptyString);
			if (bldConf->IsCustomBuild()) {
				bldCmd.SetKind(QueueCommand::CustomBuild);
				bldCmd.SetCustomBuildTarget(wxT("Build"));
			}

			ManagerST::Get()->PushQueueCommand(bldCmd);
		}

		// place a debug command
		QueueCommand dbgCmd(QueueCommand::Debug);

		// make sure that build was success before proceeding (only when build_first flag is on)
		dbgCmd.SetCheckBuildSuccess(build_first == wxID_OK);
		ManagerST::Get()->PushQueueCommand(dbgCmd);

		// trigger the commands queue
		ManagerST::Get()->ProcessCommandQueue();
	}
}

void clMainFrame::OnDebugUI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
	e.Enable( !ManagerST::Get()->IsBuildInProgress() );
}

void clMainFrame::OnDebugRestart(wxCommandEvent &e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
		GetDebuggerPane()->GetLocalsTable()->Clear();
		dbgr->Restart();
	}
}

void clMainFrame::OnDebugRestartUI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract());
}

void clMainFrame::OnDebugStop(wxCommandEvent &e)
{
	wxUnusedVar(e);
	ManagerST::Get()->DbgStop();
}

void clMainFrame::OnDebugStopUI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && dbgr->IsRunning());
}

void clMainFrame::OnDebugManageBreakpointsUI(wxUpdateUIEvent &e)
{
	e.Enable(true);
}

void clMainFrame::OnDebugCmd(wxCommandEvent &e)
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
	} else if (e.GetId() == XRCID("show_cursor")) {
		cmd = DBG_SHOW_CURSOR;
	}

	if (cmd != wxNOT_FOUND) {
		ManagerST::Get()->DbgDoSimpleCommand(cmd);
	}
}

void clMainFrame::OnDebugCmdUI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
	if (e.GetId() == XRCID("pause_debugger") ||
	        e.GetId() == XRCID("dbg_stepin") ||
	        e.GetId() == XRCID("dbg_stepout") ||
	        e.GetId() == XRCID("dbg_next") ||
	        e.GetId() == XRCID("show_cursor")) {
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		e.Enable(dbgr && dbgr->IsRunning());
	}
}

void clMainFrame::OnDebuggerSettings(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DebuggerSettingsDlg *dlg = new DebuggerSettingsDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void clMainFrame::OnIdle(wxIdleEvent &e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		dbgr->Poke();
	}
}

void clMainFrame::OnLinkClicked(wxHtmlLinkEvent &e)
{
	wxString action = e.GetLinkInfo().GetHref();
	if (!action.StartsWith(wxT("action:"), &action)) {
		wxLaunchDefaultBrowser(e.GetLinkInfo().GetHref());
		return;
	}
	wxString command = action.BeforeFirst(wxT(':'));
	wxString filename = action.AfterFirst(wxT(':'));
	if (command != wxT("switch-workspace") &&
	        command != wxT("open-file") &&
	        command != wxT("create-workspace") &&
	        command != wxT("import-msvs-solution") &&
	        command != wxT("open-workspace")) {
		e.Skip();
		return;
	}

	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("link_action"));
	event.SetEventObject(this);
	StartPageData *data = new StartPageData;
	data->action = command;
	data->file_path = filename;
	if (wxFileName(filename).GetExt() == wxT("workspace")) {
		data->action = wxT("switch-workspace");
	}
	event.SetClientData(data);
	wxPostEvent(this, event);
}

void clMainFrame::OnStartPageEvent(wxCommandEvent& e)
{
	StartPageData *data = (StartPageData *)e.GetClientData();
	if ( data->action == wxT("switch-workspace" )) {
		wxWindowUpdateLocker locker(this);
		ManagerST::Get()->OpenWorkspace(data->file_path);

	} else if ( data->action == wxT("open-file" )) {
		clMainFrame::Get()->GetMainBook()->OpenFile(data->file_path, wxEmptyString);

	} else if ( data->action == wxT("create-workspace" )) {
		OnProjectNewWorkspace(e);

	} else if ( data->action == wxT("import-msvs-solution" )) {
		OnImportMSVS(e);

	} else if ( data->action == wxT("open-workspace" )) {
		OnSwitchWorkspace(e);

	}
	delete data;
}

void clMainFrame::OnLoadLastSessionUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	event.Check(m_frameGeneralInfo.GetFlags() & CL_LOAD_LAST_SESSION);
}

void clMainFrame::SetFrameFlag(bool set, int flag)
{
	if (set) {
		m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() | flag);
	} else {
		m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() & ~(flag));
	}
}

void clMainFrame::OnLoadLastSession(wxCommandEvent &event)
{
	SetFrameFlag(event.IsChecked(), CL_LOAD_LAST_SESSION);
}

void clMainFrame::OnShowWelcomePageUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	event.Enable(GetMainBook()->FindPage(wxT("Welcome!")) == NULL);
}

void clMainFrame::OnShowWelcomePage(wxCommandEvent &event)
{
	ShowWelcomePage();
}

void clMainFrame::CompleteInitialization()
{
	PluginManager::Get()->Load();

	// Connect some system events
	m_mgr.Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(clMainFrame::OnDockablePaneClosed), NULL, this);
	m_mgr.Connect(wxEVT_AUI_RENDER,     wxAuiManagerEventHandler(clMainFrame::OnAuiManagerRender),   NULL, this);

#ifdef __WXMAC__
	wxStaticLine *line = new wxStaticLine(this);
	GetSizer()->Add(line, 0, wxEXPAND);
#endif

	OutputViewControlBar* outputViewControlBar = new OutputViewControlBar(this, GetOutputPane()->GetNotebook(), &m_mgr, wxID_ANY);
	outputViewControlBar->AddAllButtons();


	GetSizer()->Add(outputViewControlBar, 0, wxEXPAND);
	Layout();
	SelectBestEnvSet();

	this->Thaw();
}

void clMainFrame::OnAppActivated(wxActivateEvent &e)
{
	if (m_theFrame && e.GetActive()) {
		m_theFrame->ReloadExternallyModifiedProjectFiles();
		m_theFrame->GetMainBook()->ReloadExternallyModified(true);
	} else if(m_theFrame) {
		LEditor *editor = GetMainBook()->GetActiveEditor();
		if(editor) {
			// we are loosing the focus
			editor->CallTipCancel();
			editor->HideCompletionBox();
		}
	}
	e.Skip();
}

void clMainFrame::OnCompileFile(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
		LEditor *editor = GetMainBook()->GetActiveEditor();
		if (editor && !editor->GetProject().IsEmpty()) {
			mgr->CompileFile(editor->GetProject(), editor->GetFileName().GetFullPath());
		}
	}
}

void clMainFrame::OnCompileFileUI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
	e.Enable(false);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
		LEditor *editor = GetMainBook()->GetActiveEditor();
		if (editor && !editor->GetProject().IsEmpty()) {
			e.Enable(true);
		}
	}
}

void clMainFrame::OnDebugAttach(wxCommandEvent &event)
{
	wxUnusedVar(event);
	//Start the debugger
	Manager *mgr = ManagerST::Get();
	mgr->DbgStart(1);
}

void clMainFrame::OnCloseAllButThis(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxWindow *win = GetMainBook()->GetCurrentPage();
	if (win != NULL) {
		GetMainBook()->CloseAllButThis(win);
	}
}

WorkspaceTab *clMainFrame::GetWorkspaceTab()
{
	return GetWorkspacePane()->GetWorkspaceTab();
}

FileExplorer *clMainFrame::GetFileExplorer()
{
	return GetWorkspacePane()->GetFileExplorer();
}

void clMainFrame::OnLoadWelcomePage(wxCommandEvent &event)
{
	SetFrameFlag(event.IsChecked(), CL_SHOW_WELCOME_PAGE);
}

void clMainFrame::OnLoadWelcomePageUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	event.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_WELCOME_PAGE ? true : false);
}

void clMainFrame::OnFileCloseUI(wxUpdateUIEvent &event)
{
	CHECK_SHUTDOWN();
	event.Enable(GetMainBook()->GetCurrentPage() != NULL);
}

void clMainFrame::OnConvertEol(wxCommandEvent &e)
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		int eol(wxSCI_EOL_LF);
		if (e.GetId() == XRCID("convert_eol_win")) {
			eol = wxSCI_EOL_CRLF;
		} else if (e.GetId() == XRCID("convert_eol_mac")) {
			eol = wxSCI_EOL_CR;
		}
		editor->ConvertEOLs(eol);
		editor->SetEOLMode(eol);
	}
}

void clMainFrame::OnViewDisplayEOL(wxCommandEvent &e)
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
	GetMainBook()->SetViewEOL(visible);
}

void clMainFrame::OnViewDisplayEOL_UI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
	LEditor *editor = GetMainBook()->GetActiveEditor();
	bool hasEditor = editor ? true : false;
	if (!hasEditor) {
		e.Enable(false);
		return;
	}

	e.Enable(true);
	e.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_EOL ? true : false);
}

void clMainFrame::OnCopyFileName(wxCommandEvent& event)
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		wxString fileName = editor->GetFileName().GetFullName();
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

void clMainFrame::OnCopyFilePath(wxCommandEvent &event)
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
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
void clMainFrame::OnCopyFilePathOnly(wxCommandEvent &event)
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
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

void clMainFrame::OnWorkspaceMenuUI(wxUpdateUIEvent &e)
{
	CHECK_SHUTDOWN();
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

void clMainFrame::OnManagePlugins(wxCommandEvent &e)
{
	PluginMgrDlg dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
		DoSuggestRestart();
	}
}

void clMainFrame::OnCppContextMenu(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->GetContext()->ProcessEvent(e);
	}
}

void clMainFrame::OnConfigureAccelerators(wxCommandEvent &e)
{
	AccelTableDlg dlg(this);
	dlg.ShowModal();
}

void clMainFrame::OnUpdateBuildRefactorIndexBar(wxCommandEvent& e)
{
	wxUnusedVar(e);
}

void clMainFrame::OnHighlightWord(wxCommandEvent& event)
{
	long highlightWord(1);

	EditorConfigST::Get()->GetLongValue(wxT("highlight_word"), highlightWord);

	if ( !highlightWord ) {
		GetMainBook()->HighlightWord(true);
		EditorConfigST::Get()->SaveLongValue(wxT("highlight_word"), 1);
	} else {
		GetMainBook()->HighlightWord(false);
		EditorConfigST::Get()->SaveLongValue(wxT("highlight_word"), 0);
	}
}

void clMainFrame::OnShowNavBar(wxCommandEvent& e)
{
	GetMainBook()->ShowNavBar(e.IsChecked());
}

void clMainFrame::OnShowNavBarUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	e.Check(GetMainBook()->IsNavBarShown());
}

void clMainFrame::OnParsingThreadDone(wxCommandEvent& e)
{
	if ( ManagerST::Get()->IsShutdownInProgress() ) {
		// we are in shutdown progress, dont do anything
		return;
	}

	wxUnusedVar(e);
	SetStatusMessage(wxEmptyString, 0, XRCID("retag_file"));
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->UpdateColours();
	}
}

void clMainFrame::OnSingleInstanceOpenFiles(wxCommandEvent& e)
{
	wxArrayString *arr = reinterpret_cast<wxArrayString*>(e.GetClientData());
	if (arr) {
		for (size_t i=0; i<arr->GetCount(); i++) {
			wxFileName fn(arr->Item(i));

			// if file is workspace, load it
			if (fn.GetExt() == wxT("workspace")) {
				if ( ManagerST::Get()->IsWorkspaceOpen() ) {
					if (wxMessageBox(_("Close this workspace, and load workspace '") + fn.GetFullName() + wxT("'"), wxT("CodeLite"), wxICON_QUESTION|wxYES_NO, this) == wxNO) {
						continue;
					}
				}
				ManagerST::Get()->OpenWorkspace(arr->Item(i));
			} else {
				clMainFrame::Get()->GetMainBook()->OpenFile(arr->Item(i), wxEmptyString);
			}
		}
		delete arr;
	}
	Raise();
}

void clMainFrame::OnSingleInstanceRaise(wxCommandEvent& e)
{
	wxUnusedVar(e);
	Raise();
}

void clMainFrame::OnNewVersionAvailable(wxCommandEvent& e)
{
	WebUpdateJobData *data = reinterpret_cast<WebUpdateJobData*>(e.GetClientData());
	if (data) {
		if (data->IsUpToDate() == false) {

			m_codeliteDownloadPageURL = data->GetUrl();
			ButtonDetails btn;
			btn.buttonLabel = wxT("Download Now!");
			btn.commandId   = XRCID("goto_codelite_download_url");
			btn.isDefault   = true;
			btn.window      = this;

			GetMainBook()->ShowMessage(wxT("A new version of CodeLite is available"), true, PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/software_upgrade")), btn);

		} else {
			if (!data->GetShowMessage()) {
				wxLogMessage(wxString::Format(wxT("Info: CodeLite is up-to-date (or newer), version used: %d, version on site:%d"), (int)data->GetCurrentVersion(), (int)data->GetNewVersion()));

			} else {
				// User initiated the version check request
				GetMainBook()->ShowMessage(wxT("CodeLite is up-to-date"));
			}
		}
		delete data;
	}
}

void clMainFrame::OnDetachWorkspaceViewTab(wxCommandEvent& e)
{
	size_t     sel = GetWorkspacePane()->GetNotebook()->GetSelection();
	wxWindow *page = GetWorkspacePane()->GetNotebook()->GetCurrentPage();
	wxString  text = GetWorkspacePane()->GetNotebook()->GetPageText(sel);
	wxBitmap  bmp;

	DockablePane *pane = new DockablePane(m_mainPanel, GetWorkspacePane()->GetNotebook(), text, bmp, wxSize(200, 200));
	page->Reparent(pane);

	// remove the page from the notebook
	GetWorkspacePane()->GetNotebook()->RemovePage(sel, false);
	pane->SetChildNoReparent(page);

	wxUnusedVar(e);
}

void clMainFrame::OnNewDetachedPane(wxCommandEvent &e)
{
	DockablePane *pane = (DockablePane*)(e.GetClientData());
	if (pane) {
		wxString text = pane->GetName();
		m_DPmenuMgr->AddMenu(text);

		m_mgr.AddPane(pane, wxAuiPaneInfo().Name(text).Caption(text));
		m_mgr.Update();
	}
}

void clMainFrame::OnDestroyDetachedPane(wxCommandEvent& e)
{
	DockablePane *pane = (DockablePane*)(e.GetClientData());
	if (pane) {
		m_mgr.DetachPane(pane);

		// remove any menu entries for this pane
		m_DPmenuMgr->RemoveMenu(pane->GetName());
		pane->Destroy();
		m_mgr.Update();
	}
}

void clMainFrame::OnAuiManagerRender(wxAuiManagerEvent &e)
{
	wxAuiManager *mgr = e.GetManager();
	wxAuiPaneInfoArray &panes = mgr->GetAllPanes();

	wxAcceleratorTable *accelTable = GetAcceleratorTable();
	if (accelTable != NULL) {
		for (size_t i = 0; i < panes.GetCount(); i++) {
			if (panes[i].frame != NULL) {
				panes[i].frame->SetAcceleratorTable(*accelTable);
			}
		}
	}
	e.Skip();
}

void clMainFrame::OnDockablePaneClosed(wxAuiManagerEvent &e)
{
	DockablePane *pane = dynamic_cast<DockablePane*>(e.GetPane()->window);
	if (pane) {
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
		pane->GetEventHandler()->ProcessEvent(evt);
	} else {
		e.Skip();
	}
}

void clMainFrame::SetStatusMessage(const wxString &msg, int col, int id)
{
	wxUnusedVar(id);
	SetStatusText(msg, col);
}

void clMainFrame::OnFunctionCalltipUI(wxUpdateUIEvent& event)
{
	CHECK_SHUTDOWN();
	LEditor* editor = GetMainBook()->GetActiveEditor();
	event.Enable(editor ? true : false);
}

void clMainFrame::OnReloadWorkspace(wxCommandEvent& event)
{
	wxUnusedVar(event);

	// Save the current session before re-loading
	SaveLayoutAndSession();

	ManagerST::Get()->ReloadWorkspace();
}

void clMainFrame::OnReloadWorkspaceUI(wxUpdateUIEvent& event)
{
	CHECK_SHUTDOWN();
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void clMainFrame::RebuildProject(const wxString& projectName)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {
		wxString conf;
		// get the selected configuration to be built
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
		if (bldConf) {
			conf = bldConf->GetName();
		}

		// first we place a clean command
		QueueCommand buildInfo(projectName, conf, false, QueueCommand::Clean);
		if (bldConf && bldConf->IsCustomBuild()) {
			buildInfo.SetKind(QueueCommand::CustomBuild);
			buildInfo.SetCustomBuildTarget(wxT("Clean"));
		}
		ManagerST::Get()->PushQueueCommand(buildInfo);

		// now we place a build command
		buildInfo = QueueCommand(projectName, conf, false, QueueCommand::Build);

		if (bldConf && bldConf->IsCustomBuild()) {
			buildInfo.SetKind(QueueCommand::CustomBuild);
			buildInfo.SetCustomBuildTarget(wxT("Build"));
		}
		ManagerST::Get()->PushQueueCommand(buildInfo);

		// process the queue
		ManagerST::Get()->ProcessCommandQueue();
	}
}

void clMainFrame::OnBatchBuildUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	bool enable = !ManagerST::Get()->IsBuildInProgress() && ManagerST::Get()->IsWorkspaceOpen();
	e.Enable(enable);
}

void clMainFrame::OnBatchBuild(wxCommandEvent& e)
{
	BatchBuildDlg *batchBuild = new BatchBuildDlg(this);
	if (batchBuild->ShowModal() == wxID_OK) {
		// build the projects
		std::list<QueueCommand> buildInfoList;
		batchBuild->GetBuildInfoList(buildInfoList);
		if (buildInfoList.empty() == false) {
			std::list<QueueCommand>::iterator iter = buildInfoList.begin();

			// add all build items to queue
			for (; iter != buildInfoList.end(); iter ++) {
				ManagerST::Get()->PushQueueCommand(*iter);
			}
		}
	}
	batchBuild->Destroy();

	// start the build process
	ManagerST::Get()->ProcessCommandQueue();
}

void clMainFrame::SetFrameTitle(LEditor* editor)
{
	wxString title;
	if (editor && editor->GetModify()) {
		title << wxT("*");
	}

	//LEditor *activeEditor = GetMainBook()->GetActiveEditor();
	if (editor/* && activeEditor == editor*/) {
		title << editor->GetFileName().GetFullName() << wxT(" ");
		// by default display the full path as well
		long value(1);
		EditorConfigST::Get()->GetLongValue(wxT("ShowFullPathInFrameTitle"), value);
		if (value) {
			title << wxT("[") << editor->GetFileName().GetFullPath() << wxT("] ");
		}
		title << wxT("- ");
	}

	title << wxT("CodeLite - Revision: ");
	title << SvnRevision;
	SetTitle(title);
}

void clMainFrame::OnBuildWorkspace(wxCommandEvent& e)
{
	// start the build process
	wxUnusedVar(e);
	ManagerST::Get()->BuildWorkspace();
}

void clMainFrame::OnBuildWorkspaceUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
}

void clMainFrame::OnDetachDebuggerViewTab(wxCommandEvent& e)
{
	size_t     sel = GetDebuggerPane()->GetNotebook()->GetSelection();
	wxWindow *page = GetDebuggerPane()->GetNotebook()->GetCurrentPage();
	wxString  text = GetDebuggerPane()->GetNotebook()->GetPageText(sel);
	wxBitmap  bmp ;

	DockablePane *pane = new DockablePane(m_mainPanel, GetDebuggerPane()->GetNotebook(), text, bmp, wxSize(200, 200));
	page->Reparent(pane);

	// remove the page from the notebook
	GetDebuggerPane()->GetNotebook()->RemovePage(sel, false);
	pane->SetChildNoReparent(page);

	wxUnusedVar(e);
}

void clMainFrame::OnCleanWorkspace(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ManagerST::Get()->CleanWorkspace();
}

void clMainFrame::OnCleanWorkspaceUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
}

void clMainFrame::OnReBuildWorkspace(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ManagerST::Get()->RebuildWorkspace();
}

void clMainFrame::OnReBuildWorkspaceUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
}

void clMainFrame::OnOpenShellFromFilePath(wxCommandEvent& e)
{
	// get the file path
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		wxString filepath = editor->GetFileName().GetPath();
		DirSaver ds;
		wxSetWorkingDirectory(filepath);

		// Apply the environment variabels before opening the shell
		EnvSetter setter;
		if (!ProcUtils::Shell()) {
			wxLogMessage(wxString::Format(wxT("Failed to open shell at '%s'"), filepath.c_str()));
		}
	}
}

void clMainFrame::ShowWelcomePage()
{
	wxWindow *win = GetMainBook()->FindPage(wxT("Welcome!"));
	if (win) {
		GetMainBook()->SelectPage(win);
	} else {
		CreateWelcomePage();
	}
}

void clMainFrame::OnSyntaxHighlight(wxCommandEvent& e)
{
	SyntaxHighlightDlg *dlg = new SyntaxHighlightDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void clMainFrame::OnQuickDebug(wxCommandEvent& e)
{
	// launch the debugger
	QuickDebugDlg *dlg = new QuickDebugDlg(this);
	if (dlg->ShowModal() == wxID_OK) {

		DebuggerMgr::Get().SetActiveDebugger(dlg->GetDebuggerName());
		IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();

		if (dbgr && !dbgr->IsRunning()) {

			std::vector<BreakpointInfo> bpList;
			wxString exepath = dlg->GetExe();
			wxString wd = dlg->GetWorkingDirectory();
			wxArrayString cmds = dlg->GetStartupCmds();

			// update the debugger information
			DebuggerInformation dinfo;
			DebuggerMgr::Get().GetDebuggerInformation(dlg->GetDebuggerName(), dinfo);
			dinfo.breakAtWinMain = true;

			// read the console command
			dinfo.consoleCommand = EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand();

			// ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints(); TODO: Reimplement this when UpdateBreakpoints() updates only alterations, rather than delete/re-enter

			wxString dbgname = dinfo.path;
			dbgname = EnvironmentConfig::Instance()->ExpandVariables(dbgname, true);

			// launch the debugger
			dbgr->SetObserver(ManagerST::Get());
			dbgr->SetDebuggerInformation(dinfo);

			//TODO: Reimplement this when UpdateBreakpoints() updates only alterations, rather than delete/re-enter
			//GetMainBook()->UpdateBreakpoints();

			// get an updated list of breakpoints
			ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(bpList);

			DebuggerStartupInfo startup_info;
			startup_info.debugger = dbgr;

			// notify plugins that we're about to start debugging
			if (SendCmdEvent(wxEVT_DEBUG_STARTING, &startup_info))
				// plugin stopped debugging
				return;

			wxString tty;
#ifndef __WXMSW__
			wxString title;
			title << wxT("Debugging: ") << exepath << wxT(" ") << dlg->GetArguments();
			tty = StartTTY(title);
			if(tty.IsEmpty()) {
				wxMessageBox(wxT("Could not start TTY console for debugger!"), wxT("codelite"), wxOK|wxCENTER|wxICON_ERROR);
			}
#endif

			dbgr->SetIsRemoteDebugging(false);
			dbgr->Start(dbgname, exepath, wd, bpList, cmds, tty);

			// notify plugins that the debugger just started
			SendCmdEvent(wxEVT_DEBUG_STARTED, &startup_info);

			dbgr->Run(dlg->GetArguments(), wxEmptyString);

			// Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
			// so that it uses debugger_ids instead of internal_ids
			clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

			// and finally make sure that the debugger pane is visible
			wxAuiPaneInfo &info = GetDockingManager().GetPane ( wxT ( "Debugger" ) );
			if ( info.IsOk() && !info.IsShown() ) {
				ManagerST::Get()->SetDebuggerPaneOriginallyVisible(false);
			ManagerST::Get()->ShowDebuggerPane();
		}
	}
	}
	dlg->Destroy();
}

void clMainFrame::OnQuickDebugUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && !dbgr->IsRunning());
}

void clMainFrame::OnShowWhitespaceUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	if (e.GetId() == XRCID("whitepsace_invisible")) {
		e.Check(options->GetShowWhitspaces() == 0);
	} else if (e.GetId() == XRCID("whitepsace_always")) {
		e.Check(options->GetShowWhitspaces() == 1);
	} else if (e.GetId() == XRCID("whitespace_visiable_after_indent")) {
		e.Check(options->GetShowWhitspaces() == 2);
	} else if (e.GetId() == XRCID("whitespace_indent_only")) {
		e.Check(options->GetShowWhitspaces() == 3);
	}
}

void clMainFrame::OnShowWhitespace(wxCommandEvent& e)
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	if (e.GetId() == XRCID("whitepsace_invisible")) {
		options->SetShowWhitspaces(0);
	} else if (e.GetId() == XRCID("whitepsace_always")) {
		options->SetShowWhitspaces(1);
	} else if (e.GetId() == XRCID("whitespace_visiable_after_indent")) {
		options->SetShowWhitspaces(2);
	} else if (e.GetId() == XRCID("whitespace_indent_only")) {
		options->SetShowWhitspaces(3);
	}

	GetMainBook()->ShowWhitespace(options->GetShowWhitspaces());

	// save the settings
	EditorConfigST::Get()->SetOptions(options);
}

void clMainFrame::OnIncrementalSearch(wxCommandEvent& event)
{
	wxUnusedVar( event );
	GetMainBook()->ShowQuickBar(true);
}

void clMainFrame::OnRetagWorkspace(wxCommandEvent& event)
{
	wxUnusedVar( event );
	ManagerST::Get()->RetagWorkspace(event.GetId() == XRCID("retag_workspace") ? true : false );
}

void clMainFrame::OnShowFullScreen(wxCommandEvent& e)
{
	wxUnusedVar(e);

	if (IsFullScreen()) {
		ShowFullScreen(false);

	} else {

		ShowFullScreen(true, wxFULLSCREEN_NOCAPTION|wxFULLSCREEN_NOBORDER);

		// Re-apply the menu accelerators
		ManagerST::Get()->UpdateMenuAccelerators();
	}
}

void clMainFrame::OnSetStatusMessage(wxCommandEvent& e)
{
	wxString msg = e.GetString();
	int col = e.GetInt();
	SetStatusMessage(msg, col);
}

void clMainFrame::OnReloadExternallModified(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ReloadExternallyModifiedProjectFiles();
	GetMainBook()->ReloadExternallyModified(true);
}

void clMainFrame::OnReloadExternallModifiedNoPrompt(wxCommandEvent& e)
{
	wxUnusedVar(e);
	GetMainBook()->ReloadExternallyModified(false);
}

void clMainFrame::ReloadExternallyModifiedProjectFiles()
{
	if ( ManagerST::Get()->IsWorkspaceOpen() == false ) {
		return;
	}

	Workspace *workspace = WorkspaceST::Get();
	bool workspace_modified = false, project_modified = false;

	// check if the workspace needs reloading and ask the user for confirmation
	// if it does
	if (workspace->GetWorkspaceLastModifiedTime() < workspace->GetFileLastModifiedTime()) {
		// always update last modification time: if the user chooses to reload it
		// will not matter, and it avoids the program prompting the user repeatedly
		// if he chooses not to reload the workspace
		workspace->SetWorkspaceLastModifiedTime(workspace->GetFileLastModifiedTime());
		workspace_modified = true;
	}

	// check if any of the projects in the workspace needs reloading
	wxArrayString projects;
	workspace->GetProjectList(projects);

	for (size_t i = 0; i < projects.GetCount(); ++i) {
		wxString errStr;
		ProjectPtr proj = workspace->FindProjectByName(projects[i], errStr);

		if (proj->GetProjectLastModifiedTime() < proj->GetFileLastModifiedTime()) {
			// always update last modification time: if the user chooses to reload it
			// will not matter, and it avoids the program prompting the user repeatedly
			// if he chooses not to reload some of the projects
			proj->SetProjectLastModifiedTime(proj->GetFileLastModifiedTime());
			project_modified = true;
		}
	}

	if (!project_modified && !workspace_modified)
		return;

	// See if there's a saved 'Always do this' preference re reloading
	long pref;
	if (EditorConfigST::Get()->GetLongValue(wxT("ReloadWorkspaceWhenAltered"), pref)) {
		if (pref == 1) { // 1 means never reload
			return;
		} else if (pref == 2) { // 2 means always reload
			wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
			GetEventHandler()->AddPendingEvent(evt); // Lands in OnReloadWorkspace()
			return;
		}
	}

	// No preference (or it's 'Always ask') so ask
	ButtonDetails btn, noBtn;
	btn.buttonLabel = wxT("Reload Workspace");
	btn.commandId   = XRCID("reload_workspace");
	btn.isDefault   = false;
	btn.window      = this;

	noBtn.buttonLabel = wxT("&Don't reload");
	noBtn.isDefault   = true;
	noBtn.window      = NULL;

	CheckboxDetails cb(wxT("ReloadWorkspaceWhenAltered"));

	GetMainBook()->ShowMessage(_("Workspace or project settings have been modified, would you like to reload the workspace and all contained projects?"), false, PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/reload_workspace")), noBtn, btn, ButtonDetails(), cb);
}

void clMainFrame::SaveLayoutAndSession()
{
	// Save the perspective
	wxFileName userFileName   (wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config") + wxFileName::GetPathSeparator() + wxT("codelite.layout"));
	WriteFileUTF8(userFileName.GetFullPath(), m_mgr.SavePerspective());

	EditorConfigST::Get()->SaveLexers();

	//save general information
	if (IsMaximized()) {
		m_frameGeneralInfo.SetFrameSize(wxSize(800, 600));
	} else {
		m_frameGeneralInfo.SetFrameSize(this->GetSize());
	}
	m_frameGeneralInfo.SetFramePosition(this->GetScreenPosition());

	EditorConfigST::Get()->Begin();

	SetFrameFlag(IsMaximized(), CL_MAXIMIZE_FRAME);
	EditorConfigST::Get()->WriteObject(wxT("GeneralInfo"), &m_frameGeneralInfo);
	EditorConfigST::Get()->SaveLongValue(wxT("ShowNavBar"), m_mainBook->IsNavBarShown() ? 1 : 0);

	//save the 'find and replace' information
	GetOutputPane()->GetFindResultsTab()->SaveFindInFilesData();
	if (LEditor::GetFindReplaceDialog()) {
		EditorConfigST::Get()->WriteObject(wxT("FindAndReplaceData"), &(LEditor::GetFindReplaceDialog()->GetData()));
	}

	//save the current session before closing
	wxString sessionName = ManagerST::Get()->IsWorkspaceOpen() ? WorkspaceST::Get()->GetWorkspaceFileName().GetFullPath()
	                       : wxString(wxT("Default"));
	SessionEntry session;
	session.SetWorkspaceName(sessionName);
	wxArrayInt unused;
	GetMainBook()->SaveSession(session, unused);
	ManagerST::Get()->GetBreakpointsMgr()->SaveSession(session);
	SessionManager::Get().Save(sessionName, session);
	SessionManager::Get().SetLastWorkspaceName(sessionName);

	// make sure there are no 'unsaved documents'
	GetMainBook()->CloseAll(false);

	// keep list of all detached panes
	wxArrayString panes = m_DPmenuMgr->GetDeatchedPanesList();
	DetachedPanesInfo dpi(panes);
	EditorConfigST::Get()->WriteObject(wxT("DetachedPanesList"), &dpi);

	// save the notebooks styles
	EditorConfigST::Get()->SaveLongValue(wxT("MainBook"),      GetMainBook()->GetBookStyle());
	EditorConfigST::Get()->SaveLongValue(wxT("FindResults"),   GetOutputPane()->GetFindResultsTab()->GetBookStyle());
	EditorConfigST::Get()->Save();
}

void clMainFrame::OnNextFiFMatch(wxCommandEvent& e)
{
	wxUnusedVar(e);
	GetOutputPane()->GetFindResultsTab()->NextMatch();
}

void clMainFrame::OnPreviousFiFMatch(wxCommandEvent& e)
{
	wxUnusedVar(e);
	GetOutputPane()->GetFindResultsTab()->PrevMatch();
}

void clMainFrame::OnNextFiFMatchUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	e.Enable(GetOutputPane()->GetFindResultsTab()->GetPageCount() > 0);
}

void clMainFrame::OnPreviousFiFMatchUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	e.Enable(GetOutputPane()->GetFindResultsTab()->GetPageCount() > 0);
}

void clMainFrame::OnFindResourceXXX(wxCommandEvent& e)
{
	// Determine the search type
	wxString searchType;
	if (e.GetId() == XRCID("find_function")) {
		searchType = OpenResourceDialog::TYPE_FUNCTION;

	} else if (e.GetId() == XRCID("find_macro")) {
		searchType = OpenResourceDialog::TYPE_MACRO;

	} else if (e.GetId() == XRCID("find_typedef")) {
		searchType = OpenResourceDialog::TYPE_TYPEDEF;

	} else if (e.GetId() == XRCID("find_type")) {
		searchType = OpenResourceDialog::TYPE_CLASS;
	} else {
		searchType = OpenResourceDialog::TYPE_WORKSPACE_FILE;
	}

	OpenResourceDialog dlg(this, PluginManager::Get(), searchType);
	if (dlg.ShowModal() == wxID_OK) {
		OpenResourceDialog::OpenSelection(dlg.GetSelection(), PluginManager::Get());
	}
}

void clMainFrame::OnParsingThreadMessage(wxCommandEvent& e)
{
	wxString *msg = (wxString*) e.GetClientData();
	if ( msg ) {
		wxLogMessage( *msg );
		delete msg;
	}
}

// Due to differnet schema versions, the database was truncated by the
// TagsManager, prompt the user
void clMainFrame::OnDatabaseUpgrade(wxCommandEvent& e)
{
	wxUnusedVar(e);
	// turn ON the retag-required flag and perform it in the OnTimer() callback
	// this is to allow codelite to update paths if needed *before* the retag is taking place
	// (CC paths will be updated if needed in the OnTimer() callback)
	m_workspaceRetagIsRequired = true;
}

void clMainFrame::UpdateTagsOptions(const TagsOptionsData& tod)
{
	m_tagsOptionsData = tod;
	TagsManagerST::Get()->SetCtagsOptions( m_tagsOptionsData );
	EditorConfigST::Get()->WriteObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);
	ParseThreadST::Get()->SetSearchPaths( tod.GetParserSearchPaths(), tod.GetParserExcludePaths() );
}

void clMainFrame::OnCheckForUpdate(wxCommandEvent& e)
{
	JobQueueSingleton::Instance()->PushJob( new WebUpdateJob(this, true) );
}

void clMainFrame::OnShowActiveProjectSettings(wxCommandEvent& e)
{
	GetWorkspaceTab()->GetEventHandler()->ProcessEvent( e );
}

void clMainFrame::OnShowActiveProjectSettingsUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	wxArrayString projectList;
	WorkspaceST::Get()->GetProjectList( projectList );
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && (projectList.IsEmpty() == false));
}


void clMainFrame::StartTimer()
{
	m_timer->Start(1000, true);
}

void clMainFrame::OnLoadPerspective(wxCommandEvent& e)
{
	long loadIt(1);
	EditorConfigST::Get()->GetLongValue(wxT("LoadSavedPrespective"), loadIt);
	if (loadIt) {

		// locate the layout file
		wxFileName defaultfileName(ManagerST::Get()->GetInstallDir() + wxFileName::GetPathSeparator() + wxT("config") + wxFileName::GetPathSeparator() + wxT("codelite.layout"));
		wxFileName userFileName   (wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config") + wxFileName::GetPathSeparator() + wxT("codelite.layout"));
		wxString pers(wxEmptyString);

		if (userFileName.FileExists()) {
			//load this file
			ReadFileWithConversion(userFileName.GetFullPath(), pers);

		} else if(defaultfileName.FileExists()) {
			ReadFileWithConversion(defaultfileName.GetFullPath(), pers);

		}

		//wxWindowUpdateLocker locker(this);
		if ( pers.IsEmpty() == false && EditorConfigST::Get()->GetRevision() == SvnRevision) {
			m_mgr.LoadPerspective(pers);

		} else {
			EditorConfigST::Get()->SetRevision(SvnRevision);
			EditorConfigST::Get()->SetInstallDir(ManagerST::Get()->GetInstallDir());
		}

		UpdateAUI();
	}
	EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);
}

void clMainFrame::SelectBestEnvSet()
{
	///////////////////////////////////////////////////
	// Select the environment variables set to use
	///////////////////////////////////////////////////

	// Set the workspace's environment variable set to the active one
	wxString   projectSetName;
	wxString   projectDbgSetName;

	// First, if the project has an environment which is not '<Use Defaults>' use it
	if(ManagerST::Get()->IsWorkspaceOpen()) {
		wxString activeProj = WorkspaceST::Get()->GetActiveProjectName();
		ProjectPtr p = ManagerST::Get()->GetProject( activeProj );
		if(p) {
			BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf(activeProj, wxEmptyString);
			if(buildConf) {
				if( buildConf->GetEnvVarSet() != USE_WORKSPACE_ENV_VAR_SET &&
					buildConf->GetEnvVarSet() != wxT("<Use Workspace Settings>") /* backward support */) {
					projectSetName    = buildConf->GetEnvVarSet();
				}

				if( buildConf->GetDbgEnvSet() != USE_GLOBAL_SETTINGS) {
					projectDbgSetName = buildConf->GetDbgEnvSet();
				}
			}
		}
	}


	wxString   workspaceSetName = LocalWorkspaceST::Get()->GetActiveEnvironmentSet();
	wxString   globalActiveSet  = wxT("Default");
	wxString   activeSetName;
	EvnVarList vars             = EnvironmentConfig::Instance()->GetSettings();

	// By default, use the global one
	activeSetName = globalActiveSet;

	if(!projectSetName.IsEmpty() && vars.IsSetExist(projectSetName)) {
		activeSetName = projectSetName;

	} else if (!workspaceSetName.IsEmpty() && vars.IsSetExist(workspaceSetName)) {
		activeSetName = workspaceSetName;
	}

	vars.SetActiveSet(activeSetName);
	EnvironmentConfig::Instance()->SetSettings(vars);

	///////////////////////////////////////////////////
	// Select the debugger PreDefined Types settings
	///////////////////////////////////////////////////
	DebuggerSettingsPreDefMap preDefTypeMap;
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &preDefTypeMap);

	wxString dbgSetName = wxT("Default");
	if(!projectDbgSetName.IsEmpty() && preDefTypeMap.IsSetExist(projectDbgSetName)) {
		dbgSetName = projectDbgSetName;
	}

	preDefTypeMap.SetActive(dbgSetName);
	DebuggerConfigTool::Get()->WriteObject(wxT("DebuggerCommands"), &preDefTypeMap);

	SetStatusMessage(wxString::Format(wxT("Env: %s, Dbg: %s"),
					 activeSetName.c_str(),
					 preDefTypeMap.GetActiveSet().GetName().c_str()), 2);
}

void clMainFrame::OnClearTagsCache(wxCommandEvent& e)
{
	e.Skip();
	TagsManagerST::Get()->ClearTagsCache();
	SetStatusMessage(wxT("Tags cache cleared"), 0);
}

void clMainFrame::OnUpdateNumberOfBuildProcesses(wxCommandEvent& e)
{
	int cpus = wxThread::GetCPUCount();
	BuilderConfigPtr bs = BuildSettingsConfigST::Get()->GetBuilderConfig(wxT("GNU makefile for g++/gcc"));
	if (bs && cpus != wxNOT_FOUND) {
		wxString jobs;
		jobs << cpus;

		bs->SetToolJobs( jobs );
		BuildSettingsConfigST::Get()->SetBuildSystem(bs);
		wxLogMessage(wxT("Info: setting number of concurrent builder jobs to ") + jobs);
	}
}

void clMainFrame::OnWorkspaceEditorPreferences(wxCommandEvent& e)
{
	GetWorkspaceTab()->GetFileView()->GetEventHandler()->ProcessEvent(e);
}

void clMainFrame::OnWorkspaceSettings(wxCommandEvent& e)
{
	GetWorkspaceTab()->GetFileView()->GetEventHandler()->ProcessEvent(e);
}

void clMainFrame::OnGotoCodeLiteDownloadPage(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxLaunchDefaultBrowser(m_codeliteDownloadPageURL);
	m_codeliteDownloadPageURL.Clear();
}

void clMainFrame::OnUpdateParserPath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxArrayString paths;
	wxArrayString excudePaths;
	IncludePathLocator locator(PluginManager::Get());
	locator.Locate( paths, excudePaths );

	m_tagsOptionsData.SetParserSearchPaths ( paths       );
	m_tagsOptionsData.SetParserExcludePaths( excudePaths );

	// Update the parser thread
	ParseThreadST::Get()->SetSearchPaths( paths, excudePaths );
	EditorConfigST::Get()->WriteObject( wxT("m_tagsOptionsData"), &m_tagsOptionsData );
}

void clMainFrame::OnNeverUpdateParserPath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	EditorConfigST::Get()->SaveLongValue(wxT("UpdateParserPaths"), 0);
}

void clMainFrame::DoSuggestRestart()
{
#ifdef __WXMAC__
	GetMainBook()->ShowMessage(_("Some of the changes made requires restart of CodeLite"));
#else
	// On Winodws & GTK we offer auto-restart
	ButtonDetails btn1, btn2;
	btn1.buttonLabel = wxT("Restart Now!");
	btn1.commandId   = wxEVT_CMD_RESTART_CODELITE;
	btn1.menuCommand = false;
	btn1.isDefault   = true;
	btn1.window      = ManagerST::Get();

	// set button window to NULL
	btn2.buttonLabel = wxT("Not now");
	btn2.window      = NULL;

	GetMainBook()->ShowMessage(_("Some of the changes made requires a restart of CodeLite, Restart now?"), false,
								PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/restart")), btn1, btn2);
#endif
}

void clMainFrame::OnRestoreDefaultLayout(wxCommandEvent& e)
{
	e.Skip();

#ifndef __WXMAC__
	wxWindowUpdateLocker locker(this);
#endif

	wxLogMessage(wxT("Restoring layout"));

	// Close all docking panes
	wxAuiPaneInfoArray &panes = m_mgr.GetAllPanes();

	for (size_t i = 0; i < panes.GetCount(); i++) {
		wxAuiPaneInfo p = panes[i];

		if (p.window) {
			DockablePane *d = dynamic_cast<DockablePane*>(p.window);
			if(d) {

				wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
				p.window->GetEventHandler()->AddPendingEvent(evt);

			}
		}

	}

	// Delete the debugger layout
	wxString debugPrespective(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/debug.layout"));
	wxFileName fn(debugPrespective);
	if(fn.FileExists()) {
		wxRemoveFile(debugPrespective);
	}

	m_mgr.LoadPerspective(m_defaultLayout, false);
	UpdateAUI();
}

void clMainFrame::SetAUIManagerFlags()
{
	// Set the manager flags
	unsigned int auiMgrFlags = wxAUI_MGR_ALLOW_ACTIVE_PANE |
							   wxAUI_MGR_ALLOW_FLOATING    |
							   wxAUI_MGR_TRANSPARENT_DRAG;

	int dockingStyle = EditorConfigST::Get()->GetOptions()->GetDockingStyle();
	switch(dockingStyle) {
	case 0: // Transparent hint
		auiMgrFlags |= wxAUI_MGR_TRANSPARENT_HINT;
		break;
	case 1: // Rectangle
		auiMgrFlags |= wxAUI_MGR_RECTANGLE_HINT;
		break;
	case 2: // Venetians blinds hint
		auiMgrFlags |= wxAUI_MGR_VENETIAN_BLINDS_HINT;
		break;
	}

#ifndef __WXGTK__
	auiMgrFlags |= wxAUI_MGR_LIVE_RESIZE;
#endif

	m_mgr.SetFlags( auiMgrFlags );
}

void clMainFrame::UpdateAUI()
{
	SetAUIManagerFlags();
	// Once loaded, update the output pane caption
	wxAuiPaneInfo& paneInfo = m_mgr.GetPane(wxT("Output View"));

	if (paneInfo.IsOk()) {
		paneInfo.CaptionVisible(EditorConfigST::Get()->GetOptions()->GetOutputPaneDockable());
		m_mgr.Update();
	}
}

void clMainFrame::OnRetaggingCompelted(wxCommandEvent& e)
{
	e.Skip();
#if USE_PARSER_TREAD_FOR_RETAGGING_WORKSPACE
	SetStatusMessage(wxT("Done"), 0);
	GetWorkspacePane()->ClearProgress();

	// Clear all cached tags now that we got our database updated
	TagsManagerST::Get()->ClearAllCaches();

	// Send event notifying parsing completed
	std::vector<std::string>* files = (std::vector<std::string>*) e.GetClientData();
	if(files) {

		// Print the parsing end time
		wxLogMessage(wxT("INFO: Retag workspace completed in %ld seconds (%lu files were scanned)"), gStopWatch.Time()/1000, (unsigned long)files->size());

		std::vector<wxFileName> taggedFiles;
		for(size_t i=0; i<files->size(); i++) {
			taggedFiles.push_back( wxFileName(wxString(files->at(i).c_str(), wxConvUTF8)) );
		}

		SendCmdEvent ( wxEVT_FILE_RETAGGED, ( void* ) &taggedFiles );
		delete files;

	} else {
		wxLogMessage(wxT("INFO: Retag workspace completed in 0 seconds (No files were retagged)"));

	}
	ManagerST::Get()->SetRetagInProgress(false);
#endif
}

void clMainFrame::OnRetaggingProgress(wxCommandEvent& e)
{
	e.Skip();
#if USE_PARSER_TREAD_FOR_RETAGGING_WORKSPACE
	if(e.GetInt() == 1) {
		// parsing started
		gStopWatch.Start();
	}

	GetWorkspacePane()->UpdateProgress( e.GetInt() );
#endif
}

void clMainFrame::OnRetagWorkspaceUI(wxUpdateUIEvent& event)
{
	CHECK_SHUTDOWN();
	event.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->GetRetagInProgress());
}

wxString clMainFrame::StartTTY(const wxString &title)
{
#ifndef __WXMSW__

	// try to locate the debug prespective
	wxString content;
	wxString debugPrespective(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/debug.layout"));
	wxFileName fn(debugPrespective);
	if(fn.FileExists()) {
		ReadFileWithConversion(fn.GetFullPath(), content);
	}

	// Create a new TTY Console and place it in the AUI
	ConsoleFrame *console = new ConsoleFrame(m_mainPanel);
	wxAuiPaneInfo paneInfo;
	paneInfo.Name(wxT("Debugger Console")).Float().Caption(title).Dockable().FloatingSize(300, 200).CloseButton(false);
	m_mgr.AddPane(console, paneInfo);

	if(!content.IsEmpty()) {
		m_mgr.LoadPerspective(content, false);
	}

	wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Debugger Console"));
	if(info.IsShown() == false) {
		info.Show();
	}

	m_mgr.Update();
	return console->StartTTY();

#else

	return wxT("");

#endif
}

void clMainFrame::OnViewWordWrap(wxCommandEvent& e)
{
	CHECK_SHUTDOWN();

	OptionsConfigPtr opts = EditorConfigST::Get()->GetOptions();
	opts->SetWordWrap(e.IsChecked());
	EditorConfigST::Get()->SetOptions(opts);

	GetMainBook()->SetViewWordWrap(e.IsChecked());
}

void clMainFrame::OnViewWordWrapUI(wxUpdateUIEvent& e)
{
	CHECK_SHUTDOWN();
	LEditor *editor = GetMainBook()->GetActiveEditor();
	bool hasEditor = editor ? true : false;
	if (!hasEditor) {
		e.Enable(false);
		return;
	}

	OptionsConfigPtr opts = EditorConfigST::Get()->GetOptions();
	e.Enable(true);
	e.Check(opts->GetWordWrap());
}

