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
#include <wx/busyinfo.h>
#include "tags_parser_search_path_dlg.h"
#include "includepathlocator.h"
#include "quickfinder.h"
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
#include "open_resouce_dlg.h"
#include "open_type_dlg.h"
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
	EVT_MENU(XRCID("quickfinder_class"),        Frame::OnShowQuickFinder   )
	EVT_MENU(XRCID("quickfinder_function"),     Frame::OnShowQuickFinder   )
	EVT_MENU(XRCID("quickfinder_macro"),        Frame::OnShowQuickFinder   )
	EVT_MENU(XRCID("quickfinder_typedef"),      Frame::OnShowQuickFinder   )
	EVT_MENU(XRCID("quickfinder_file"),         Frame::OnShowQuickFinder   )
	EVT_MENU(XRCID("find_next"),                Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous"),            Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_next_at_caret"),       Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous_at_caret"),   Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("incremental_search"),       Frame::OnIncrementalSearch )
	EVT_MENU(XRCID("find_resource"),            Frame::OnFindResource      )
	EVT_MENU(XRCID("find_type"),                Frame::OnFindType          )
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
//	EVT_UPDATE_UI(XRCID("quickfinder_class"),       Frame::OnShowQuickFinderUI )
//	EVT_UPDATE_UI(XRCID("quickfinder_function"),    Frame::OnShowQuickFinderUI )
//	EVT_UPDATE_UI(XRCID("quickfinder_macro"),       Frame::OnShowQuickFinderUI )
//	EVT_UPDATE_UI(XRCID("quickfinder_typedef"),     Frame::OnShowQuickFinderUI )
//	EVT_UPDATE_UI(XRCID("quickfinder_file"),        Frame::OnShowQuickFinderUI )


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
	DebuggerConfigTool::Get()->Load(wxT("config/debuggers.xml"), wxT("2.0.1"));
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
	m_status.resize(4);
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

	// Initialize the QuickFinder
	QuickFinder::Initialize(PluginManager::Get());

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
	dlg.SetInfo(wxString::Format(wxT("SVN build, revision: %s"), SvnRevision));
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

void Frame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->SaveFile();
	}
}

void Frame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->SaveFileAs();
	}
}

void Frame::OnFileLoadTabGroup(wxCommandEvent& WXUNUSED(event))
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

void Frame::OnFileReload(wxCommandEvent &event)
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

void Frame::OnCloseWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen()) {
		ManagerST::Get()->CloseWorkspace();
		ShowWelcomePage();
	}
}

void Frame::OnSwitchWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);

	// now it is time to prompt user for new workspace to open
	const wxString ALL(wxT("CodeLite Workspace files (*.workspace)|*.workspace|")
	                   wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Workspace"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		ManagerST::Get()->OpenWorkspace(dlg->GetPath());
	}
	dlg->Destroy();
}

void Frame::OnCompleteWord(wxCommandEvent& event)
{
	wxUnusedVar(event);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->CompleteWord();
	}
}

void Frame::OnFunctionCalltip(wxCommandEvent& event)
{
	wxUnusedVar(event);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->ShowFunctionTipFromCurrentPos();
	}
}

// Open new file
void Frame::OnFileNew(wxCommandEvent &event)
{
	wxUnusedVar(event);
	GetMainBook()->NewEditor();
}

void Frame::OnFileOpen(wxCommandEvent & WXUNUSED(event))
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

void Frame::OnFileClose(wxCommandEvent &event)
{
	wxUnusedVar( event );
	GetMainBook()->ClosePage(GetMainBook()->GetCurrentPage());
}

void Frame::OnFileSaveAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	GetMainBook()->SaveAll(false, true);
}

void Frame::OnFileSaveTabGroup(wxCommandEvent& WXUNUSED(event))
{
	wxArrayString previousgroups;
	EditorConfigST::Get()->GetRecentItems( previousgroups, wxT("RecentTabgroups") );

	SaveTabGroupDlg dlg(this, previousgroups);
	wxString path = ManagerST::Get()->IsWorkspaceOpen() ? WorkspaceST::Get()->GetWorkspaceFileName().GetPath() : wxGetHomeDir();
	dlg.SetComboPath(path);

	std::vector<LEditor*> editors; wxArrayString filepaths;
	GetMainBook()->GetAllEditors(editors);
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

		path = dlg.GetComboPath();
		if (path.IsEmpty() || !wxFileName::DirExists(path)) {
			if ( wxMessageBox(_("Please enter a valid directory in which to save the tab group"), wxT("CodeLite"), wxICON_ERROR|wxOK|wxCANCEL, this) != wxOK ) {
				return;
			} else {
				continue;
			}
		}

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

void Frame::OnCompleteWordUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = GetMainBook()->GetActiveEditor();
	// This menu item is enabled only if the current editor belongs to a project
	event.Enable(editor && ManagerST::Get()->IsWorkspaceOpen());
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
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Project"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST , wxDefaultPosition);
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

		wxArrayString pathsAfter = m_tagsOptionsData.GetParserSearchPaths();
		wxArrayString removedPaths;

		// Compare the paths
		for(size_t i=0; i<pathsBefore.GetCount(); i++) {
			int where = pathsAfter.Index(pathsBefore.Item(i));
			if(where == wxNOT_FOUND) {
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
			for(size_t i=0; i<removedPaths.GetCount(); i++) {
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

		if(pathsAfter.IsEmpty() == false) {
			// a retagg is needed
			wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
			AddPendingEvent(e);
		}
	}
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
	OptionsDlg2 dlg(this);
	dlg.ShowModal();

	if ( dlg.restartRquired ) {
#ifdef __WXMAC__
		wxMessageBox(_("Some of the changes made requires restart of CodeLite"), wxT("CodeLite"), wxICON_INFORMATION|wxOK, this);
#else
		// On Winodws & GTK we offer auto-restart
		int answer = wxMessageBox(_("Some of the changes made requires restart of CodeLite\nWould you like to restart now?"), wxT("CodeLite"), wxICON_INFORMATION|wxYES_NO|wxCANCEL, this);
		if ( answer == wxYES ) {
			wxCommandEvent e(wxEVT_CMD_RESTART_CODELITE);
			ManagerST::Get()->AddPendingEvent(e);
		}
#endif
	}
}

void Frame::OnTogglePanes(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->TogglePanes();
}

void Frame::OnAddEnvironmentVariable(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarsTableDlg dlg(this);
	dlg.ShowModal();

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

void Frame::OnBuildEnded(wxCommandEvent &event)
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
void Frame::OnBuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	if (enable) {

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
	}
}

void Frame::OnBuildCustomTarget(wxCommandEvent& event)
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

void Frame::OnBuildAndRunProject(wxCommandEvent &event)
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

void Frame::OnRebuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	RebuildProject(ManagerST::Get()->GetActiveProjectName());
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

void Frame::OnTimer(wxTimerEvent &event)
{
	// since there is a bug in wxURL, which it can not be used while constucting a wxFrame,
	// it must be called *after* the frame constuction
	// add new version notification updater
	long check(1);
	EditorConfigST::Get()->GetLongValue(wxT("CheckNewVersion"), check);

	if ( check ) {
		JobQueueSingleton::Instance()->PushJob(new WebUpdateJob(this));
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

				//prompt the user
				long val(0);
				bool do_it(false);
				if ( !EditorConfigST::Get()->GetLongValue(wxT("AdjustCPUNumber"), val) ) {

					//no entry was found in the configuration file, popup the dialog and ask the user
					ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL, wxT("Should CodeLite adjust the number of concurrent build jobs to match the number of CPUs?"), wxT("CodeLite"));
					if (dlg->ShowModal() == wxID_OK) {
						do_it = true;
					}

					if ( dlg->GetDontAskMeAgain() ) {
						// the user wishes that his answer will be kept
						EditorConfigST::Get()->SaveLongValue(wxT("AdjustCPUNumber"), do_it ? 1 : 0);
					}
					dlg->Destroy();

				} else {
					do_it = !(val == 0);
				}

				// are we allowed to update the concurrent jobs number?
				if ( do_it ) {
					bs->SetToolJobs( jobs );
					BuildSettingsConfigST::Get()->SetBuildSystem(bs);
					wxLogMessage(wxT("Info: setting number of concurrent builder jobs to ") + jobs);
				}
			}
		}

		// enable/disable plugins toolbar functionality
		PluginManager::Get()->EnableToolbars();

		// Check that the user has some paths set in the parser
		EditorConfigST::Get()->ReadObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);

		if ( m_tagsOptionsData.GetParserSearchPaths().IsEmpty() ) {
			// Try to locate the paths automatically
			wxArrayString paths;
			IncludePathLocator locator(PluginManager::Get());
			locator.Locate( paths );

			if ( paths.IsEmpty() == false ) {
				TagsParserSearchPathsDlg dlg(this, paths);
				if(dlg.ShowModal() == wxID_OK) {
					paths = dlg.GetSearchPaths();
				} else {
					paths.Empty();
				}
			}

			if ( paths.IsEmpty() ) {

				wxMessageBox(   wxT("CodeLite has detected that there are no search paths set for the parser\n")
								wxT("This means that CodeLite will *NOT* be able to offer any code completion\n")
								wxT("for non-workspace files (e.g. string.h).\n")
								wxT("To fix this, please set search paths for the parser\n")
								wxT("This can be done from the main menu: Settings > Tags Settings > Include Files"), _("CodeLite"), wxOK|wxCENTER|wxICON_INFORMATION, this);

			} else {
				m_tagsOptionsData.SetParserSearchPaths( paths );

				// Update the parser thread
				ParseThreadST::Get()->SetSearchPaths( paths, m_tagsOptionsData.GetParserExcludePaths() );
				EditorConfigST::Get()->WriteObject( wxT("m_tagsOptionsData"), &m_tagsOptionsData );
			}
		}

		//send initialization end event
		SendCmdEvent(wxEVT_INIT_DONE);
	}

	//clear navigation queue
	if (GetMainBook()->GetCurrentPage() == 0) {
		NavMgr::Get()->Clear();
	}
	event.Skip();
}

void Frame::OnFileCloseAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	GetMainBook()->CloseAll(true);
}

void Frame::OnQuickOutline(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (ManagerST::Get()->IsWorkspaceOpen() == false)
		return;

	if (!GetMainBook()->GetActiveEditor())
		return;

	if (GetMainBook()->GetActiveEditor()->GetProject().IsEmpty())
		return;

	QuickOutlineDlg *dlg = new QuickOutlineDlg(this, GetMainBook()->GetActiveEditor()->GetFileName().GetFullPath());
	if (dlg->ShowModal() == wxID_OK) {
	}
	dlg->Destroy();
#ifdef __WXMAC__
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->SetActive();
	}
#endif
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
			LEditor *editor = GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine());
			if (editor) {
				editor->FindAndSelect(tag->GetPattern(), tag->GetName());
			}
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
			Frame::Get()->GetMainBook()->OpenFile(fileName, projectName);
		}
	}
	dlg->Destroy();
}

void Frame::OnAddSymbols(SymbolTreeEvent &event)
{
	// Notify the plugins
	ParseThreadEventData data;
	data.SetFileName(event.GetFileName());
	data.SetItems(event.GetItems());

	SendCmdEvent(wxEVT_SYNBOL_TREE_ADD_ITEM, (void*)&data);
}

void Frame::OnDeleteSymbols(SymbolTreeEvent &event)
{
	// Notify the plugins
	ParseThreadEventData data;
	data.SetFileName(event.GetFileName());
	data.SetItems(event.GetItems());

	SendCmdEvent(wxEVT_SYNBOL_TREE_DELETE_ITEM, (void*)&data);

}

void Frame::OnUpdateSymbols(SymbolTreeEvent &event)
{
	// Notify the plugins
	ParseThreadEventData data;
	data.SetFileName(event.GetFileName());
	data.SetItems(event.GetItems());

	SendCmdEvent(wxEVT_SYNBOL_TREE_UPDATE_ITEM, (void*)&data);
}

wxString Frame::CreateWorkspaceTable()
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
		wxColour lineCol(0xd0, 0xff, 0xff);
		for (int i=(int)files.GetCount(); i>0; --i) {
			wxFileName fn( files.Item(i-1) );

			lineCol.Set(lineCol.Red() == 0xff ? 0xd0 : 0xff, 0xff, 0xff);
			lineCol = DrawingUtils::LightColour(lineCol, 1);
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

wxString Frame::CreateFilesTable()
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
		wxColour lineCol(0xd0, 0xff, 0xff);
		for (int i=(int)files.GetCount(); i>0; --i) {

			wxFileName fn( files.Item(i-1) );
			lineCol.Set(lineCol.Red() == 0xff ? 0xd0 : 0xff, 0xff, 0xff);

			lineCol = DrawingUtils::LightColour(lineCol, 1);
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

void Frame::CreateRecentlyOpenedFilesMenu()
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

void Frame::CreateRecentlyOpenedWorkspacesMenu()
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
				hs.AddFileToHistory(files.Item(i).BeforeLast(wxT('.')));
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
	FileHistory &fh = GetMainBook()->GetRecentlyOpenedFilesClass();

	wxArrayString files;
	fh.GetFiles(files);

	if (idx < files.GetCount()) {
		wxString projectName = ManagerST::Get()->GetProjectNameByFile(files.Item(idx));
		Frame::Get()->GetMainBook()->OpenFile(files.Item(idx), projectName);
	}
}

void Frame::OnRecentWorkspace(wxCommandEvent &event)
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
		ManagerST::Get()->OpenWorkspace( file_name );
	}
}

void Frame::OnBackwardForward(wxCommandEvent &event)
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

void Frame::CreateWelcomePage()
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

void Frame::OnImportMSVS(wxCommandEvent &e)
{
	wxUnusedVar(e);
	const wxString ALL(wxT("MS Visual Studio Solution File (*.sln)|*.sln|")
	                   wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open MS Solution File"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
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

void Frame::OnDebugUI(wxUpdateUIEvent &e)
{
	e.Enable( !ManagerST::Get()->IsBuildInProgress() );
}

void Frame::OnDebugRestart(wxCommandEvent &e)
{
	if (DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning()) {
		OnDebugStop(e);
	}
	OnDebug(e);
}

void Frame::OnDebugRestartUI(wxUpdateUIEvent &e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && dbgr->IsRunning());
}

void Frame::OnDebugStop(wxCommandEvent &e)
{
	wxUnusedVar(e);
	ManagerST::Get()->DbgStop();
}

void Frame::OnDebugStopUI(wxUpdateUIEvent &e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && dbgr->IsRunning());
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
	} else if (e.GetId() == XRCID("show_cursor")) {
		cmd = DBG_SHOW_CURSOR;
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
	        e.GetId() == XRCID("dbg_next") ||
	        e.GetId() == XRCID("show_cursor")) {
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		e.Enable(dbgr && dbgr->IsRunning());
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
	}
}

void Frame::OnLinkClicked(wxHtmlLinkEvent &e)
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

void Frame::OnStartPageEvent(wxCommandEvent& e)
{
	StartPageData *data = (StartPageData *)e.GetClientData();
	if ( data->action == wxT("switch-workspace" )) {
		ManagerST::Get()->OpenWorkspace(data->file_path);
	} else if ( data->action == wxT("open-file" )) {
		Frame::Get()->GetMainBook()->OpenFile(data->file_path, wxEmptyString);
	} else if ( data->action == wxT("create-workspace" )) {
		OnProjectNewWorkspace(e);
	} else if ( data->action == wxT("import-msvs-solution" )) {
		OnImportMSVS(e);
	} else if ( data->action == wxT("open-workspace" )) {
		OnSwitchWorkspace(e);
	}
	delete data;
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
	event.Enable(GetMainBook()->FindPage(wxT("Welcome!")) == NULL);
}

void Frame::OnShowWelcomePage(wxCommandEvent &event)
{
	ShowWelcomePage();
}

void Frame::CompleteInitialization()
{
	PluginManager::Get()->Load();

	// Add buttons to the OutputControlBarView
	m_controlBar->AddAllButtons();

	// Connect some system events
	m_mgr.Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(Frame::OnDockablePaneClosed), NULL, this);
	m_mgr.Connect(wxEVT_AUI_RENDER,     wxAuiManagerEventHandler(Frame::OnAuiManagerRender),   NULL, this);
}

void Frame::OnAppActivated(wxActivateEvent &e)
{
	if (m_theFrame && e.GetActive()) {
		m_theFrame->ReloadExternallyModifiedProjectFiles();
		m_theFrame->GetMainBook()->ReloadExternallyModified(true);
	}
	e.Skip();
}

void Frame::OnCompileFile(wxCommandEvent &e)
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

void Frame::OnCompileFileUI(wxUpdateUIEvent &e)
{
	e.Enable(false);
	Manager *mgr = ManagerST::Get();
	if (mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
		LEditor *editor = GetMainBook()->GetActiveEditor();
		if (editor && !editor->GetProject().IsEmpty()) {
			e.Enable(true);
		}
	}
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
	wxWindow *win = GetMainBook()->GetCurrentPage();
	if (win != NULL) {
		GetMainBook()->CloseAllButThis(win);
	}
}

WorkspaceTab *Frame::GetWorkspaceTab()
{
	return GetWorkspacePane()->GetWorkspaceTab();
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

void Frame::OnFileCloseUI(wxUpdateUIEvent &event)
{
	event.Enable(GetMainBook()->GetCurrentPage() != NULL);
}

void Frame::OnConvertEol(wxCommandEvent &e)
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
	GetMainBook()->SetViewEOL(visible);
}

void Frame::OnViewDisplayEOL_UI(wxUpdateUIEvent &e)
{
	LEditor *editor = GetMainBook()->GetActiveEditor();
	bool hasEditor = editor ? true : false;
	if (!hasEditor) {
		e.Enable(false);
		return;
	}

	e.Enable(true);
	e.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_EOL ? true : false);
}

void Frame::OnCopyFilePath(wxCommandEvent &event)
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
void Frame::OnCopyFilePathOnly(wxCommandEvent &event)
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
	PluginMgrDlg dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
#ifdef __WXMAC__
		wxMessageBox(_("Changes will take place after restart of CodeLite"), wxT("CodeLite"), wxICON_INFORMATION|wxOK, this);
#else
		// On Winodws & GTK we offer auto-restart
		int answer = wxMessageBox(_("Changes made requires restart of CodeLite\nWould you like to restart now?"), wxT("CodeLite"), wxICON_INFORMATION|wxYES_NO|wxCANCEL, this);
		if ( answer == wxYES ) {
			wxCommandEvent e(wxEVT_CMD_RESTART_CODELITE);
			ManagerST::Get()->AddPendingEvent(e);
		}
#endif
	}
}

void Frame::OnCppContextMenu(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		editor->GetContext()->ProcessEvent(e);
	}
}

void Frame::OnConfigureAccelerators(wxCommandEvent &e)
{
	AccelTableDlg *dlg = new AccelTableDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnUpdateBuildRefactorIndexBar(wxCommandEvent& e)
{
	wxUnusedVar(e);
}

void Frame::OnHighlightWord(wxCommandEvent& event)
{
	if (event.IsChecked()) {
		GetMainBook()->HighlightWord(true);
		EditorConfigST::Get()->SaveLongValue(wxT("highlight_word"), 1);
	} else {
		GetMainBook()->HighlightWord(false);
		EditorConfigST::Get()->SaveLongValue(wxT("highlight_word"), 0);
	}
}

void Frame::OnShowNavBar(wxCommandEvent& e)
{
	GetMainBook()->ShowNavBar(e.IsChecked());
}

void Frame::OnShowNavBarUI(wxUpdateUIEvent& e)
{
	e.Check(GetMainBook()->IsNavBarShown());
}

void Frame::OnParsingThreadDone(wxCommandEvent& e)
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

void Frame::OnSingleInstanceOpenFiles(wxCommandEvent& e)
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
				Frame::Get()->GetMainBook()->OpenFile(arr->Item(i), wxEmptyString);
			}
		}
		delete arr;
	}
	Raise();
}

void Frame::OnSingleInstanceRaise(wxCommandEvent& e)
{
	wxUnusedVar(e);
	Raise();
}

void Frame::OnNewVersionAvailable(wxCommandEvent& e)
{
	WebUpdateJobData *data = reinterpret_cast<WebUpdateJobData*>(e.GetClientData());
	if (data) {
		if (data->IsUpToDate() == false) {
			NewVersionDlg dlg(this);
			dlg.SetMessage(wxString::Format(wxT("A new version is available!\nCurrent version: rev%d\nNew version: rev%d\nWould you like CodeLite to take you to the download page?"), data->GetCurrentVersion(), data->GetNewVersion()));
			dlg.SetReleaseNotesURL(data->GetReleaseNotes());
			if (dlg.ShowModal() == wxID_OK) {
				wxString url = data->GetUrl();
				wxLaunchDefaultBrowser(url);
			}
		} else {
			wxLogMessage(wxString::Format(wxT("Info: CodeLite is up-to-date (or newer), version used: %d, version on site:%d"), data->GetCurrentVersion(), data->GetNewVersion()));
		}
		delete data;
	}
}

void Frame::OnDetachWorkspaceViewTab(wxCommandEvent& e)
{
	size_t sel = GetWorkspacePane()->GetNotebook()->GetSelection();
	CustomTab *t = GetWorkspacePane()->GetNotebook()->GetTabContainer()->IndexToTab(sel);
	wxString text = t->GetText();
	wxBitmap bmp = t->GetBmp();
	wxWindow *page = t->GetWindow();

	// remove the page from the notebook
	GetWorkspacePane()->GetNotebook()->RemovePage(sel, false);

	DockablePane *pane = new DockablePane(m_mainPanel, GetWorkspacePane()->GetNotebook(), page, text, bmp, wxSize(200, 200));
	wxUnusedVar(pane);
	wxUnusedVar(e);
}

void Frame::OnNewDetachedPane(wxCommandEvent &e)
{
	DockablePane *pane = (DockablePane*)(e.GetClientData());
	if (pane) {
		wxString text = pane->GetName();
		m_DPmenuMgr->AddMenu(text);

		m_mgr.AddPane(pane, wxAuiPaneInfo().Name(text).Caption(text));
		m_mgr.Update();
	}
}

void Frame::OnDestroyDetachedPane(wxCommandEvent& e)
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

void Frame::OnAuiManagerRender(wxAuiManagerEvent &e)
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

void Frame::OnDockablePaneClosed(wxAuiManagerEvent &e)
{
	DockablePane *pane = dynamic_cast<DockablePane*>(e.GetPane()->window);
	if (pane) {
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
		pane->ProcessEvent(evt);
	} else {
		e.Skip();
	}
}

void Frame::SetStatusMessage(const wxString &msg, int col, int id)
{
	wxUnusedVar(id);
	SetStatusText(msg, col);
}

void Frame::OnFunctionCalltipUI(wxUpdateUIEvent& event)
{
	LEditor* editor = GetMainBook()->GetActiveEditor();
	event.Enable(editor ? true : false);
}

void Frame::OnReloadWorkspace(wxCommandEvent& event)
{
	wxUnusedVar(event);

	// Save the current session before re-loading
	SaveLayoutAndSession();

	ManagerST::Get()->ReloadWorkspace();
}

void Frame::OnReloadWorkspaceUI(wxUpdateUIEvent& event)
{
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void Frame::RebuildProject(const wxString& projectName)
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

void Frame::OnBatchBuildUI(wxUpdateUIEvent& e)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && ManagerST::Get()->IsWorkspaceOpen();
	e.Enable(enable);
}

void Frame::OnBatchBuild(wxCommandEvent& e)
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

void Frame::SetFrameTitle(LEditor* editor)
{
	wxString title;
	if (editor && editor->GetModify()) {
		title << wxT("*");
	}

	LEditor *activeEditor = GetMainBook()->GetActiveEditor();
	if (editor && activeEditor == editor) {
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

void Frame::OnBuildWorkspace(wxCommandEvent& e)
{
	// start the build process
	wxUnusedVar(e);
	ManagerST::Get()->BuildWorkspace();
}

void Frame::OnBuildWorkspaceUI(wxUpdateUIEvent& e)
{
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
}

void Frame::OnDetachDebuggerViewTab(wxCommandEvent& e)
{
	size_t sel = GetDebuggerPane()->GetNotebook()->GetSelection();
	CustomTab *t = GetDebuggerPane()->GetNotebook()->GetTabContainer()->IndexToTab(sel);
	wxString text = t->GetText();
	wxBitmap bmp = t->GetBmp();
	wxWindow *page = t->GetWindow();

	// remove the page from the notebook
	GetDebuggerPane()->GetNotebook()->RemovePage(sel, false);

	DockablePane *pane = new DockablePane(m_mainPanel, GetDebuggerPane()->GetNotebook(), page, text, bmp, wxSize(200, 200));
	wxUnusedVar(pane);
	wxUnusedVar(e);
}

void Frame::OnCleanWorkspace(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ManagerST::Get()->CleanWorkspace();
}

void Frame::OnCleanWorkspaceUI(wxUpdateUIEvent& e)
{
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
}

void Frame::OnReBuildWorkspace(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ManagerST::Get()->RebuildWorkspace();
}

void Frame::OnReBuildWorkspaceUI(wxUpdateUIEvent& e)
{
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
}

void Frame::OnDetachTab(wxCommandEvent &e)
{
	wxUnusedVar(e);
	GetMainBook()->DetachPage(GetMainBook()->GetCurrentPage());
}

void Frame::OnDetachTabUI(wxUpdateUIEvent &e)
{
	e.Enable(!GetMainBook()->IsDetached(GetMainBook()->GetCurrentPage()));
}

void Frame::OnOpenShellFromFilePath(wxCommandEvent& e)
{
	// get the file path
	LEditor *editor = GetMainBook()->GetActiveEditor();
	if (editor) {
		wxString filepath = editor->GetFileName().GetPath();
		DirSaver ds;
		wxSetWorkingDirectory(filepath);
		if (!ProcUtils::Shell()) {
			wxLogMessage(wxString::Format(wxT("Failed to open shell at '%s'"), filepath.c_str()));
		}
	}
}

void Frame::ShowWelcomePage()
{
	wxWindow *win = GetMainBook()->FindPage(wxT("Welcome!"));
	if (win) {
		GetMainBook()->SelectPage(win);
	} else {
		CreateWelcomePage();
	}
}

void Frame::OnSyntaxHighlight(wxCommandEvent& e)
{
	SyntaxHighlightDlg *dlg = new SyntaxHighlightDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnQuickDebug(wxCommandEvent& e)
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
			dbgname = EnvironmentConfig::Instance()->ExpandVariables(dbgname);

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

			dbgr->Start(dbgname, exepath, wd, bpList, cmds);

			// notify plugins that the debugger just started
			SendCmdEvent(wxEVT_DEBUG_STARTED, &startup_info);

			dbgr->Run(dlg->GetArguments(), wxEmptyString);

			// Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
			// so that it uses debugger_ids instead of internal_ids
			Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

			// and finally make sure that the debugger pane is visiable
			ManagerST::Get()->ShowDebuggerPane();
		}
	}
	dlg->Destroy();
}

void Frame::OnQuickDebugUI(wxUpdateUIEvent& e)
{
	IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && !dbgr->IsRunning());
}

void Frame::OnShowWhitespaceUI(wxUpdateUIEvent& e)
{
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

void Frame::OnShowWhitespace(wxCommandEvent& e)
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

void Frame::OnIncrementalSearch(wxCommandEvent& event)
{
	wxUnusedVar( event );
	GetMainBook()->ShowQuickBar(true);
}

void Frame::OnRetagWorkspace(wxCommandEvent& event)
{
	wxUnusedVar( event );
	ManagerST::Get()->RetagWorkspace(event.GetId() == XRCID("retag_workspace") ? true : false );
}

void Frame::OnShowFullScreen(wxCommandEvent& e)
{
	wxUnusedVar(e);
	static std::set<wxString> s_toolbars;

	if (IsFullScreen()) {

		// show all toolbars that were hiddne due to fullscreen mode
		std::set<wxString>::iterator iter = s_toolbars.begin();
		for (; iter != s_toolbars.end(); iter++) {
			wxAuiPaneInfo &info = GetDockingManager().GetPane(*iter);
			if (info.IsOk() && info.IsShown() == false) {
				info.Show();
			}
		}

		// apply the changes
		GetDockingManager().Update();

		// clear the list
		s_toolbars.clear();

		ShowFullScreen(false);
	} else {
		// get list of all shown toolbars
		std::map<int, wxString>::iterator iter = m_toolbars.begin();
		for (; iter != m_toolbars.end(); iter++) {
			wxAuiPaneInfo &info = GetDockingManager().GetPane(iter->second);
			if (info.IsOk() && info.IsShown()) {
				s_toolbars.insert(iter->second);
				info.Hide();
			}
		}

		// apply the changes
		GetDockingManager().Update();
		ShowFullScreen(true);

		// Re-apply the menu accelerators
		ManagerST::Get()->UpdateMenuAccelerators();
	}
}

void Frame::OnSetStatusMessage(wxCommandEvent& e)
{
	wxString msg = e.GetString();
	int col = e.GetInt();
	SetStatusMessage(msg, col);
}

void Frame::OnReloadExternallModified(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ReloadExternallyModifiedProjectFiles();
	GetMainBook()->ReloadExternallyModified(true);
}

void Frame::OnReloadExternallModifiedNoPrompt(wxCommandEvent& e)
{
	wxUnusedVar(e);
	GetMainBook()->ReloadExternallyModified(false);
}

void Frame::ReloadExternallyModifiedProjectFiles()
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

	if (wxMessageBox(_("Workspace or project settings have been modified, would you like to reload the workspace and all contained projects?"), wxT("CodeLite"), wxICON_QUESTION|wxYES_NO, this) == wxYES) {
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
		AddPendingEvent( evt );
	}
}

void Frame::SaveLayoutAndSession()
{
	//save the perspective
	WriteFileUTF8(ManagerST::Get()->GetStarupDirectory() + wxT("/config/codelite.layout"), m_mgr.SavePerspective());
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
	wxArrayInt unused; GetMainBook()->SaveSession(session, unused);
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
	EditorConfigST::Get()->SaveLongValue(wxT("DebuggerBook"),  GetDebuggerPane()->GetNotebook()->GetBookStyle());
	EditorConfigST::Get()->SaveLongValue(wxT("OutputPane"),    GetOutputPane()->GetNotebook()->GetBookStyle());
	EditorConfigST::Get()->SaveLongValue(wxT("WorkspaceView"), GetWorkspacePane()->GetNotebook()->GetBookStyle());
	EditorConfigST::Get()->SaveLongValue(wxT("FindResults"),   GetOutputPane()->GetFindResultsTab()->GetBookStyle());

	EditorConfigST::Get()->Save();
}

void Frame::OnNextFiFMatch(wxCommandEvent& e)
{
	wxUnusedVar(e);
	GetOutputPane()->GetFindResultsTab()->NextMatch();
}

void Frame::OnPreviousFiFMatch(wxCommandEvent& e)
{
	wxUnusedVar(e);
	GetOutputPane()->GetFindResultsTab()->PrevMatch();
}

void Frame::OnNextFiFMatchUI(wxUpdateUIEvent& e)
{
	e.Enable(GetOutputPane()->GetFindResultsTab()->GetPageCount() > 0);
}

void Frame::OnPreviousFiFMatchUI(wxUpdateUIEvent& e)
{
	e.Enable(GetOutputPane()->GetFindResultsTab()->GetPageCount() > 0);
}

void Frame::OnShowQuickFinderUI(wxUpdateUIEvent& e)
{
//	// Determine the search type
//	wxString searchType;
//	wxString currentSearchType = EditorConfigST::Get()->GetStringValue(wxT("QuickFinderSearchType"));
//
//	if(e.GetId() == XRCID("quickfinder_class")) {
//		e.Check(ST_CLASS == currentSearchType);
//
//	} else if(e.GetId() == XRCID("quickfinder_function")) {
//		e.Check(ST_FUNCTION == currentSearchType);
//
//	} else if(e.GetId() == XRCID("quickfinder_macro")) {
//		e.Check(ST_MACRO == currentSearchType);
//
//	} else if(e.GetId() == XRCID("quickfinder_typedef")) {
//		e.Check(ST_TYPEDEF == currentSearchType);
//
//	} else {
//		e.Check(ST_WORKSPACE_FILE == currentSearchType);
//
//	}
}

void Frame::OnShowQuickFinder(wxCommandEvent& e)
{
	// the proper way of showing it, is by setting the configuration to true and
	// sending an event
	// Notify plugins about settings changed
	OptionsConfigPtr opts = EditorConfigST::Get()->GetOptions();
	opts->SetShowQuickFinder( true );
	EditorConfigST::Get()->SetOptions( opts );

	// Determine the search type
	wxString searchType;
	if(e.GetId() == XRCID("quickfinder_class")) {
		searchType = ST_CLASS;

	} else if(e.GetId() == XRCID("quickfinder_function")) {
		searchType = ST_FUNCTION;

	} else if(e.GetId() == XRCID("quickfinder_macro")) {
		searchType = ST_MACRO;

	} else if(e.GetId() == XRCID("quickfinder_typedef")) {
		searchType = ST_TYPEDEF;
	} else {
		searchType = ST_WORKSPACE_FILE;
	}

	// Set the search type
	EditorConfigST::Get()->SaveStringValue(wxT("QuickFinderSearchType"), searchType);
	PostCmdEvent( wxEVT_EDITOR_SETTINGS_CHANGED );
}

void Frame::OnParsingThreadMessage(wxCommandEvent& e)
{
	wxString *msg = (wxString*) e.GetClientData();
	if( msg ) {
		wxLogMessage( *msg );
		delete msg;
	}
}

// Due to differnet schema versions, the database was truncated by the
// TagsManager, prompt the user
void Frame::OnDatabaseUpgrade(wxCommandEvent& e)
{
	int answer = wxMessageBox(wxT("Your workspace symbols file does not match the current version of CodeLite. This can be fixed by re-tagging your workspace\nWould you like to re-tag your workspace now?"),
							  wxT("CodeLite"), wxYES_NO|wxICON_QUESTION|wxCENTER, this);
	if ( answer == wxYES ) {
		OnRetagWorkspace( e /* dummy */);
	}
}

void Frame::UpdateTagsOptions(const TagsOptionsData& tod)
{
	m_tagsOptionsData = tod;
	TagsManagerST::Get()->SetCtagsOptions( m_tagsOptionsData );
	EditorConfigST::Get()->WriteObject(wxT("m_tagsOptionsData"), &m_tagsOptionsData);
	ParseThreadST::Get()->SetSearchPaths( tod.GetParserSearchPaths(), tod.GetParserExcludePaths() );
}

void Frame::OnCheckForUpdate(wxCommandEvent& e)
{
	JobQueueSingleton::Instance()->PushJob( new WebUpdateJob(this) );
}

void Frame::OnShowActiveProjectSettings(wxCommandEvent& e)
{
	GetWorkspaceTab()->ProcessEvent( e );
}

void Frame::OnShowActiveProjectSettingsUI(wxUpdateUIEvent& e)
{
	wxArrayString projectList;
	WorkspaceST::Get()->GetProjectList( projectList );
	e.Enable(ManagerST::Get()->IsWorkspaceOpen() && (projectList.IsEmpty() == false));
}


void Frame::StartTimer()
{
	m_timer->Start(2500, true);
}

void Frame::OnLoadPerspective(wxCommandEvent& e)
{
	long loadIt(1);
	EditorConfigST::Get()->GetLongValue(wxT("LoadSavedPrespective"), loadIt);
	if (loadIt) {

		//locate the layout file
		wxString file_name(ManagerST::Get()->GetStarupDirectory() + wxT("/config/codelite.layout"));
		wxString pers(wxEmptyString);

		if (wxFileName(file_name).FileExists()) {
			//load this file
			ReadFileWithConversion(file_name, pers);
		}

		if ( pers.IsEmpty() == false && EditorConfigST::Get()->GetRevision() == SvnRevision) {
			m_mgr.LoadPerspective(pers);
		} else {
			EditorConfigST::Get()->SetRevision(SvnRevision);
		}
	}

	// Since of revision 3048, we need to manually force some of the changes
	// to the "Output View" pane, otherwise users wont be able to see them
	// unless they delete their codelite.layout file, which people usually
	// dont do
	GetDockingManager().GetPane(wxT("Output View")).Floatable(false);
	GetDockingManager().GetPane(wxT("Output View")).Dockable(false);
	GetDockingManager().GetPane(wxT("Output View")).CloseButton(false);
	GetDockingManager().GetPane(wxT("Output View")).CaptionVisible(false);
	GetDockingManager().GetPane(wxT("Output View")).Hide();
	GetDockingManager().Update();

	EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);

}
