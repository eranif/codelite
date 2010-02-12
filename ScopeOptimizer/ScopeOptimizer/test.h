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
#include "open_resource_dialog.h" // New open resource 
#include <wx/busyinfo.h>
#include "tags_parser_search_path_dlg.h"
#include "includepathlocator.h"
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
#include "custom_tab.h"
#include "custom_tabcontainer.h"
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
#include "custom_notebook.h"
#include "options_dlg2.h"
#include <wx/msgdlg.h>
#include "tabgroupdlg.h"

// from auto-generated file svninfo.cpp:
extern wxString CODELITE_VERSION_STR;
extern const wxChar *SvnRevision;

// from iconsextra.cpp:
extern char *cubes_xpm[];
extern unsigned char cubes_alpha[];

static int FrameTimerId = wxNewId();

const wxEventType wxEVT_UPDATE_STATUS_BAR = XRCID("update_status_bar");
const wxEventType wxEVT_LOAD_PERSPECTIVE  = XRCID("load_perspective");


//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(Frame, wxFrame)
	// ATTN: Any item that appears in more than one menu is
	//       only listed once here, the first time it occurs

	//---------------------------------------------------
	// System events
	//---------------------------------------------------
	//EVT_IDLE(Frame::OnIdle)
	EVT_ACTIVATE(Frame::OnAppActivated)
	EVT_CLOSE(Frame::OnClose)
	EVT_TIMER(FrameTimerId, Frame::OnTimer)
	//	EVT_AUI_RENDER(Frame::OnAuiManagerRender)
	//	EVT_AUI_PANE_CLOSE(Frame::OnDockablePaneClosed)

	//---------------------------------------------------
	// File menu
	//---------------------------------------------------
	EVT_MENU(XRCID("new_file"),                 Frame::OnFileNew)
	EVT_MENU(XRCID("open_file"),                Frame::OnFileOpen)
	EVT_MENU(XRCID("refresh_file"),             Frame::OnFileReload)
	EVT_MENU(XRCID("load_tab_group"),           Frame::OnFileLoadTabGroup)
	EVT_MENU(XRCID("save_file"),                Frame::OnSave)
	EVT_MENU(XRCID("save_file_as"),             Frame::OnSaveAs)
	EVT_MENU(XRCID("save_all"),                 Frame::OnFileSaveAll)
	EVT_MENU(XRCID("save_tab_group"),           Frame::OnFileSaveTabGroup)
	EVT_MENU(XRCID("close_file"),               Frame::OnFileClose)
	EVT_MENU(wxID_CLOSE_ALL,                    Frame::OnFileCloseAll)
	EVT_MENU_RANGE(RecentFilesSubMenuID, RecentFilesSubMenuID + 10, Frame::OnRecentFile)
	EVT_MENU_RANGE(RecentWorkspaceSubMenuID, RecentWorkspaceSubMenuID + 10, Frame::OnRecentWorkspace)
	EVT_MENU(XRCID("load_last_session"),        Frame::OnLoadLastSession)
	EVT_MENU(XRCID("exit_app"),                 Frame::OnQuit)

	EVT_UPDATE_UI(XRCID("refresh_file"),        Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_file"),           Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_file_as"),        Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_all"),            Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_tab_group"),      Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("close_file"),          Frame::OnFileCloseUI)
	EVT_UPDATE_UI(XRCID("load_last_session"),   Frame::OnLoadLastSessionUI)

	//--------------------------------------------------
	// Edit menu
	//--------------------------------------------------
	EVT_MENU(wxID_UNDO,                         Frame::DispatchCommandEvent)
	EVT_MENU(wxID_REDO,                         Frame::DispatchCommandEvent)
	EVT_MENU(wxID_DUPLICATE,                    Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_line"),              Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_line_end"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_line_start"),        Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("transpose_lines"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("to_upper"),                 Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("to_lower"),                 Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("match_brace"),              Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("select_to_brace"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("complete_word"),            Frame::OnCompleteWord)
	EVT_MENU(XRCID("function_call_tip"),        Frame::OnFunctionCalltip)
	EVT_MENU(XRCID("convert_eol_win"),          Frame::OnConvertEol)
	EVT_MENU(XRCID("convert_eol_unix"),         Frame::OnConvertEol)
	EVT_MENU(XRCID("convert_eol_mac"),          Frame::OnConvertEol)

	EVT_UPDATE_UI(wxID_UNDO,                    Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_REDO,                    Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_DUPLICATE,               Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("delete_line"),         Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("delete_line_end"),     Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("delete_line_start"),   Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("transpose_lines"),     Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("to_upper"),            Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("to_lower"),            Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("match_brace"),         Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("select_to_brace"),     Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("complete_word"),       Frame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("function_call_tip"),   Frame::OnFunctionCalltipUI)
	EVT_UPDATE_UI(XRCID("convert_eol_win"),     Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("convert_eol_unix"),    Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("convert_eol_mac"),     Frame::OnFileExistUpdateUI)

	//-------------------------------------------------------
	// View menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("word_wrap"),                Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_fold"),              Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("fold_all"),                 Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("display_eol"),              Frame::OnViewDisplayEOL)
	EVT_MENU(XRCID("whitepsace_invisible"),     Frame::OnShowWhitespace)
	EVT_MENU(XRCID("whitepsace_always"),        Frame::OnShowWhitespace)
	EVT_MENU(XRCID("whitespace_visiable_after_indent"), Frame::OnShowWhitespace)
	EVT_MENU(XRCID("whitespace_indent_only"),   Frame::OnShowWhitespace)
	EVT_MENU(XRCID("full_screen"),              Frame::OnShowFullScreen)
	EVT_MENU(XRCID("view_welcome_page"),        Frame::OnShowWelcomePage)
	EVT_MENU(XRCID("view_welcome_page_at_startup"), Frame::OnLoadWelcomePage)
	EVT_MENU(XRCID("show_nav_toolbar"),         Frame::OnShowNavBar)
	EVT_MENU(XRCID("toggle_panes"),             Frame::OnTogglePanes)
	EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchCommandEvent)

	EVT_UPDATE_UI(XRCID("word_wrap"),           Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("toggle_fold"),         Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("fold_all"),            Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("display_eol"),         Frame::OnViewDisplayEOL_UI)
	EVT_UPDATE_UI(XRCID("whitepsace_invisible"),    Frame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("whitepsace_always"),   Frame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("whitespace_visiable_after_indent"),    Frame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("whitespace_indent_only"), Frame::OnShowWhitespaceUI)
	EVT_UPDATE_UI(XRCID("view_welcome_page"),   Frame::OnShowWelcomePageUI)
	EVT_UPDATE_UI(XRCID("view_welcome_page_at_startup"),    Frame::OnLoadWelcomePageUI)
	EVT_UPDATE_UI(XRCID("show_nav_toolbar"),    Frame::OnShowNavBarUI)
	EVT_UPDATE_UI(viewAsSubMenuID,              Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchUpdateUIEvent)

	//-------------------------------------------------------
	// Search menu
	//-------------------------------------------------------
	EVT_MENU(wxID_FIND,                         Frame::DispatchCommandEvent)
	EVT_MENU(wxID_REPLACE,                      Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_function"),            Frame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_macro"),               Frame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_typedef"),             Frame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_resource"),            Frame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_type"),                Frame::OnFindResourceXXX   )
	EVT_MENU(XRCID("find_next"),                Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous"),            Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_next_at_caret"),       Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous_at_caret"),   Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("incremental_search"),       Frame::OnIncrementalSearch )
	EVT_MENU(XRCID("find_symbol"),              Frame::OnQuickOutline      )
	EVT_MENU(XRCID("goto_definition"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_previous_definition"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_linenumber"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_bookmark"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_bookmark"),            Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("previous_bookmark"),        Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("removeall_bookmarks"),      Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_fif_match"),           Frame::OnNextFiFMatch      )
	EVT_MENU(XRCID("previous_fif_match"),       Frame::OnPreviousFiFMatch  )

	EVT_UPDATE_UI(wxID_FIND,                        Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(wxID_REPLACE,                     Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_next"),               Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_previous"),           Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_next_at_caret"),      Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_previous_at_caret"),  Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("incremental_search"),      Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("find_resource"),           Frame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_type"),               Frame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_function"),           Frame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_macro"),              Frame::OnWorkspaceOpen       )
	EVT_UPDATE_UI(XRCID("find_typedef"),            Frame::OnWorkspaceOpen       )

	EVT_UPDATE_UI(XRCID("find_symbol"),             Frame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("goto_definition"),         Frame::DispatchUpdateUIEvent )
	EVT_UPDATE_UI(XRCID("goto_previous_definition"),Frame::DispatchUpdateUIEvent )
	EVT_UPDATE_UI(XRCID("goto_linenumber"),         Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("toggle_bookmark"),         Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("next_bookmark"),           Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("previous_bookmark"),       Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("removeall_bookmarks"),     Frame::OnFileExistUpdateUI   )
	EVT_UPDATE_UI(XRCID("next_fif_match"),          Frame::OnNextFiFMatchUI      )
	EVT_UPDATE_UI(XRCID("previous_fif_match"),      Frame::OnPreviousFiFMatchUI  )

	//-------------------------------------------------------
	// Project menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("new_workspace"),            Frame::OnProjectNewWorkspace)
	EVT_MENU(XRCID("switch_to_workspace"),      Frame::OnSwitchWorkspace)
	EVT_MENU(XRCID("close_workspace"),          Frame::OnCloseWorkspace)
	EVT_MENU(XRCID("reload_workspace"),         Frame::OnReloadWorkspace)
	EVT_MENU(XRCID("import_from_msvs"),         Frame::OnImportMSVS)
	EVT_MENU(XRCID("new_project"),              Frame::OnProjectNewProject)
	EVT_MENU(XRCID("add_project"),              Frame::OnProjectAddProject)
	EVT_MENU(XRCID("retag_workspace"),          Frame::OnRetagWorkspace)
	EVT_MENU(XRCID("full_retag_workspace"),     Frame::OnRetagWorkspace)
	EVT_MENU(XRCID("project_properties"),       Frame::OnShowActiveProjectSettings)

	EVT_UPDATE_UI(XRCID("close_workspace"),     Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("reload_workspace"),    Frame::OnReloadWorkspaceUI)
	EVT_UPDATE_UI(XRCID("add_project"),         Frame::OnWorkspaceMenuUI)
	EVT_UPDATE_UI(XRCID("retag_workspace"),     Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("full_retag_workspace"),Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("project_properties"),  Frame::OnShowActiveProjectSettingsUI)

	//-------------------------------------------------------
	// Build menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("execute_no_debug"),         Frame::OnExecuteNoDebug)
	EVT_MENU(XRCID("stop_executed_program"),    Frame::OnStopExecutedProgram)
	EVT_MENU(XRCID("build_active_project"),     Frame::OnBuildProject)
	EVT_MENU(XRCID("compile_active_file"),      Frame::OnCompileFile)
	EVT_MENU(XRCID("clean_active_project"),     Frame::OnCleanProject)
	EVT_MENU(XRCID("stop_active_project_build"),    Frame::OnStopBuild)
	EVT_MENU(XRCID("rebuild_active_project"),   Frame::OnRebuildProject)
	EVT_MENU(XRCID("build_n_run_active_project"),   Frame::OnBuildAndRunProject)
	EVT_MENU(XRCID("build_workspace"),          Frame::OnBuildWorkspace)
	EVT_MENU(XRCID("clean_workspace"),          Frame::OnCleanWorkspace)
	EVT_MENU(XRCID("rebuild_workspace"),        Frame::OnReBuildWorkspace)
	EVT_MENU(XRCID("batch_build"),              Frame::OnBatchBuild)

	EVT_UPDATE_UI(XRCID("execute_no_debug"),        Frame::OnExecuteNoDebugUI)
	EVT_UPDATE_UI(XRCID("stop_executed_program"),   Frame::OnStopExecutedProgramUI)
	EVT_UPDATE_UI(XRCID("build_active_project"),    Frame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("compile_active_file"),     Frame::OnCompileFileUI)
	EVT_UPDATE_UI(XRCID("clean_active_project"),    Frame::OnCleanProjectUI)
	EVT_UPDATE_UI(XRCID("stop_active_project_build"),   Frame::OnStopBuildUI)
	EVT_UPDATE_UI(XRCID("rebuild_active_project"),  Frame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("build_n_run_active_project"),  Frame::OnBuildProjectUI)
	EVT_UPDATE_UI(XRCID("build_workspace"),         Frame::OnBuildWorkspaceUI)
	EVT_UPDATE_UI(XRCID("clean_workspace"),         Frame::OnCleanWorkspaceUI)
	EVT_UPDATE_UI(XRCID("rebuild_workspace"),       Frame::OnReBuildWorkspaceUI)
	EVT_UPDATE_UI(XRCID("batch_build"),             Frame::OnBatchBuildUI)

	//-------------------------------------------------------
	// Debug menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("start_debugger"),           Frame::OnDebug)
	EVT_MENU(XRCID("restart_debugger"),         Frame::OnDebugRestart)
	EVT_MENU(XRCID("attach_debugger"),          Frame::OnDebugAttach)
	EVT_MENU(XRCID("pause_debugger"),           Frame::OnDebugCmd)
	EVT_MENU(XRCID("stop_debugger"),            Frame::OnDebugStop)
	EVT_MENU(XRCID("dbg_stepin"),               Frame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_stepout"),              Frame::OnDebugCmd)
	EVT_MENU(XRCID("dbg_next"),                 Frame::OnDebugCmd)
	EVT_MENU(XRCID("show_cursor"),              Frame::OnDebugCmd)
	EVT_MENU(XRCID("add_breakpoint"),						Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("insert_breakpoint"),        Frame::DispatchCommandEvent) // Toggles
	EVT_MENU(XRCID("insert_temp_breakpoint"),   Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("insert_cond_breakpoint"),   Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("edit_breakpoint"),          Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("show_breakpoint_dlg"),      Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("insert_watchpoint"),        Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_breakpoint_enabled_status"),  Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("ignore_breakpoint"),        Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("delete_breakpoint"),        Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("quick_debug"),              Frame::OnQuickDebug)

	EVT_UPDATE_UI(XRCID("start_debugger"),      Frame::OnDebugUI)
	EVT_UPDATE_UI(XRCID("restart_debugger"),    Frame::OnDebugRestartUI)
	EVT_UPDATE_UI(XRCID("pause_debugger"),      Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("stop_debugger"),       Frame::OnDebugStopUI)
	EVT_UPDATE_UI(XRCID("dbg_stepin"),          Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("dbg_stepout"),         Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("dbg_next"),            Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("show_cursor"),         Frame::OnDebugCmdUI)
	EVT_UPDATE_UI(XRCID("insert_breakpoint"),   Frame::OnDebugManageBreakpointsUI)
	EVT_UPDATE_UI(XRCID("quick_debug"),         Frame::OnQuickDebugUI)

	//-------------------------------------------------------
	// Plugins menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("manage_plugins"),           Frame::OnManagePlugins)

	//-------------------------------------------------------
	// Settings menu
	//-------------------------------------------------------
	EVT_MENU(wxID_PREFERENCES,                  Frame::OnViewOptions)
	EVT_MENU(XRCID("syntax_highlight"),         Frame::OnSyntaxHighlight)
	EVT_MENU(XRCID("configure_accelerators"),   Frame::OnConfigureAccelerators)
	EVT_MENU(XRCID("add_envvar"),               Frame::OnAddEnvironmentVariable)
	EVT_MENU(XRCID("advance_settings"),         Frame::OnAdvanceSettings)
	EVT_MENU(XRCID("debuger_settings"),         Frame::OnDebuggerSettings)
	EVT_MENU(XRCID("tags_options"),             Frame::OnCtagsOptions)

	//-------------------------------------------------------
	// Help menu
	//-------------------------------------------------------
	EVT_MENU(wxID_ABOUT,                        Frame::OnAbout)
	EVT_MENU(XRCID("check_for_update"),         Frame::OnCheckForUpdate)

	//-----------------------------------------------------------------
	// Toolbar
	//-----------------------------------------------------------------
	EVT_MENU(wxID_FORWARD,                      Frame::OnBackwardForward)
	EVT_MENU(wxID_BACKWARD,                     Frame::OnBackwardForward)
	EVT_MENU(XRCID("highlight_word"),           Frame::OnHighlightWord)

	EVT_UPDATE_UI(wxID_FORWARD,                 Frame::OnBackwardForwardUI)
	EVT_UPDATE_UI(wxID_BACKWARD,                Frame::OnBackwardForwardUI)

	//-------------------------------------------------------
	// Workspace Pane tab context menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("detach_wv_tab"),            Frame::OnDetachWorkspaceViewTab)

	//-------------------------------------------------------
	// Debugger Pane tab context menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("detach_dv_tab"),            Frame::OnDetachDebuggerViewTab)

	//-------------------------------------------------------
	// Editor tab context menu
	//-------------------------------------------------------
	EVT_MENU(XRCID("close_other_tabs"),         Frame::OnCloseAllButThis)
	EVT_MENU(XRCID("copy_file_name"),           Frame::OnCopyFilePath)
	EVT_MENU(XRCID("copy_file_path"),           Frame::OnCopyFilePathOnly)
	EVT_MENU(XRCID("detach_tab"),               Frame::OnDetachTab)
	EVT_MENU(XRCID("open_shell_from_filepath"), Frame::OnOpenShellFromFilePath)

	EVT_UPDATE_UI(XRCID("copy_file_name"),      Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("copy_file_path"),      Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("detach_tab"),          Frame::OnDetachTabUI)
	EVT_UPDATE_UI(XRCID("open_shell_from_filepath"),    Frame::OnFileExistUpdateUI)

	//-----------------------------------------------------------------
	// Default editor context menu
	//-----------------------------------------------------------------
	EVT_MENU(wxID_DELETE,                       Frame::DispatchCommandEvent)
	EVT_UPDATE_UI(wxID_DELETE,                  Frame::DispatchUpdateUIEvent)

	//-----------------------------------------------------------------
	// C++ editor context menu
	//-----------------------------------------------------------------
	EVT_MENU(XRCID("add_include_file"),         Frame::OnCppContextMenu)
	EVT_MENU(XRCID("swap_files"),               Frame::OnCppContextMenu)
	EVT_MENU(XRCID("find_decl"),                Frame::OnCppContextMenu)
	EVT_MENU(XRCID("find_impl"),                Frame::OnCppContextMenu)
	EVT_MENU(XRCID("go_to_function_start"),     Frame::OnCppContextMenu)
	EVT_MENU(XRCID("go_to_next_function"),      Frame::OnCppContextMenu)
	EVT_MENU(XRCID("insert_doxy_comment"),      Frame::OnCppContextMenu)
	EVT_MENU(XRCID("setters_getters"),          Frame::OnCppContextMenu)
	EVT_MENU(XRCID("move_impl"),                Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_impl"),                 Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_multi_impl"),           Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_virtual_impl"),         Frame::OnCppContextMenu)
	EVT_MENU(XRCID("add_pure_virtual_impl"),    Frame::OnCppContextMenu)
	EVT_MENU(XRCID("rename_function"),          Frame::OnCppContextMenu)
	EVT_MENU(XRCID("comment_selection"),        Frame::OnCppContextMenu)
	EVT_MENU(XRCID("comment_line"),             Frame::OnCppContextMenu)
	EVT_MENU(XRCID("retag_file"),               Frame::OnCppContextMenu)

	//-----------------------------------------------------------------
	// Hyperlinks
	//-----------------------------------------------------------------
	EVT_HTML_LINK_CLICKED(wxID_ANY,             Frame::OnLinkClicked)
	EVT_MENU(XRCID("link_action"),              Frame::OnStartPageEvent)

	//-----------------------------------------------------------------
	// CodeLite-specific events
	//-----------------------------------------------------------------
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS, Frame::OnParsingThreadDone   )
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_MESSAGE             , Frame::OnParsingThreadMessage)
	EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE,     Frame::OnClearTagsCache)
	EVT_MENU   (XRCID("update_num_builders_count"),                Frame::OnUpdateNumberOfBuildProcesses)
	EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_STATUS_BAR, Frame::OnSetStatusMessage)
	EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE,   Frame::OnDatabaseUpgrade )
	EVT_COMMAND(wxID_ANY, wxEVT_SHELL_COMMAND_PROCESS_ENDED, Frame::OnBuildEnded)

	EVT_SYMBOLTREE_ADD_ITEM(wxID_ANY,    Frame::OnAddSymbols)
	EVT_SYMBOLTREE_DELETE_ITEM(wxID_ANY, Frame::OnDeleteSymbols)
	EVT_SYMBOLTREE_UPDATE_ITEM(wxID_ANY, Frame::OnUpdateSymbols)

	EVT_COMMAND(wxID_ANY, wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, Frame::OnSingleInstanceOpenFiles)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP,  Frame::OnSingleInstanceRaise)

	EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_VERSION_AVAILABLE, Frame::OnNewVersionAvailable)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_UPTODATE,      Frame::OnNewVersionAvailable)

	EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_DOCKPANE,    Frame::OnNewDetachedPane)
	EVT_COMMAND(wxID_ANY, wxEVT_LOAD_PERSPECTIVE,    Frame::OnLoadPerspective)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_DELETE_DOCKPANE, Frame::OnDestroyDetachedPane)

	EVT_MENU(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED,          Frame::OnReloadExternallModified)
	EVT_MENU(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, Frame::OnReloadExternallModifiedNoPrompt)
END_EVENT_TABLE()


Frame* Frame::m_theFrame = NULL;

Frame::Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
		: wxFrame(pParent, id, title, pos, size, style)
		, m_buildAndRun(false)
		, m_cppMenu(NULL)
		, m_highlightWord(false)
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
	JobQueueSingleton::Instance()->Start(5);

	// the single instance job is a presisstent job, so the pool will contain only 4 available threads
	JobQueueSingleton::Instance()->PushJob(new SingleInstanceThreadJob(this, ManagerST::Get()->GetStarupDirectory()));

	//start the editor creator thread
	m_timer = new wxTimer(this, FrameTimerId);

	// connect common edit events
	wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::DispatchCommandEvent), NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::DispatchCommandEvent), NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::DispatchCommandEvent), NULL, this);
	wxTheApp->Connect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::DispatchCommandEvent), NULL, this);

	wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Frame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Frame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Frame::DispatchUpdateUIEvent ), NULL, this);
	wxTheApp->Connect(wxID_CUT,       wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Frame::DispatchUpdateUIEvent ), NULL, this);
}

Frame::~Frame(void)
{
	delete m_timer;
	ManagerST::Free();
	delete m_DPmenuMgr;

	// uninitialize AUI manager
	m_mgr.UnInit();
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

	m_theFrame = new Frame( NULL,
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
	m_theFrame->GetFileExplorer()->ProcessEvent(e);

	//load last session?
	if (m_theFrame->m_frameGeneralInfo.GetFlags() & CL_LOAD_LAST_SESSION && loadLastSession) {
		m_theFrame->LoadSession(SessionManager::Get().GetLastSession());
	}

	m_theFrame->SendSizeEvent();
	m_theFrame->StartTimer();

	// After all the plugins / panes have been loaded,
	// its time to re-load the perspective
	wxCommandEvent evt(wxEVT_LOAD_PERSPECTIVE);
	m_theFrame->AddPendingEvent( evt );
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
	m_mainPanel = new wxPanel(this);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	mainSizer->Add(m_mainPanel, 1, wxEXPAND);

	// tell wxAuiManager to manage this frame
	m_mgr.SetManagedWindow(m_mainPanel);
	m_mgr.SetArtProvider(new CLAuiDockArt());

#ifndef __WXMSW__
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE);
#endif

// Mac only
#if defined (__WXMAC__) || defined (__WXGTK__)
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR,        wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,      DrawingUtils::GetPanelBgColour());
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR,   wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT));
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR,                  DrawingUtils::GetPanelBgColour());
	m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR,            DrawingUtils::GetPanelBgColour());
#endif

	//initialize debugger configuration tool
	DebuggerConfigTool::Get()->Load(wxT("config/debuggers.xml"), wxT("2.0.2"));
	WorkspaceST::Get()->SetStartupDir(ManagerST::Get()->GetStarupDirectory());

	// On wx2.8.7, AUI dragging is broken but this happens only in debug build & on GTK
#if defined (__WXGTK__) && defined (__WXDEBUG__)
	m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING|wxAUI_MGR_ALLOW_ACTIVE_PANE|wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_RECTANGLE_HINT);
#endif

	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 6);

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
	m_mgr.AddPane(m_outputPane, paneInfo.Name(wxT("Output View")).Caption(wxT("Output View")).Bottom().Layer(2).Position(1).CaptionVisible(false));
	RegisterDockWindow(XRCID("output_pane"), wxT("Output View"));

	// Add the explorer pane
	m_workspacePane = new WorkspacePane(m_mainPanel, wxT("Workspace View"), &m_mgr);
	m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo().
	              Name(m_workspacePane->GetCaption()).Caption(m_workspacePane->GetCaption()).
	              Left().BestSize(250, 300).Layer(2).Position(0).CloseButton(true));
	RegisterDockWindow(XRCID("workspace_pane"), wxT("Workspace View"));

	//add the debugger locals tree, make it hidden by default
	m_debuggerPane = new DebuggerPane(m_mainPanel, wxT("Debugger"), &m_mgr);
	m_mgr.AddPane(m_debuggerPane,
	              wxAuiPaneInfo().Name(m_debuggerPane->GetCaption()).Caption(m_debuggerPane->GetCaption()).Bottom().Layer(1).Position(1).CloseButton(true).Hide());
	RegisterDockWindow(XRCID("debugger_pane"), wxT("Debugger"));

	m_mainBook = new MainBook(m_mainPanel);
	m_mgr.AddPane(m_mainBook, wxAuiPaneInfo().Name(wxT("Editor")).CenterPane().PaneBorder(true));
	CreateRecentlyOpenedFilesMenu();

	long show_nav(1);
	EditorConfigST::Get()->GetLongValue(wxT("ShowNavBar"), show_nav);
	if ( !show_nav ) {
		m_mainBook->ShowNavBar( false );
	}

	BuildSettingsConfigST::Get()->Load(wxT("2.0.4"));

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
	tagsManager->StartCtagsProcess();

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

	//load the tab right click menu
	GetWorkspacePane()->GetNotebook()->SetRightClickMenu(wxXmlResource::Get()->LoadMenu(wxT("workspace_view_right_click_menu")));
	GetDebuggerPane()->GetNotebook()->SetRightClickMenu(wxXmlResource::Get()->LoadMenu(wxT("debugger_view_right_click_menu")));

	// construct the output view control bar
	m_controlBar = new OutputViewControlBar(this, GetOutputPane()->GetNotebook(), &m_mgr, wxID_ANY);
	mainSizer->Add(m_controlBar, 0, wxEXPAND);

	m_mgr.Update();
	SetAutoLayout (true);

	//load debuggers
	DebuggerMgr::Get().Initialize(this, EnvironmentConfig::Instance(), ManagerST::Get()->GetInstallDir());
	DebuggerMgr::Get().LoadDebuggers();

	wxString sessConfFile;
	sessConfFile << ManagerST::Get()->GetStarupDirectory() << wxT("/config/sessions.xml");
	SessionManager::Get().Load(sessConfFile);

	//try to locate the build tools

	long fix(1);
	EditorConfigST::Get()->GetLongValue(wxT("FixBuildToolOnStartup"), fix);
	if ( fix ) {
		UpdateBuildTools();
	}

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

void Frame::CreateToolbars24()
{
	wxAuiPaneInfo info;
	wxWindow *toolbar_parent (this);
	if (PluginManager::Get()->AllowToolbar()) {
		toolbar_parent = m_mainPanel;
	}

	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	wxToolBar *tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
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


	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());
	}
	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		info = wxAuiPaneInfo();
		tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		tb->SetToolBitmapSize(wxSize(24, 24));
	}
	tb->AddTool(wxID_FIND, wxT("Find"), wxXmlResource::Get()->LoadBitmap(wxT("find_and_replace24")), wxT("Find"));
	tb->AddTool(wxID_REPLACE, wxT("Replace"), wxXmlResource::Get()->LoadBitmap(wxT("refresh24")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"), wxXmlResource::Get()->LoadBitmap(wxT("find_in_files24")), wxT("Find In Files"));
	tb->AddSeparator();
	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_resource24")), wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"), wxT("Find Type In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_type24")), wxT("Find Type In Workspace"));
	tb->AddTool(XRCID("find_symbol"), wxT("Quick Outline"), wxXmlResource::Get()->LoadBitmap(wxT("outline24")), wxT("Show Current File Outline"));
	tb->AddSeparator();
	tb->AddTool(XRCID("highlight_word"), wxT("Highlight Word"), wxXmlResource::Get()->LoadBitmap(wxT("highlight24")), wxT("Highlight Word"), wxITEM_CHECK);
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
		tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		tb->SetToolBitmapSize(wxSize(24, 24));
	}

	tb->AddTool(XRCID("build_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("build_active_project24")), wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_build24")), wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("clean24")), wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("execute24")), wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_executed_program24")), wxT("Stop Running Program"));

	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Build")).ToolbarPane().Top().Row(1));
	}
	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		tb->SetToolBitmapSize(wxSize(24, 24));
	}

	tb->AddTool(XRCID("start_debugger"), wxT("Start / Continue debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_start24")), wxT("Start / Continue debugger"));
	tb->AddTool(XRCID("stop_debugger"), wxT("Stop debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stop24")), wxT("Stop debugger"));
	tb->AddTool(XRCID("pause_debugger"), wxT("Pause debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_pause24")), wxT("Pause debugger"));
	tb->AddTool(XRCID("restart_debugger"), wxT("Restart debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_restart24")), wxT("Restart debugger"));
	tb->AddSeparator();
	tb->AddTool(XRCID("show_cursor"), wxT("Show Current Line"), wxXmlResource::Get()->LoadBitmap(wxT("arrow_green_right24")), wxT("Show Current Line"));
	tb->AddSeparator();
	tb->AddTool(XRCID("dbg_stepin"), wxT("Step Into"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepin24")), wxT("Step In"));
	tb->AddTool(XRCID("dbg_next"), wxT("Next"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_next24")), wxT("Next"));
	tb->AddTool(XRCID("dbg_stepout"), wxT("Step Out"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepout24")), wxT("Step Out"));
	tb->Realize();

	if (PluginManager::Get()->AllowToolbar()) {
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Debug")).ToolbarPane().Top().Row(1));

		RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
		RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
		RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
		RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
	} else {
		SetToolBar(tb);
	}
}

void Frame::CreateToolbars16()
{
	//----------------------------------------------
	//create the standard toolbar
	//----------------------------------------------
	wxWindow *toolbar_parent (this);
	if (PluginManager::Get()->AllowToolbar()) {
		toolbar_parent = m_mainPanel;
	}

	wxToolBar *tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	wxAuiPaneInfo info;

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


	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());
	}

	//----------------------------------------------
	//create the search toolbar
	//----------------------------------------------
	info = wxAuiPaneInfo();

	if (PluginManager::Get()->AllowToolbar()) {
		tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		tb->SetToolBitmapSize(wxSize(16, 16));
	}

	tb->AddTool(wxID_FIND, wxT("Find"), wxXmlResource::Get()->LoadBitmap(wxT("find_and_replace16")), wxT("Find"));
	tb->AddTool(wxID_REPLACE, wxT("Replace"), wxXmlResource::Get()->LoadBitmap(wxT("refresh16")), wxT("Replace"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"), wxXmlResource::Get()->LoadBitmap(wxT("find_in_files16")), wxT("Find In Files"));
	tb->AddSeparator();

	tb->AddTool(XRCID("find_resource"), wxT("Find Resource In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_resource16")), wxT("Find Resource In Workspace"));
	tb->AddTool(XRCID("find_type"), wxT("Find Type In Workspace"), wxXmlResource::Get()->LoadBitmap(wxT("open_type16")), wxT("Find Type In Workspace"));
	tb->AddTool(XRCID("find_symbol"), wxT("Quick Outline"), wxXmlResource::Get()->LoadBitmap(wxT("outline16")), wxT("Show Current File Outline"));
	tb->AddSeparator();
	tb->AddTool(XRCID("highlight_word"), wxT("Highlight Word"), wxXmlResource::Get()->LoadBitmap(wxT("highlight16")), wxT("Highlight Word"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
	tb->AddSeparator();

	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Search")).ToolbarPane().Top());

		//----------------------------------------------
		//create the build toolbar
		//----------------------------------------------
		tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		tb->SetToolBitmapSize(wxSize(16, 16));
	}

	tb->AddTool(XRCID("build_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("build_active_project16")), wxT("Build Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_build16")), wxT("Stop Current Build"));
	tb->AddTool(XRCID("clean_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("clean16")), wxT("Clean Active Project"));
	tb->AddSeparator();
	tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("execute16")), wxT("Run Active Project"));
	tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_executed_program16")), wxT("Stop Running Program"));

	if (PluginManager::Get()->AllowToolbar()) {
		tb->Realize();
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Build")).ToolbarPane().Top().Row(1));
	}

	//----------------------------------------------
	//create the debugger toolbar
	//----------------------------------------------
	if (PluginManager::Get()->AllowToolbar()) {
		tb = new wxToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		tb->SetToolBitmapSize(wxSize(16, 16));
	}

	tb->AddTool(XRCID("start_debugger"), wxT("Start / Continue debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_start16")), wxT("Start / Continue debugger"));
	tb->AddTool(XRCID("stop_debugger"), wxT("Stop debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stop16")), wxT("Stop debugger"));
	tb->AddTool(XRCID("pause_debugger"), wxT("Pause debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_pause16")), wxT("Pause debugger"));
	tb->AddTool(XRCID("restart_debugger"), wxT("Restart debugger"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_restart16")), wxT("Restart debugger"));
	tb->AddSeparator();
	tb->AddTool(XRCID("show_cursor"), wxT("Show Current Line"), wxXmlResource::Get()->LoadBitmap(wxT("arrow_green_right16")), wxT("Show Current Line"));
	tb->AddSeparator();
	tb->AddTool(XRCID("dbg_stepin"), wxT("Step Into"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepin16")), wxT("Step In"));
	tb->AddTool(XRCID("dbg_next"), wxT("Next"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_next16")), wxT("Next"));
	tb->AddTool(XRCID("dbg_stepout"), wxT("Step Out"), wxXmlResource::Get()->LoadBitmap(wxT("debugger_stepout16")), wxT("Step Out"));

	tb->Realize();
	if (PluginManager::Get()->AllowToolbar()) {
		info = wxAuiPaneInfo();
		m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Debug")).ToolbarPane().Top().Row(1));
		RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
		RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
		RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
		RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
	} else {
		SetToolBar(tb);
	}
}

void Frame::UpdateBuildTools()
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

void Frame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}
//----------------------------------------------------
// Helper method for the event handling
//----------------------------------------------------

static bool IsEditorEvent(wxEvent &event)
{
	// Handle common edit events
	// if the focused window is *not* LEditor,
	// and the focused windows is of type
	// wxTextCtrl or wxScintilla, let the focused
	// Window handle the event
	wxWindow *focusWin = wxWindow::FindFocus();
	if ( focusWin ) {
		switch (event.GetId()) {
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
	return true;
}

void Frame::DispatchCommandEvent(wxCommandEvent &event)
{
	if ( !IsEditorEvent(event) ) {
		event.Skip();
		return;
	}

	// Do the default and pass this event to the Editor
	LEditor* editor = GetMainBook()->GetActiveEditor();
	if ( !editor )
		return;

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

void Frame::OnFileExistUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = GetMainBook()->GetActiveEditor();
	if ( !editor ) {
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}

void Frame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString mainTitle;
	mainTitle = CODELITE_VERSION_STR;

	AboutDlg dlg(this, mainTitle);
	dlg.SetInfo(mainTitle);
	dlg.ShowModal();
}

void Frame::OnClose(wxCloseEvent& event)
{
	ManagerST::Get()->SetShutdownInProgress(true);

	// Stop the search thread
	ManagerST::Get()->KillProgram();
	ManagerST::Get()->DbgStop();
	SearchThreadST::Get()->StopSearch();

	SaveLayoutAndSession();
	event.Skip();
}

void Frame::LoadSession(const wxString &sessionName)
{
	try {
		SessionEntry session;
		
	