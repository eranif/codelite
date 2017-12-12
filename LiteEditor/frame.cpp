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

#include "ColoursAndFontsManager.h"
#include "CompilersFoundDlg.h"
#include "NewProjectWizard.h"
#include "WelcomePage.h"
#include "app.h"
#include "autoversion.h"
#include "bitmap_loader.h"
#include "bookmark_manager.h"
#include "build_custom_targets_menu_manager.h"
#include "clBootstrapWizard.h"
#include "clGotoAnythingManager.h"
#include "clMainFrameHelper.h"
#include "clSingleChoiceDialog.h"
#include "clWorkspaceManager.h"
#include "cl_aui_dock_art.h"
#include "cl_aui_tb_are.h"
#include "cl_aui_tool_stickness.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "cl_standard_paths.h"
#include "cl_unredo.h"
#include "clang_compilation_db_thread.h"
#include "clsplashscreen.h"
#include "code_completion_manager.h"
#include "console_frame.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "includepathlocator.h"
#include "localstable.h"
#include "manage_perspective_dlg.h"
#include "my_menu_bar.h"
#include "open_resource_dialog.h" // New open resource
#include "precompiled_header.h"
#include "refactorengine.h"
#include "refactoring_storage.h"
#include "save_perspective_as_dlg.h"
#include "tags_parser_search_path_dlg.h"
#include "wxCodeCompletionBoxManager.h"
#include "wxCustomStatusBar.h"
#include <CompilersDetectorManager.h>
#include <wx/busyinfo.h>
#include <wx/richmsgdlg.h>
#include <wx/splash.h>
#include <wx/stc/stc.h>
#include <wx/wupdlock.h>
#include "theme_handler_helper.h"
#include "cl_defs.h"

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif
#include "Notebook.h"
#include "aboutdlg.h"
#include "acceltabledlg.h"
#include "advanced_settings.h"
#include "async_executable_cmd.h"
#include "batchbuilddlg.h"
#include "breakpointdlg.h"
#include "build_settings_config.h"
#include "builder.h"
#include "buildmanager.h"
#include "buildtabsettingsdata.h"
#include "cl_defs.h"
#include "clang_code_completion.h"
#include "configuration_manager_dlg.h"
#include "cpp_symbol_tree.h"
#include "debugcoredump.h"
#include "debuggerconfigtool.h"
#include "debuggermanager.h"
#include "debuggerpane.h"
#include "debuggersettingsdlg.h"
#include "detachedpanesinfo.h"
#include "dirsaver.h"
#include "dockablepane.h"
#include "dockablepanemenumanager.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "exelocator.h"
#include "filedroptarget.h"
#include "fileexplorer.h"
#include "fileview.h"
#include "findresultstab.h"
#include "frame.h"
#include "generalinfo.h"
#include "globals.h"
#include "imanager.h"
#include "jobqueue.h"
#include "language.h"
#include "macros.h"
#include "manager.h"
#include "menumanager.h"
#include "navigationmanager.h"
#include "new_build_tab.h"
#include "newversiondlg.h"
#include "newworkspacedlg.h"
#include "openwindowspanel.h"
#include "options_dlg2.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "pluginmgrdlg.h"
#include "project.h"
#include "quickdebugdlg.h"
#include "quickoutlinedlg.h"
#include "refactorindexbuildjob.h"
#include "replaceinfilespanel.h"
#include "search_thread.h"
#include "sessionmanager.h"
#include "singleinstancethreadjob.h"
#include "symbol_tree.h"
#include "syntaxhighlightdlg.h"
#include "tabgroupdlg.h"
#include "tabgroupmanager.h"
#include "tabgroupspane.h"
#include "webupdatethread.h"
#include "workspace_pane.h"
#include "workspacetab.h"
#include <wx/msgdlg.h>

//////////////////////////////////////////////////

// from auto-generated file svninfo.cpp:
static wxStopWatch gStopWatch;

// from iconsextra.cpp:
extern char* cubes_xpm[];
extern unsigned char cubes_alpha[];

static int FrameTimerId = wxNewId();

const wxEventType wxEVT_LOAD_PERSPECTIVE = XRCID("load_perspective");
const wxEventType wxEVT_REFRESH_PERSPECTIVE_MENU = XRCID("refresh_perspective_menu");
const wxEventType wxEVT_ACTIVATE_EDITOR = XRCID("activate_editor");
const wxEventType wxEVT_LOAD_SESSION = ::wxNewEventType();

#define CHECK_SHUTDOWN()                                         \
    {                                                            \
        if(ManagerST::Get()->IsShutdownInProgress()) { return; } \
    }
#ifdef __WXGTK__
#define FACTOR_1 0.0
#define FACTOR_2 0.0
#else
#define FACTOR_1 2.0
#define FACTOR_2 2.0
#endif

/**
 * @brief is the debugger running?
 */
static bool IsDebuggerRunning()
{
    clDebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent(eventIsRunning);
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    return (dbgr && dbgr->IsRunning()) || eventIsRunning.IsAnswer();
}

//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(clMainFrame, wxFrame)
// ATTN: Any item that appears in more than one menu is
//       only listed once here, the first time it occurs

//---------------------------------------------------
// System events
//---------------------------------------------------
EVT_IDLE(clMainFrame::OnIdle)
EVT_ACTIVATE(clMainFrame::OnAppActivated)
EVT_CLOSE(clMainFrame::OnClose)
EVT_TIMER(FrameTimerId, clMainFrame::OnTimer)
//  EVT_AUI_RENDER(Frame::OnAuiManagerRender)
//  EVT_AUI_PANE_CLOSE(Frame::OnDockablePaneClosed)

//---------------------------------------------------
// File menu
//---------------------------------------------------
EVT_MENU(XRCID("new_file"), clMainFrame::OnFileNew)
EVT_MENU(XRCID("open_file"), clMainFrame::OnFileOpen)
EVT_MENU(XRCID("open_folder"), clMainFrame::OnFileOpenFolder)
EVT_MENU(XRCID("refresh_file"), clMainFrame::OnFileReload)
EVT_MENU(XRCID("load_tab_group"), clMainFrame::OnFileLoadTabGroup)
EVT_MENU(XRCID("save_file"), clMainFrame::OnSave)
EVT_MENU(XRCID("duplicate_tab"), clMainFrame::OnDuplicateTab)
EVT_MENU(XRCID("save_file_as"), clMainFrame::OnSaveAs)
EVT_MENU(XRCID("save_all"), clMainFrame::OnFileSaveAll)
EVT_MENU(XRCID("save_tab_group"), clMainFrame::OnFileSaveTabGroup)
EVT_MENU(XRCID("close_file"), clMainFrame::OnFileClose)
EVT_MENU(wxID_CLOSE_ALL, clMainFrame::OnFileCloseAll)
EVT_MENU_RANGE(RecentFilesSubMenuID, RecentFilesSubMenuID + 10, clMainFrame::OnRecentFile)
EVT_MENU_RANGE(RecentWorkspaceSubMenuID, RecentWorkspaceSubMenuID + 10, clMainFrame::OnRecentWorkspace)
EVT_MENU_RANGE(ID_MENU_CUSTOM_TARGET_FIRST, ID_MENU_CUSTOM_TARGET_MAX, clMainFrame::OnBuildCustomTarget)
EVT_MENU(wxID_EXIT, clMainFrame::OnQuit)
// print
EVT_MENU(wxID_PRINT, clMainFrame::OnPrint)
EVT_UPDATE_UI(wxID_PRINT, clMainFrame::OnFileExistUpdateUI)
EVT_MENU(wxID_PAGE_SETUP, clMainFrame::OnPageSetup)

EVT_UPDATE_UI(XRCID("refresh_file"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("save_file"), clMainFrame::OnFileSaveUI)
EVT_UPDATE_UI(XRCID("duplicate_tab"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("save_file_as"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("save_all"), clMainFrame::OnFileSaveAllUI)
EVT_UPDATE_UI(XRCID("save_tab_group"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("close_file"), clMainFrame::OnFileCloseUI)
EVT_UPDATE_UI(XRCID("recent_workspaces"), clMainFrame::OnRecentWorkspaceUI)

//--------------------------------------------------
// Edit menu
//--------------------------------------------------
EVT_MENU(wxID_UNDO, clMainFrame::DispatchCommandEvent)
EVT_MENU(wxID_REDO, clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("label_current_state"), clMainFrame::DispatchCommandEvent)
EVT_MENU(wxID_DUPLICATE, clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("delete_line"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("delete_line_end"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("delete_line_start"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("copy_line"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("cut_line"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("transpose_lines"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("trim_trailing"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("to_upper"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("to_lower"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("match_brace"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("select_to_brace"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("complete_word"), clMainFrame::OnCodeComplete)
EVT_MENU(XRCID("simple_word_completion"), clMainFrame::OnWordComplete)
EVT_MENU(XRCID("complete_word_refresh_list"), clMainFrame::OnCompleteWordRefreshList)
EVT_MENU(XRCID("function_call_tip"), clMainFrame::OnFunctionCalltip)
EVT_MENU(XRCID("convert_eol_win"), clMainFrame::OnConvertEol)
EVT_MENU(XRCID("convert_eol_unix"), clMainFrame::OnConvertEol)
EVT_MENU(XRCID("convert_eol_mac"), clMainFrame::OnConvertEol)
EVT_MENU(XRCID("move_line_down"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("move_line_up"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("center_line"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("center_line_roll"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("selection_to_multi_caret"), clMainFrame::OnSplitSelection)
EVT_MENU(XRCID("convert_indent_to_tabs"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("convert_indent_to_spaces"), clMainFrame::DispatchCommandEvent)
EVT_UPDATE_UI(XRCID("selection_to_multi_caret"), clMainFrame::OnSplitSelectionUI)

EVT_UPDATE_UI(wxID_UNDO, clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(wxID_REDO, clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(wxID_DUPLICATE, clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("delete_line"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("delete_line_end"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("delete_line_start"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("copy_line"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("cut_line"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("transpose_lines"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("trim_trailing"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("to_upper"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("to_lower"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("match_brace"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("select_to_brace"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("complete_word"), clMainFrame::OnCompleteWordUpdateUI)
EVT_UPDATE_UI(XRCID("simple_word_completion"), clMainFrame::OnCompleteWordUpdateUI)
EVT_UPDATE_UI(XRCID("function_call_tip"), clMainFrame::OnFunctionCalltipUI)
EVT_UPDATE_UI(XRCID("convert_eol_win"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("convert_eol_unix"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("convert_eol_mac"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("move_line_down"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("move_line_up"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("center_line_roll"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("convert_indent_to_tabs"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("convert_indent_to_spaces"), clMainFrame::OnFileExistUpdateUI)

//-------------------------------------------------------
// View menu
//-------------------------------------------------------
EVT_MENU(XRCID("restore_layout"), clMainFrame::OnRestoreDefaultLayout)
EVT_MENU(XRCID("word_wrap"), clMainFrame::OnViewWordWrap)
EVT_MENU(XRCID("toggle_fold"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("fold_all"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("fold_all_in_selection"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("fold_topmost_in_selection"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("display_eol"), clMainFrame::OnViewDisplayEOL)
EVT_MENU(XRCID("whitepsace_invisible"), clMainFrame::OnShowWhitespace)
EVT_MENU(XRCID("whitepsace_always"), clMainFrame::OnShowWhitespace)
EVT_MENU(XRCID("whitespace_visiable_after_indent"), clMainFrame::OnShowWhitespace)
EVT_MENU(XRCID("whitespace_indent_only"), clMainFrame::OnShowWhitespace)
EVT_MENU(XRCID("next_tab"), clMainFrame::OnNextTab)
EVT_MENU(XRCID("prev_tab"), clMainFrame::OnPrevTab)
EVT_MENU(XRCID("full_screen"), clMainFrame::OnShowFullScreen)
EVT_MENU(XRCID("view_welcome_page"), clMainFrame::OnShowWelcomePage)
EVT_MENU(XRCID("view_welcome_page_at_startup"), clMainFrame::OnLoadWelcomePage)
EVT_MENU(XRCID("show_nav_toolbar"), clMainFrame::OnShowNavBar)
EVT_MENU(XRCID("toogle_main_toolbars"), clMainFrame::OnToggleMainTBars)
EVT_MENU(XRCID("toogle_plugin_toolbars"), clMainFrame::OnTogglePluginTBars)
EVT_MENU(XRCID("toggle_panes"), clMainFrame::OnTogglePanes)
EVT_MENU(XRCID("distraction_free_mode"), clMainFrame::OnToggleMinimalView)
EVT_UPDATE_UI(XRCID("distraction_free_mode"), clMainFrame::OnToggleMinimalViewUI)
EVT_MENU(XRCID("hide_status_bar"), clMainFrame::OnShowStatusBar)
EVT_UPDATE_UI(XRCID("hide_status_bar"), clMainFrame::OnShowStatusBarUI)
EVT_MENU(XRCID("hide_tool_bar"), clMainFrame::OnShowToolbar)
EVT_UPDATE_UI(XRCID("hide_tool_bar"), clMainFrame::OnShowToolbarUI)
EVT_MENU(XRCID("show_tab_bar"), clMainFrame::OnShowTabBar)
EVT_UPDATE_UI(XRCID("show_tab_bar"), clMainFrame::OnShowTabBarUI)
EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, clMainFrame::DispatchCommandEvent)

EVT_UPDATE_UI(XRCID("word_wrap"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("toggle_fold"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("fold_all"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("fold_all_in_selection"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("fold_topmost_in_selection"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("display_eol"), clMainFrame::OnViewDisplayEOL_UI)
EVT_UPDATE_UI(XRCID("next_tab"), clMainFrame::OnNextPrevTab_UI)
EVT_UPDATE_UI(XRCID("prev_tab"), clMainFrame::OnNextPrevTab_UI)
EVT_UPDATE_UI(XRCID("whitepsace_invisible"), clMainFrame::OnShowWhitespaceUI)
EVT_UPDATE_UI(XRCID("whitepsace_always"), clMainFrame::OnShowWhitespaceUI)
EVT_UPDATE_UI(XRCID("whitespace_visiable_after_indent"), clMainFrame::OnShowWhitespaceUI)
EVT_UPDATE_UI(XRCID("whitespace_indent_only"), clMainFrame::OnShowWhitespaceUI)
EVT_UPDATE_UI(XRCID("view_welcome_page"), clMainFrame::OnShowWelcomePageUI)
EVT_UPDATE_UI(XRCID("view_welcome_page_at_startup"), clMainFrame::OnLoadWelcomePageUI)
EVT_UPDATE_UI(XRCID("show_nav_toolbar"), clMainFrame::OnShowNavBarUI)
EVT_UPDATE_UI(viewAsSubMenuID, clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, clMainFrame::DispatchUpdateUIEvent)

//-------------------------------------------------------
// Search menu
//-------------------------------------------------------
EVT_MENU(wxID_FIND, clMainFrame::DispatchCommandEvent)
EVT_MENU(wxID_REPLACE, clMainFrame::OnIncrementalReplace)
EVT_MENU(XRCID("find_resource"), clMainFrame::OnFindResourceXXX)
EVT_MENU(XRCID("incremental_search"), clMainFrame::OnIncrementalSearch)
EVT_MENU(XRCID("incremental_replace"), clMainFrame::OnIncrementalReplace)
EVT_MENU(XRCID("find_symbol"), clMainFrame::OnQuickOutline)
EVT_MENU(XRCID("goto_definition"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("goto_previous_definition"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("goto_linenumber"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("toggle_bookmark"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("next_bookmark"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("previous_bookmark"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("removeall_current_bookmarks"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("removeall_bookmarks"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("next_fif_match"), clMainFrame::OnNextFiFMatch)
EVT_MENU(XRCID("previous_fif_match"), clMainFrame::OnPreviousFiFMatch)
EVT_MENU(XRCID("grep_current_file"), clMainFrame::OnGrepWord)
EVT_MENU(XRCID("grep_current_workspace"), clMainFrame::OnGrepWord)
EVT_MENU(XRCID("ID_QUICK_ADD_NEXT"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("ID_QUICK_FIND_ALL"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("ID_GOTO_ANYTHING"), clMainFrame::OnGotoAnything)
EVT_UPDATE_UI(XRCID("ID_QUICK_ADD_NEXT"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("ID_QUICK_FIND_ALL"), clMainFrame::OnFileExistUpdateUI)

EVT_AUITOOLBAR_TOOL_DROPDOWN(XRCID("toggle_bookmark"), clMainFrame::OnShowBookmarkMenu)

EVT_UPDATE_UI(wxID_FIND, clMainFrame::OnIncrementalSearchUI)
EVT_UPDATE_UI(wxID_REPLACE, clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("select_previous"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("select_next"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("find_next"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("find_previous"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("find_next_at_caret"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("find_previous_at_caret"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("incremental_search"), clMainFrame::OnIncrementalSearchUI)
EVT_UPDATE_UI(XRCID("find_resource"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("find_type"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("find_function"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("find_macro"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("find_typedef"), clMainFrame::OnWorkspaceOpen)

EVT_UPDATE_UI(XRCID("find_symbol"), clMainFrame::OnCompleteWordUpdateUI)
EVT_UPDATE_UI(XRCID("goto_definition"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("goto_previous_definition"), clMainFrame::DispatchUpdateUIEvent)
EVT_UPDATE_UI(XRCID("goto_linenumber"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("toggle_bookmark"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("next_bookmark"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("previous_bookmark"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("removeall_bookmarks"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("next_fif_match"), clMainFrame::OnNextFiFMatchUI)
EVT_UPDATE_UI(XRCID("previous_fif_match"), clMainFrame::OnPreviousFiFMatchUI)
EVT_UPDATE_UI(XRCID("grep_current_file"), clMainFrame::OnGrepWordUI)
EVT_UPDATE_UI(XRCID("grep_current_workspace"), clMainFrame::OnGrepWordUI)

//-------------------------------------------------------
// Project menu
//-------------------------------------------------------
EVT_MENU(XRCID("local_workspace_prefs"), clMainFrame::OnWorkspaceEditorPreferences)
EVT_MENU(XRCID("local_workspace_settings"), clMainFrame::OnWorkspaceSettings)
EVT_MENU(XRCID("new_workspace"), clMainFrame::OnProjectNewWorkspace)
EVT_MENU(XRCID("file_new_workspace"), clMainFrame::OnProjectNewWorkspace)
EVT_UPDATE_UI(XRCID("file_new_workspace"), clMainFrame::OnNewWorkspaceUI)
EVT_UPDATE_UI(XRCID("new_workspace"), clMainFrame::OnNewWorkspaceUI)
EVT_UPDATE_UI(XRCID("import_from_msvs"), clMainFrame::OnNewWorkspaceUI)
EVT_MENU(XRCID("switch_to_workspace"), clMainFrame::OnSwitchWorkspace)
EVT_MENU(XRCID("file_switch_to_workspace"), clMainFrame::OnSwitchWorkspace)
EVT_UPDATE_UI(XRCID("switch_to_workspace"), clMainFrame::OnSwitchWorkspaceUI)
EVT_UPDATE_UI(XRCID("file_switch_to_workspace"), clMainFrame::OnSwitchWorkspaceUI)
EVT_MENU(XRCID("close_workspace"), clMainFrame::OnCloseWorkspace)
EVT_MENU(XRCID("file_close_workspace"), clMainFrame::OnCloseWorkspace)
EVT_MENU(XRCID("reload_workspace"), clMainFrame::OnReloadWorkspace)
EVT_MENU(XRCID("import_from_msvs"), clMainFrame::OnImportMSVS)
EVT_MENU(XRCID("new_project"), clMainFrame::OnProjectNewProject)
EVT_MENU(XRCID("file_new_project"), clMainFrame::OnProjectNewProject)
EVT_MENU(XRCID("add_project"), clMainFrame::OnProjectAddProject)
EVT_MENU(XRCID("reconcile_project"), clMainFrame::OnReconcileProject)
EVT_MENU(XRCID("retag_workspace"), clMainFrame::OnRetagWorkspace)
EVT_MENU(XRCID("full_retag_workspace"), clMainFrame::OnRetagWorkspace)
EVT_MENU(XRCID("project_properties"), clMainFrame::OnShowActiveProjectSettings)

EVT_UPDATE_UI(XRCID("local_workspace_prefs"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("local_workspace_settings"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("close_workspace"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("file_close_workspace"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("reload_workspace"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("add_project"), clMainFrame::OnWorkspaceMenuUI)
EVT_UPDATE_UI(XRCID("file_new_project"), clMainFrame::OnWorkspaceOpen)
EVT_UPDATE_UI(XRCID("new_project"), clMainFrame::OnNewProjectUI)
EVT_UPDATE_UI(XRCID("reconcile_project"), clMainFrame::OnShowActiveProjectSettingsUI)
EVT_UPDATE_UI(XRCID("retag_workspace"), clMainFrame::OnRetagWorkspaceUI)
EVT_UPDATE_UI(XRCID("full_retag_workspace"), clMainFrame::OnRetagWorkspaceUI)
EVT_UPDATE_UI(XRCID("project_properties"), clMainFrame::OnShowActiveProjectSettingsUI)

//-------------------------------------------------------
// Build menu
//-------------------------------------------------------
EVT_MENU(XRCID("execute_no_debug"), clMainFrame::OnExecuteNoDebug)
EVT_MENU(XRCID("stop_executed_program"), clMainFrame::OnStopExecutedProgram)
EVT_MENU(XRCID("build_active_project"), clMainFrame::OnBuildProject)
EVT_MENU(XRCID("build_active_project_only"), clMainFrame::OnBuildProjectOnly)
EVT_AUITOOLBAR_TOOL_DROPDOWN(XRCID("build_active_project"), clMainFrame::OnShowAuiBuildMenu)
EVT_MENU(XRCID("compile_active_file"), clMainFrame::OnCompileFile)
EVT_MENU(XRCID("clean_active_project"), clMainFrame::OnCleanProject)
EVT_MENU(XRCID("clean_active_project_only"), clMainFrame::OnCleanProjectOnly)
EVT_MENU(XRCID("stop_active_project_build"), clMainFrame::OnStopBuild)
EVT_MENU(XRCID("rebuild_active_project"), clMainFrame::OnRebuildProject)
EVT_MENU(XRCID("build_n_run_active_project"), clMainFrame::OnBuildAndRunProject)
EVT_MENU(XRCID("build_workspace"), clMainFrame::OnBuildWorkspace)
EVT_MENU(XRCID("clean_workspace"), clMainFrame::OnCleanWorkspace)
EVT_MENU(XRCID("rebuild_workspace"), clMainFrame::OnReBuildWorkspace)
EVT_MENU(XRCID("batch_build"), clMainFrame::OnBatchBuild)

EVT_UPDATE_UI(XRCID("execute_no_debug"), clMainFrame::OnExecuteNoDebugUI)
EVT_UPDATE_UI(XRCID("stop_executed_program"), clMainFrame::OnStopExecutedProgramUI)
EVT_UPDATE_UI(XRCID("build_active_project"), clMainFrame::OnBuildProjectUI)
EVT_UPDATE_UI(XRCID("compile_active_file"), clMainFrame::OnCompileFileUI)
EVT_UPDATE_UI(XRCID("clean_active_project"), clMainFrame::OnCleanProjectUI)
EVT_UPDATE_UI(XRCID("stop_active_project_build"), clMainFrame::OnStopBuildUI)
EVT_UPDATE_UI(XRCID("rebuild_active_project"), clMainFrame::OnBuildProjectUI)
EVT_UPDATE_UI(XRCID("build_n_run_active_project"), clMainFrame::OnBuildProjectUI)
EVT_UPDATE_UI(XRCID("build_workspace"), clMainFrame::OnBuildWorkspaceUI)
EVT_UPDATE_UI(XRCID("clean_workspace"), clMainFrame::OnCleanWorkspaceUI)
EVT_UPDATE_UI(XRCID("rebuild_workspace"), clMainFrame::OnReBuildWorkspaceUI)
EVT_UPDATE_UI(XRCID("batch_build"), clMainFrame::OnBatchBuildUI)

//-------------------------------------------------------
// Debug menu
//-------------------------------------------------------
EVT_MENU(XRCID("debugger_win_locals"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_watches"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_output"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_threads"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_callstack"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_memory"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_breakpoints"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_asciiview"), clMainFrame::OnShowDebuggerWindow)
EVT_MENU(XRCID("debugger_win_disassemble"), clMainFrame::OnShowDebuggerWindow)
EVT_UPDATE_UI(XRCID("debugger_win_locals"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_watches"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_output"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_threads"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_callstack"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_memory"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_breakpoints"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_asciiview"), clMainFrame::OnShowDebuggerWindowUI)
EVT_UPDATE_UI(XRCID("debugger_win_disassemble"), clMainFrame::OnShowDebuggerWindowUI)
EVT_MENU(XRCID("start_debugger"), clMainFrame::OnDebug)
EVT_MENU(XRCID("restart_debugger"), clMainFrame::OnDebugRestart)
EVT_MENU(XRCID("attach_debugger"), clMainFrame::OnDebugAttach)
EVT_MENU(XRCID("pause_debugger"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("stop_debugger"), clMainFrame::OnDebugStop)
EVT_MENU(XRCID("dbg_run_to_cursor"), clMainFrame::OnDebugRunToCursor)
EVT_MENU(XRCID("dbg_jump_cursor"), clMainFrame::OnDebugJumpToCursor)
EVT_MENU(XRCID("dbg_stepin"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("dbg_stepi"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("dbg_stepout"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("dbg_enable_reverse_debug"), clMainFrame::OnToggleReverseDebugging)
EVT_MENU(XRCID("dbg_start_recording"), clMainFrame::OnToggleReverseDebuggingRecording)
EVT_MENU(XRCID("dbg_next"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("dbg_nexti"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("show_cursor"), clMainFrame::OnDebugCmd)
EVT_MENU(XRCID("add_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("insert_breakpoint"), clMainFrame::DispatchCommandEvent) // Toggles
EVT_MENU(XRCID("disable_all_breakpoints"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("enable_all_breakpoints"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("delete_all_breakpoints"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("insert_temp_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("insert_disabled_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("insert_cond_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("edit_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("show_breakpoint_dlg"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("insert_watchpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("toggle_breakpoint_enabled_status"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("ignore_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("delete_breakpoint"), clMainFrame::DispatchCommandEvent)
EVT_MENU(XRCID("quick_debug"), clMainFrame::OnQuickDebug)
EVT_MENU(XRCID("debug_core_dump"), clMainFrame::OnDebugCoreDump)

EVT_UPDATE_UI(XRCID("start_debugger"), clMainFrame::OnDebugUI)
EVT_UPDATE_UI(XRCID("restart_debugger"), clMainFrame::OnDebugRestartUI)
EVT_UPDATE_UI(XRCID("stop_debugger"), clMainFrame::OnDebugStopUI)
EVT_UPDATE_UI(XRCID("dbg_stepin"), clMainFrame::OnDebugCmdUI)
EVT_UPDATE_UI(XRCID("dbg_stepi"), clMainFrame::OnDebugCmdUI)
EVT_UPDATE_UI(XRCID("dbg_stepout"), clMainFrame::OnDebugCmdUI)
EVT_UPDATE_UI(XRCID("dbg_next"), clMainFrame::OnDebugCmdUI)

EVT_UPDATE_UI(XRCID("dbg_enable_reverse_debug"), clMainFrame::OnToggleReverseDebuggingUI)
EVT_UPDATE_UI(XRCID("dbg_start_recording"), clMainFrame::OnToggleReverseDebuggingRecordingUI)
EVT_UPDATE_UI(XRCID("pause_debugger"), clMainFrame::OnDebugInterruptUI)
EVT_UPDATE_UI(XRCID("dbg_nexti"), clMainFrame::OnDebugStepInstUI)
EVT_UPDATE_UI(XRCID("show_cursor"), clMainFrame::OnDebugShowCursorUI)
EVT_UPDATE_UI(XRCID("dbg_run_to_cursor"), clMainFrame::OnDebugRunToCursorUI)
EVT_UPDATE_UI(XRCID("dbg_jump_cursor"), clMainFrame::OnDebugJumpToCursorUI)

EVT_UPDATE_UI(XRCID("insert_breakpoint"), clMainFrame::OnDebugManageBreakpointsUI)
EVT_UPDATE_UI(XRCID("disable_all_breakpoints"), clMainFrame::OnDebugManageBreakpointsUI)
EVT_UPDATE_UI(XRCID("enable_all_breakpoints"), clMainFrame::OnDebugManageBreakpointsUI)
EVT_UPDATE_UI(XRCID("delete_all_breakpoints"), clMainFrame::OnDebugManageBreakpointsUI)
EVT_UPDATE_UI(XRCID("quick_debug"), clMainFrame::OnQuickDebugUI)
EVT_UPDATE_UI(XRCID("debug_core_dump"), clMainFrame::OnQuickDebugUI)

//-------------------------------------------------------
// Plugins menu
//-------------------------------------------------------
EVT_MENU(XRCID("manage_plugins"), clMainFrame::OnManagePlugins)

//-------------------------------------------------------
// Settings menu
//-------------------------------------------------------
EVT_MENU(wxID_PREFERENCES, clMainFrame::OnViewOptions)
EVT_MENU(XRCID("syntax_highlight"), clMainFrame::OnSyntaxHighlight)
EVT_MENU(XRCID("configure_accelerators"), clMainFrame::OnConfigureAccelerators)
EVT_MENU(XRCID("add_envvar"), clMainFrame::OnAddEnvironmentVariable)
EVT_MENU(XRCID("advance_settings"), clMainFrame::OnAdvanceSettings)
EVT_MENU(XRCID("debuger_settings"), clMainFrame::OnDebuggerSettings)
EVT_MENU(XRCID("tags_options"), clMainFrame::OnCtagsOptions)

//-------------------------------------------------------
// Help menu
//-------------------------------------------------------
EVT_MENU(wxID_ABOUT, clMainFrame::OnAbout)
EVT_MENU(XRCID("check_for_update"), clMainFrame::OnCheckForUpdate)
EVT_MENU(XRCID("run_setup_wizard"), clMainFrame::OnRunSetupWizard)

//-------------------------------------------------------
// Perspective menu
//-------------------------------------------------------
EVT_MENU_RANGE(PERSPECTIVE_FIRST_MENU_ID, PERSPECTIVE_LAST_MENU_ID, clMainFrame::OnChangePerspective)
EVT_UPDATE_UI_RANGE(PERSPECTIVE_FIRST_MENU_ID, PERSPECTIVE_LAST_MENU_ID, clMainFrame::OnChangePerspectiveUI)
EVT_MENU(XRCID("manage_perspectives"), clMainFrame::OnManagePerspectives)
EVT_MENU(XRCID("save_current_layout"), clMainFrame::OnSaveLayoutAsPerspective)

//-----------------------------------------------------------------
// Toolbar
//-----------------------------------------------------------------
EVT_MENU(wxID_FORWARD, clMainFrame::OnBackwardForward)
EVT_MENU(wxID_BACKWARD, clMainFrame::OnBackwardForward)
EVT_MENU(XRCID("highlight_word"), clMainFrame::OnHighlightWord)

EVT_UPDATE_UI(wxID_FORWARD, clMainFrame::OnBackwardForwardUI)
EVT_UPDATE_UI(wxID_BACKWARD, clMainFrame::OnBackwardForwardUI)

//-------------------------------------------------------
// Workspace Pane tab context menu
//-------------------------------------------------------
EVT_MENU(XRCID("detach_wv_tab"), clMainFrame::OnDetachWorkspaceViewTab)
EVT_MENU(XRCID("hide_wv_tab"), clMainFrame::OnHideWorkspaceViewTab)

//-------------------------------------------------------
// Debugger Pane tab context menu
//-------------------------------------------------------
EVT_MENU(XRCID("detach_debugger_tab"), clMainFrame::OnDetachDebuggerViewTab)

//-------------------------------------------------------
// Editor tab context menu
//-------------------------------------------------------
EVT_MENU(XRCID("close_other_tabs"), clMainFrame::OnCloseAllButThis)
EVT_MENU(XRCID("close_tabs_to_the_right"), clMainFrame::OnCloseTabsToTheRight)
EVT_MENU(XRCID("copy_file_relative_path_to_workspace"), clMainFrame::OnCopyFilePathRelativeToWorkspace)
EVT_MENU(XRCID("copy_file_name"), clMainFrame::OnCopyFilePath)
EVT_MENU(XRCID("copy_file_path"), clMainFrame::OnCopyFilePathOnly)
EVT_MENU(XRCID("copy_file_name_only"), clMainFrame::OnCopyFileName)
EVT_MENU(XRCID("open_shell_from_filepath"), clMainFrame::OnOpenShellFromFilePath)
EVT_MENU(XRCID("open_file_explorer"), clMainFrame::OnOpenFileExplorerFromFilePath)
EVT_MENU(XRCID("ID_DETACH_EDITOR"), clMainFrame::OnDetachEditor)
EVT_MENU(XRCID("mark_readonly"), clMainFrame::OnMarkEditorReadonly)
EVT_UPDATE_UI(XRCID("mark_readonly"), clMainFrame::OnMarkEditorReadonlyUI)
EVT_UPDATE_UI(XRCID("copy_file_relative_path_to_workspace"), clMainFrame::OnCopyFilePathRelativeToWorkspaceUI)
EVT_UPDATE_UI(XRCID("copy_file_name"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("copy_file_path"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("open_shell_from_filepath"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("open_file_explorer"), clMainFrame::OnFileExistUpdateUI)
EVT_UPDATE_UI(XRCID("ID_DETACH_EDITOR"), clMainFrame::OnDetachEditorUI)

//-----------------------------------------------------------------
// Default editor context menu
//-----------------------------------------------------------------
EVT_MENU(wxID_DELETE, clMainFrame::DispatchCommandEvent)
EVT_UPDATE_UI(wxID_DELETE, clMainFrame::DispatchUpdateUIEvent)

//-----------------------------------------------------------------
// C++ editor context menu
//-----------------------------------------------------------------
EVT_MENU(XRCID("open_include_file"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("add_include_file"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("add_forward_decl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("swap_files"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("find_decl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("find_impl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("go_to_function_start"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("go_to_next_function"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("insert_doxy_comment"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("setters_getters"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("move_impl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("add_impl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("sync_signatures"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("add_multi_impl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("add_virtual_impl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("add_pure_virtual_impl"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("rename_symbol"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("find_references"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("comment_selection"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("comment_line"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("retag_file"), clMainFrame::OnCppContextMenu)
EVT_MENU(XRCID("rename_local_variable"), clMainFrame::OnCppContextMenu)

//-----------------------------------------------------------------
// Hyperlinks
//-----------------------------------------------------------------
EVT_HTML_LINK_CLICKED(wxID_ANY, clMainFrame::OnLinkClicked)
EVT_MENU(XRCID("link_action"), clMainFrame::OnStartPageEvent)

//-----------------------------------------------------------------
// CodeLite-specific events
//-----------------------------------------------------------------
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_MESSAGE, clMainFrame::OnParsingThreadMessage)
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE, clMainFrame::OnClearTagsCache)
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_COMPLETED, clMainFrame::OnRetaggingCompelted)
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS, clMainFrame::OnRetaggingProgress)
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_READY, clMainFrame::OnParserThreadReady)

EVT_COMMAND(wxID_ANY, wxEVT_ACTIVATE_EDITOR, clMainFrame::OnActivateEditor)

EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE, clMainFrame::OnDatabaseUpgrade)
EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE_INTER, clMainFrame::OnDatabaseUpgradeInternally)
EVT_COMMAND(wxID_ANY, wxEVT_REFRESH_PERSPECTIVE_MENU, clMainFrame::OnRefreshPerspectiveMenu)
EVT_MENU(XRCID("update_num_builders_count"), clMainFrame::OnUpdateNumberOfBuildProcesses)
EVT_MENU(XRCID("goto_codelite_download_url"), clMainFrame::OnGotoCodeLiteDownloadPage)

EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_VERSION_AVAILABLE, clMainFrame::OnNewVersionAvailable)
EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_UPTODATE, clMainFrame::OnNewVersionAvailable)
EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_CHECK_ERROR, clMainFrame::OnVersionCheckError)

EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_DOCKPANE, clMainFrame::OnNewDetachedPane)
EVT_COMMAND(wxID_ANY, wxEVT_LOAD_PERSPECTIVE, clMainFrame::OnLoadPerspective)
EVT_COMMAND(wxID_ANY, wxEVT_CMD_DELETE_DOCKPANE, clMainFrame::OnDestroyDetachedPane)
END_EVENT_TABLE()

clMainFrame* clMainFrame::m_theFrame = NULL;
bool clMainFrame::m_initCompleted = false;

clMainFrame::clMainFrame(wxWindow* pParent, wxWindowID id, const wxString& title, const wxPoint& pos,
                         const wxSize& size, long style)
    : wxFrame(pParent, id, title, pos, size, style)
    , m_buildAndRun(false)
    , m_cppMenu(NULL)
    , m_highlightWord(false)
    , m_workspaceRetagIsRequired(false)
    , m_bookmarksDropDownMenu(NULL)
    , m_singleInstanceThread(NULL)
    , m_mainToolBar(NULL)
#ifdef __WXGTK__
    , m_isWaylandSession(false)
#endif
    , m_webUpdate(NULL)
{
#if defined(__WXGTK20__)
    // A rather ugly hack here.  GTK V2 insists that F10 should be the
    // accelerator for the menu bar.  We don't want that.  There is
    // no sane way to turn this off, but we *can* get the same effect
    // by setting the "menu bar accelerator" property to the name of a
    // function key that is apparently legal, but doesn't really exist.
    // (Or if it does, it certainly isn't a key we use.)
    gtk_settings_set_string_property(gtk_settings_get_default(), "gtk-menu-bar-accel", "F15", "foo");

#endif
    // Pass the docking manager to the plugin-manager
    PluginManager::Get()->SetDockingManager(&m_mgr);

    long value = EditorConfigST::Get()->GetInteger(wxT("highlight_word"), 0);
    m_highlightWord = (bool)value;

    // Initialize the frame helper
    m_frameHelper.Reset(new clMainFrameHelper(this, &m_mgr));
    CreateGUIControls();

    ManagerST::Get();              // Dummy call
    RefactoringEngine::Instance(); // Dummy call

    // allow the main frame to receive files by drag and drop
    SetDropTarget(new FileDropTarget());

    // Start the search thread
    SearchThreadST::Get()->SetNotifyWindow(this);
    SearchThreadST::Get()->Start(WXTHREAD_MIN_PRIORITY);

    // start the job queue
    JobQueueSingleton::Instance()->Start(6);

    // Create the single instance thread
    m_singleInstanceThread = new clSingleInstanceThread();
    m_singleInstanceThread->Start();

    // start the editor creator thread
    m_timer = new wxTimer(this, FrameTimerId);

    // connect common edit events
    wxTheApp->Connect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent),
                      NULL, this);
    wxTheApp->Connect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent),
                      NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Connect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent),
                      NULL, this);

    wxTheApp->Connect(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                      this);
    wxTheApp->Connect(wxID_PASTE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                      this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                      this);
    wxTheApp->Connect(wxID_CUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                      this);

    EventNotifier::Get()->Bind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, &clMainFrame::OnEnvironmentVariablesModified,
                               this);
    EventNotifier::Get()->Connect(wxEVT_LOAD_SESSION, wxCommandEventHandler(clMainFrame::OnLoadSession), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_PROCESS_ENDED, clCommandEventHandler(clMainFrame::OnBuildEnded),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_PROJECT_CHANGED,
                                  wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                  wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED,
                                  wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clMainFrame::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                  wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(clMainFrame::OnWorkspaceClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING,
                                  wxCommandEventHandler(clMainFrame::OnRefactoringCacheStatus), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(clMainFrame::OnThemeChanged), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                  wxCommandEventHandler(clMainFrame::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(clMainFrame::OnSettingsChanged),
                               this);
    EventNotifier::Get()->Bind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT,
                               wxCommandEventHandler(clMainFrame::OnReloadExternallModifiedNoPrompt), this);
    EventNotifier::Get()->Bind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED,
                               wxCommandEventHandler(clMainFrame::OnReloadExternallModified), this);
    EventNotifier::Get()->Bind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, &clMainFrame::OnSingleInstanceOpenFiles,
                               this);
    EventNotifier::Get()->Bind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, &clMainFrame::OnSingleInstanceRaise, this);
    Connect(wxID_UNDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo), NULL,
            this);
    Connect(wxID_REDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo), NULL,
            this);

    EventNotifier::Get()->Connect(wxEVT_PROJ_RENAMED, clCommandEventHandler(clMainFrame::OnProjectRenamed), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_DEBUG_STARTED, &clMainFrame::OnDebugStarted, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &clMainFrame::OnDebugEnded, this);

    // Start the code completion manager, we do this by calling it once
    CodeCompletionManager::Get();

    // Register keyboard shortcuts
    clKeyboardManager::Get()->AddGlobalAccelerator("selection_to_multi_caret", "Ctrl-Shift-L",
                                                   _("Edit::Split selection into multiple carets"));
    clKeyboardManager::Get()->AddGlobalAccelerator("incremental_replace", "",
                                                   _("Search::Toggle the Quick-Replace Bar"));
    clKeyboardManager::Get()->AddGlobalAccelerator("copy_file_relative_path_to_workspace", "Ctrl-Alt-Shift-C",
                                                   "Edit::Copy Path Relative to Clipboard");
    clKeyboardManager::Get()->AddGlobalAccelerator("copy_file_name", "", "Edit::Copy Path to Clipboard");
    clKeyboardManager::Get()->AddGlobalAccelerator("copy_file_path", "", "Edit::Copy Full Path to Clipboard");
    clKeyboardManager::Get()->AddGlobalAccelerator("copy_file_name_only", "", "Edit::Copy File Name to Clipboard");
    clKeyboardManager::Get()->AddGlobalAccelerator("open_shell_from_filepath", "Ctrl-Shift-T",
                                                   "Search::Open Shell From File Path");
    clKeyboardManager::Get()->AddGlobalAccelerator("open_file_explorer", "Ctrl-Alt-Shift-T",
                                                   "Search::Open Containing Folder");

#ifdef __WXGTK__
    // Try to detect if this is a Wayland session; we have some Wayland-workaround code
    wxString sesstype("XDG_SESSION_TYPE"), session_type;
    wxGetEnv(sesstype, &session_type);
    m_isWaylandSession = session_type.Lower().Contains("wayland");
#endif
}

clMainFrame::~clMainFrame(void)
{
    wxDELETE(m_singleInstanceThread);
    wxDELETE(m_webUpdate);

#ifndef __WXMSW__
    m_zombieReaper.Stop();
#endif

    // Free the code completion manager
    CodeCompletionManager::Release();

// this will make sure that the main menu bar's member m_widget is freed before the we enter wxMenuBar destructor
// see this wxWidgets bug report for more details:
//  http://trac.wxwidgets.org/ticket/14292
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
    delete m_myMenuBar;
#endif

    wxTheApp->Disconnect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                         this);
    wxTheApp->Disconnect(wxID_PASTE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                         this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent),
                         NULL, this);
    wxTheApp->Disconnect(wxID_CUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::DispatchUpdateUIEvent), NULL,
                         this);

    EventNotifier::Get()->Unbind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, &clMainFrame::OnEnvironmentVariablesModified,
                                 this);
    EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_PROCESS_ENDED,
                                     clCommandEventHandler(clMainFrame::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_LOAD_SESSION, wxCommandEventHandler(clMainFrame::OnLoadSession), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_PROJECT_CHANGED,
                                     wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED,
                                     wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clMainFrame::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                     wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(clMainFrame::OnWorkspaceClosed),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                     wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING,
                                     wxCommandEventHandler(clMainFrame::OnRefactoringCacheStatus), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(clMainFrame::OnThemeChanged), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(clMainFrame::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(clMainFrame::OnSettingsChanged),
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT,
                                 wxCommandEventHandler(clMainFrame::OnReloadExternallModifiedNoPrompt), this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, &clMainFrame::OnSingleInstanceOpenFiles,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, &clMainFrame::OnSingleInstanceRaise, this);

    EventNotifier::Get()->Unbind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED,
                                 wxCommandEventHandler(clMainFrame::OnReloadExternallModified), this);

    Disconnect(wxID_UNDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo),
               NULL, this);
    Disconnect(wxID_REDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo),
               NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_RENAMED, clCommandEventHandler(clMainFrame::OnProjectRenamed), NULL,
                                     this);
    wxDELETE(m_timer);

    EventNotifier::Get()->Unbind(wxEVT_DEBUG_STARTED, &clMainFrame::OnDebugStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &clMainFrame::OnDebugEnded, this);

    // GetPerspectiveManager().DisconnectEvents() assumes that m_mgr is still alive (and it should be as it is allocated
    // on the stack of clMainFrame)
    ManagerST::Get()->GetPerspectiveManager().DisconnectEvents();

    ManagerST::Free();
    delete m_DPmenuMgr;

    // uninitialize AUI manager
    m_mgr.UnInit();
}

void clMainFrame::Initialize(bool loadLastSession)
{
    // set the revision number in the frame title
    wxString title(_("CodeLite "));
    title << CODELITE_VERSION_STRING;

    // initialize the environment variable configuration manager
    EnvironmentConfig::Instance()->Load();

    EditorConfig* cfg = EditorConfigST::Get();
    GeneralInfo inf;
    cfg->ReadObject(wxT("GeneralInfo"), &inf);

    int screenW = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int screenH = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

    // validate the frame loaded pos & size
    if(inf.GetFramePosition().x < 0 || inf.GetFramePosition().x > screenW) { inf.SetFramePosition(wxPoint(30, 3)); }

    if(inf.GetFramePosition().y < 0 || inf.GetFramePosition().y > screenH) { inf.SetFramePosition(wxPoint(30, 3)); }

    wxSize frameSize(inf.GetFrameSize());
    if(inf.GetFrameSize().x < 600 || inf.GetFrameSize().x > screenW) { frameSize.SetWidth(600); }
    if(inf.GetFrameSize().y < 400 || inf.GetFrameSize().y > screenH) { frameSize.SetHeight(400); }
    inf.SetFrameSize(frameSize);

#ifdef __WXOSX__
    inf.SetFramePosition(wxPoint(30, 100));
    inf.SetFrameSize(wxSize(600, 400));
#endif

    m_theFrame = new clMainFrame(NULL, wxID_ANY, title, inf.GetFramePosition(), inf.GetFrameSize(),
                                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);
    m_theFrame->m_frameGeneralInfo = inf;
    m_theFrame->m_loadLastSession = loadLastSession;
    m_theFrame->Maximize(m_theFrame->m_frameGeneralInfo.GetFlags() & CL_MAXIMIZE_FRAME ? true : false);

    // add the welcome page
    if(m_theFrame->m_frameGeneralInfo.GetFlags() & CL_SHOW_WELCOME_PAGE) { m_theFrame->CreateWelcomePage(); }

    // plugins must be loaded before the file explorer
    m_theFrame->CompleteInitialization();

    // time to create the file explorer
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("go_home"));
    m_theFrame->GetFileExplorer()->GetEventHandler()->ProcessEvent(e);

    m_theFrame->SendSizeEvent();
    m_theFrame->StartTimer();

    // Keep the current layout before loading the perspective from the disk
    m_theFrame->m_defaultLayout = m_theFrame->m_mgr.SavePerspective();

    // Save the current layout as the "Default" layout (unless we already got one ...)
    ManagerST::Get()->GetPerspectiveManager().SavePerspectiveIfNotExists(NORMAL_LAYOUT);

    // After all the plugins / panes have been loaded,
    // its time to re-load the perspective
    //    wxCommandEvent evt(wxEVT_LOAD_PERSPECTIVE);
    //    m_theFrame->AddPendingEvent(evt);
}

clMainFrame* clMainFrame::Get() { return m_theFrame; }

void clMainFrame::CreateGUIControls()
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_mainPanel = new wxPanel(this);
    GetSizer()->Add(m_mainPanel, 1, wxEXPAND);
    InitializeLogo();

#if defined(__WXOSX__) && wxCHECK_VERSION(3, 1, 0)
    EnableFullScreenView();
#endif

    // Instantiate the workspace manager
    // By calling its "Get" method
    clWorkspaceManager::Get();

    // tell wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(m_mainPanel);
    m_mgr.SetArtProvider(new clAuiDockArt(PluginManager::Get()));
    SetAUIManagerFlags();

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);

    int captionSize = 22;
    {
        wxMemoryDC memDC;
        wxBitmap bmp(1, 1);
        memDC.SelectObject(bmp);
        memDC.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        wxSize textSize = memDC.GetTextExtent("Tp");
        captionSize = textSize.y + 6; // 3 pixesl space on each side
    }

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, captionSize);

    m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, DrawingUtils::GetPanelBgColour());
    m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, DrawingUtils::GetPanelBgColour());

    // initialize debugger configuration tool
    DebuggerConfigTool::Get()->Load(wxT("config/debuggers.xml"), wxT("5.4"));
    clCxxWorkspaceST::Get()->SetStartupDir(ManagerST::Get()->GetStartupDirectory());

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
#ifdef __WXMSW__
    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
#elif defined(__WXOSX__)
    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 6);
#else
    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
#endif
    // Load the menubar from XRC and set this frame's menubar to it.
    wxMenuBar* mb = wxXmlResource::Get()->LoadMenuBar(wxT("main_menu"));
#ifdef __WXOSX__
    wxMenu* view = NULL;
    wxMenuItem* item = mb->FindItem(XRCID("show_tabs_tab"), &view);
    if(item && view) { view->Remove(item); }
#endif
    // Under wxGTK < 2.9.4 we need this wrapper class to avoid warnings on ubuntu when codelite exits
    m_myMenuBar = new MyMenuBar();
    m_myMenuBar->Set(mb);
    SetMenuBar(mb);

    // Create the status bar
    m_statusBar = new clStatusBar(this, PluginManager::Get());
    SetStatusBar(m_statusBar);

    // Set up dynamic parts of menu.
    CreateRecentlyOpenedWorkspacesMenu();
    DoUpdatePerspectiveMenu();

    // Connect to Edit menu, so that its labelled-state submenu can be added on the fly when necessary
    wxMenu* editmenu = NULL;
    wxMenuItem* menuitem = GetMenuBar()->FindItem(wxID_UNDO, &editmenu);
    if(menuitem && editmenu) {
        editmenu->Bind(wxEVT_MENU_OPEN, wxMenuEventHandler(clMainFrame::OnEditMenuOpened), this);
    }

    m_DPmenuMgr = new DockablePaneMenuManager(GetMenuBar(), &m_mgr);

    //---------------------------------------------
    // Add docking windows
    //---------------------------------------------

    // I'm not localising the captions of these Views atm. That's because wxAui uses name+caption to ID a pane
    // It also serialises the caption. That means that changing locale will break the layout stored in codelite.layout
    // If it's decided to do this in the future, change only the 'Caption(wxT("Output View"))' bits below
    // However I'm creating unused strings here, so that the translations remain in the catalogue
    const wxString unusedOV(_("Output View"));
    const wxString unusedWV(_("Workspace View"));
    const wxString unusedCR(
        _("wxCrafter")); // One that would otherwise be untranslated; OT here, but it's a convenient place to put it

    // Add the explorer pane
    m_workspacePane = new WorkspacePane(m_mainPanel, wxT("Workspace View"), &m_mgr);
    m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo()
                                       .PinButton()
                                       .CaptionVisible(true)
                                       .MinimizeButton()
                                       .MaximizeButton()
                                       .Name(m_workspacePane->GetCaption())
                                       .Caption(m_workspacePane->GetCaption())
                                       .Left()
                                       .BestSize(250, 300)
                                       .Layer(1)
                                       .Position(0)
                                       .CloseButton(true));
    RegisterDockWindow(XRCID("workspace_pane"), wxT("Workspace View"));

    // add the debugger locals tree, make it hidden by default
    m_debuggerPane = new DebuggerPane(m_mainPanel, wxT("Debugger"), &m_mgr);
    m_mgr.AddPane(m_debuggerPane, wxAuiPaneInfo()
                                      .CaptionVisible(true)
                                      .Name(m_debuggerPane->GetCaption())
                                      .Caption(m_debuggerPane->GetCaption())
                                      .Bottom()
                                      .Layer(1)
                                      .Position(1)
                                      .CloseButton(true)
                                      .MinimizeButton()
                                      .Hide()
                                      .MaximizeButton());
    RegisterDockWindow(XRCID("debugger_pane"), wxT("Debugger"));

    m_mainBook = new MainBook(m_mainPanel);
    m_mgr.AddPane(m_mainBook, wxAuiPaneInfo().Name(wxT("Editor")).CenterPane().PaneBorder(false));
    CreateRecentlyOpenedFilesMenu();

    m_outputPane = new OutputPane(m_mainPanel, wxT("Output View"));
    wxAuiPaneInfo paneInfo;
    m_mgr.AddPane(m_outputPane, paneInfo.CaptionVisible(true)
                                    .PinButton()
                                    .Name(wxT("Output View"))
                                    .Caption(wxT("Output View"))
                                    .Bottom()
                                    .Layer(1)
                                    .Position(0)
                                    .MinimizeButton()
                                    .Show()
                                    .BestSize(wxSize(400, 200))
                                    .MaximizeButton());
    RegisterDockWindow(XRCID("output_pane"), wxT("Output View"));

    long show_nav = EditorConfigST::Get()->GetInteger(wxT("ShowNavBar"), 0);
    if(!show_nav) { m_mainBook->ShowNavBar(false); }

    if(!BuildSettingsConfigST::Get()->Load(wxT("2.1"))) {
        CL_ERROR("Could not locate build configuration! CodeLite installation is broken this might cause unwanted "
                 "behavior!");
    }

    clConfig ccConfig("code-completion.conf");
    ccConfig.ReadItem(&m_tagsOptionsData);

    // If the cc options value has changed, construct a new instance
    // with default values and call the "Merge" method
    TagsOptionsData tmp;
    m_tagsOptionsData.Merge(tmp);
    ccConfig.WriteItem(&m_tagsOptionsData);

    TagsManager* tagsManager = TagsManagerST::Get();

    // start ctags process
    TagsManagerST::Get()->SetCodeLiteIndexerPath(clStandardPaths::Get().GetBinaryFullPath("codelite_indexer"));
    ManagerST::Get()->SetCodeLiteLauncherPath(clStandardPaths::Get().GetBinaryFullPath("codelite_launcher"));
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
    ParseThreadST::Get()->SetSearchPaths(m_tagsOptionsData.GetParserSearchPaths(),
                                         m_tagsOptionsData.GetParserExcludePaths());

    ParseThreadST::Get()->Start();
    ClangCompilationDbThreadST::Get()->Start();

    // Connect this tree to the parse thread
    ParseThreadST::Get()->SetNotifyWindow(this);

    // update ctags options
    TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);

    // set this frame as the event handler for any events
    TagsManagerST::Get()->SetEvtHandler(this);

    // We must do Layout() before loading the toolbars, otherwise they're broken in >=wxGTK-2.9
    Layout();

    // Create the toolbars
    // If we requested to create a single toolbar, create a native toolbar
    // otherwise, we create a multiple toolbars using wxAUI toolbar if possible
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    if(options) {
        bool nativeToolbar = !PluginManager::Get()->AllowToolbar();
        if(options->GetIconsSize() == 16) {
            nativeToolbar ? CreateNativeToolbar16() : CreateToolbars16();
        } else {
            nativeToolbar ? CreateNativeToolbar24() : CreateToolbars24();
        }
        Bind(wxEVT_TOOL_DROPDOWN, wxCommandEventHandler(clMainFrame::OnNativeTBUnRedoDropdown), this, wxID_UNDO,
             wxID_REDO);
    } else {
        CreateNativeToolbar16();
    }

    // Connect the custom build target events range: !USE_AUI_TOOLBAR only
    if(GetMainToolBar()) {
        GetMainToolBar()->Connect(ID_MENU_CUSTOM_TARGET_FIRST, ID_MENU_CUSTOM_TARGET_MAX, wxEVT_COMMAND_MENU_SELECTED,
                                  wxCommandEventHandler(clMainFrame::OnBuildCustomTarget), NULL, this);
    }

    Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnChangeActiveBookmarkType), this,
         XRCID("BookmarkTypes[start]"), XRCID("BookmarkTypes[end]"));
         
#if !USE_AUI_NOTEBOOK
    GetWorkspacePane()->GetNotebook()->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("workspace_view_rmenu")));
    GetDebuggerPane()->GetNotebook()->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("debugger_view_rmenu")));
#endif

    m_mgr.Update();
    SetAutoLayout(true);

    // try to locate the build tools

    long fix(1);
    fix = EditorConfigST::Get()->GetInteger(wxT("FixBuildToolOnStartup"), fix);
    if(fix) { UpdateBuildTools(); }

    // This is needed in >=wxGTK-2.9, otherwise the auinotebook doesn't fully expand at first
    SendSizeEvent(wxSEND_EVENT_POST);

    // Ditto the workspace pane auinotebook
    GetWorkspacePane()->SendSizeEvent(wxSEND_EVENT_POST);
}

void clMainFrame::DoShowToolbars(bool show, bool update)
{
// Hide the _native_ toolbar
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif

    if(GetMainToolBar()) {
        if(show) {
            SetToolBar(NULL);
            // Recreate the toolbar
            if(EditorConfigST::Get()->GetOptions()->GetIconsSize() == 24) {
                CreateNativeToolbar24();
            } else {
                CreateNativeToolbar16();
            }

            // Update the build drop down menu
            if(clCxxWorkspaceST::Get()->IsOpen()) {
                wxMenu* buildDropDownMenu = new wxMenu;
                DoCreateBuildDropDownMenu(buildDropDownMenu);
                if(GetMainToolBar() && GetMainToolBar()->FindById(XRCID("build_active_project"))) {
                    GetMainToolBar()->SetDropdownMenu(XRCID("build_active_project"), buildDropDownMenu);
                }
            }

        } else {
            GetMainToolBar()->Hide();
            GetMainToolBar()->Realize();
            Layout();
        }
    } else {
        wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();
        for(size_t i = 0; i < panes.GetCount(); ++i) {
            if(panes.Item(i).IsOk() && panes.Item(i).IsToolbar()) { panes.Item(i).Show(show); }
        }

        if(update) {
            m_mgr.Update();
            SendSizeEvent();
        }
    }
}

void clMainFrame::OnEditMenuOpened(wxMenuEvent& event)
{
    event.Skip();
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    wxMenuItem* labelCurrentState = event.GetMenu()->FindChildItem(XRCID("label_current_state"));
    if(labelCurrentState) { // Here seems to be the only reliable place to do 'updateui' for this; a real UpdateUI
                            // handler is only hit when there's no editor :/
        labelCurrentState->Enable(editor != NULL);
    }

    if(editor) {
        editor->GetCommandsProcessor().PrepareLabelledStatesMenu(event.GetMenu());

    } else {
        // There's no active editor, so remove any stale submenu; otherwise it'll display but the contents won't work
        wxMenuItem* menuitem = event.GetMenu()->FindChildItem(XRCID("goto_labelled_state"));
        if(menuitem) { event.GetMenu()->Delete(menuitem); }
    }
}

void clMainFrame::OnNativeTBUnRedoDropdown(wxCommandEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor && GetMainToolBar()) {
        bool undoing = event.GetId() == wxID_UNDO;
        wxMenu* menu = new wxMenu;
        editor->GetCommandsProcessor().DoPopulateUnRedoMenu(*menu, undoing);
        if(!menu->GetMenuItemCount()) {
            delete menu;
            return;
        }

        if(undoing) {
            menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem),
                       &editor->GetCommandsProcessor());
        } else {
            menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem),
                       &editor->GetCommandsProcessor());
        }
        GetMainToolBar()->SetDropdownMenu(event.GetId(), menu);

        event.Skip();
    }
    // Don't skip if there's no active editor/toolbar, otherwise a stale menu will show
}

void clMainFrame::CreateToolbars24()
{
    wxAuiPaneInfo info;
    wxWindow* toolbar_parent(m_mainPanel);

    //----------------------------------------------
    // create the standard toolbar
    //----------------------------------------------
    clToolBar* tb =
        new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetToolBitmapSize(wxSize(24, 24));
    tb->SetArtProvider(new CLMainAuiTBArt(true));

    BitmapLoader& bmpLoader = *(PluginManager::Get()->GetStdIcons());
    tb->AddTool(XRCID("new_file"), _("New"), bmpLoader.LoadBitmap(wxT("file_new"), 24), _("New File"));
    tb->AddTool(XRCID("open_file"), _("Open"), bmpLoader.LoadBitmap(wxT("file_open"), 24), _("Open File"));
    tb->AddTool(XRCID("refresh_file"), _("Reload"), bmpLoader.LoadBitmap(wxT("file_reload"), 24), _("Reload File"));
    tb->AddTool(XRCID("save_file"), _("Save"), bmpLoader.LoadBitmap(wxT("file_save"), 24), _("Save"));
    tb->AddTool(XRCID("save_all"), _("Save All"), bmpLoader.LoadBitmap(wxT("file_save_all"), 24), _("Save All"));
    tb->AddSeparator();
    tb->AddTool(XRCID("close_file"), _("Close"), bmpLoader.LoadBitmap(wxT("file_close"), 24), _("Close File"));
    tb->AddSeparator();
    tb->AddTool(wxID_CUT, _("Cut"), bmpLoader.LoadBitmap(wxT("cut"), 24), _("Cut"));
    tb->AddTool(wxID_COPY, _("Copy"), bmpLoader.LoadBitmap(wxT("copy"), 24), _("Copy"));
    tb->AddTool(wxID_PASTE, _("Paste"), bmpLoader.LoadBitmap(wxT("paste"), 24), _("Paste"));
    tb->AddSeparator();
    tb->AddTool(wxID_UNDO, _("Undo"), bmpLoader.LoadBitmap(wxT("undo"), 24), _("Undo"));
    tb->SetToolDropDown(wxID_UNDO, true);
    tb->AddTool(wxID_REDO, _("Redo"), bmpLoader.LoadBitmap(wxT("redo"), 24), _("Redo"));
    tb->SetToolDropDown(wxID_REDO, true);
    tb->AddTool(wxID_BACKWARD, _("Backward"), bmpLoader.LoadBitmap(wxT("back"), 24), _("Backward"));
    tb->AddTool(wxID_FORWARD, _("Forward"), bmpLoader.LoadBitmap(wxT("forward"), 24), _("Forward"));
    tb->AddSeparator();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("bookmark"), 24),
                _("Toggle Bookmark"));
    tb->SetToolDropDown(XRCID("toggle_bookmark"), true);

    tb->Realize();
    m_mgr.AddPane(tb, wxAuiPaneInfo()
                          .Name(wxT("Standard Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Standard"))
                          .ToolbarPane()
                          .Top()
                          .Position(0));
    //----------------------------------------------
    // create the search toolbar
    //----------------------------------------------
    info = wxAuiPaneInfo();
    tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));
    tb->SetToolBitmapSize(wxSize(24, 24));

    tb->AddTool(wxID_FIND, _("Find"), bmpLoader.LoadBitmap(wxT("find"), 24), _("Find"));
    tb->AddTool(wxID_REPLACE, _("Replace"), bmpLoader.LoadBitmap(wxT("find_and_replace"), 24), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"), bmpLoader.LoadBitmap(wxT("find_in_files"), 24),
                _("Find In Files"));
    tb->AddSeparator();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("open_resource"), 24),
                _("Find Resource In Workspace"));
    tb->AddSeparator();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"), bmpLoader.LoadBitmap(wxT("mark_word"), 24),
                _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    tb->AddSeparator();

    tb->Realize();
    m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Search"))
                          .ToolbarPane()
                          .Top()
                          .Position(1));
    //----------------------------------------------
    // create the build toolbar
    //----------------------------------------------
    tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));
    tb->SetToolBitmapSize(wxSize(24, 24));

    tb->AddTool(XRCID("build_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("build"), 24),
                _("Build Active Project"));
    tb->SetToolDropDown(XRCID("build_active_project"), true);

    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("stop"), 24),
                _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("clean"), 24),
                _("Clean Active Project"));
    tb->AddSeparator();
    tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute"), 24),
                _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute_stop"), 24),
                _("Stop Running Program"));

    tb->Realize();
    info = wxAuiPaneInfo();
    m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Build"))
                          .ToolbarPane()
                          .Top()
                          .Position(2));

    //----------------------------------------------
    // create the debugger toolbar
    //----------------------------------------------
    tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));
    tb->SetToolBitmapSize(wxSize(24, 24));

    tb->AddTool(XRCID("start_debugger"), _("Start or Continue debugger"),
                bmpLoader.LoadBitmap(wxT("debugger_start"), 24), _("Start or Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), bmpLoader.LoadBitmap(wxT("debugger_stop"), 24),
                _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), bmpLoader.LoadBitmap(wxT("interrupt"), 24),
                _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"), bmpLoader.LoadBitmap(wxT("debugger_restart"), 24),
                _("Restart debugger"));
    tb->AddSeparator();
    tb->AddTool(XRCID("show_cursor"), _("Show Current Line"), bmpLoader.LoadBitmap(wxT("show_current_line"), 24),
                _("Show Current Line"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), bmpLoader.LoadBitmap(wxT("step_in"), 24), _("Step In"));
    tb->AddTool(XRCID("dbg_next"), _("Next"), bmpLoader.LoadBitmap(wxT("next"), 24), _("Next"));
    tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), bmpLoader.LoadBitmap(wxT("step_out"), 24), _("Step Out"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"), bmpLoader.LoadBitmap("rewind", 24),
                _("Toggle Rewind Commands"), wxITEM_CHECK);
    tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"), bmpLoader.LoadBitmap("record", 24),
                _("Start Reverse Debug Recording"), wxITEM_CHECK);

    tb->Realize();

    info = wxAuiPaneInfo();
    m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Debug"))
                          .ToolbarPane()
                          .Top()
                          .Position(3));

    RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
    RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
    RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
    RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
}

void clMainFrame::CreateNativeToolbar16()
{
    //----------------------------------------------
    // create the standard toolbar
    //----------------------------------------------
    wxToolBar* tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
    BitmapLoader& bmpLoader = *(PluginManager::Get()->GetStdIcons());

    tb->SetToolBitmapSize(wxSize(16, 16));
    tb->AddTool(XRCID("new_file"), _("New"), bmpLoader.LoadBitmap(wxT("file_new")), _("New File"));
    tb->AddTool(XRCID("open_file"), _("Open"), bmpLoader.LoadBitmap(wxT("file_open")), _("Open File"));
    tb->AddTool(XRCID("refresh_file"), _("Reload"), bmpLoader.LoadBitmap(wxT("file_reload")), _("Reload File"));
    tb->AddSeparator();
    tb->AddTool(XRCID("save_file"), _("Save"), bmpLoader.LoadBitmap(wxT("file_save")), _("Save"));
    tb->AddTool(XRCID("save_all"), _("Save All"), bmpLoader.LoadBitmap(wxT("file_save_all")), _("Save All"));
    tb->AddSeparator();
    tb->AddTool(XRCID("close_file"), _("Close"), bmpLoader.LoadBitmap(wxT("file_close")), _("Close File"));
    tb->AddSeparator();
    tb->AddTool(wxID_CUT, _("Cut"), bmpLoader.LoadBitmap(wxT("cut")), _("Cut"));
    tb->AddTool(wxID_COPY, _("Copy"), bmpLoader.LoadBitmap(wxT("copy")), _("Copy"));
    tb->AddTool(wxID_PASTE, _("Paste"), bmpLoader.LoadBitmap(wxT("paste")), _("Paste"));
    tb->AddSeparator();
    tb->AddTool(wxID_UNDO, _("Undo"), bmpLoader.LoadBitmap(wxT("undo")), _("Undo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_REDO, _("Redo"), bmpLoader.LoadBitmap(wxT("redo")), _("Redo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_BACKWARD, _("Backward"), bmpLoader.LoadBitmap(wxT("back")), _("Backward"));
    tb->AddTool(wxID_FORWARD, _("Forward"), bmpLoader.LoadBitmap(wxT("forward")), _("Forward"));
    tb->AddSeparator();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("bookmark")),
                _("Toggle Bookmark"), wxITEM_DROPDOWN);
    tb->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));

    //----------------------------------------------
    // create the search toolbar
    //----------------------------------------------
    tb->AddSeparator();
    tb->AddTool(wxID_FIND, _("Find"), bmpLoader.LoadBitmap(wxT("find")), _("Find"));
    tb->AddTool(wxID_REPLACE, _("Replace"), bmpLoader.LoadBitmap(wxT("find_and_replace")), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"), bmpLoader.LoadBitmap(wxT("find_in_files")),
                _("Find In Files"));
    tb->AddSeparator();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("open_resource")),
                _("Find Resource In Workspace"));
    tb->AddSeparator();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"), bmpLoader.LoadBitmap(wxT("mark_word")),
                _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    tb->AddSeparator();

    //----------------------------------------------
    // create the build toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("build_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("build")),
                _("Build Active Project"), wxITEM_DROPDOWN);

    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("stop")),
                _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("clean")),
                _("Clean Active Project"));
    tb->AddSeparator();
    tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute")),
                _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute_stop")),
                _("Stop Running Program"));
    tb->AddSeparator();

    //----------------------------------------------
    // create the debugger toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("start_debugger"), _("Start or Continue debugger"), bmpLoader.LoadBitmap(wxT("debugger_start")),
                _("Start or Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), bmpLoader.LoadBitmap(wxT("debugger_stop")),
                _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), bmpLoader.LoadBitmap(wxT("interrupt")),
                _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"), bmpLoader.LoadBitmap(wxT("debugger_restart")),
                _("Restart debugger"));
    tb->AddSeparator();
    tb->AddTool(XRCID("show_cursor"), _("Show Current Line"), bmpLoader.LoadBitmap(wxT("show_current_line")),
                _("Show Current Line"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), bmpLoader.LoadBitmap(wxT("step_in")), _("Step In"));
    tb->AddTool(XRCID("dbg_next"), _("Next"), bmpLoader.LoadBitmap(wxT("next")), _("Next"));
    tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), bmpLoader.LoadBitmap(wxT("step_out")), _("Step Out"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"), bmpLoader.LoadBitmap("rewind"),
                _("Toggle Rewind Commands"), wxITEM_CHECK);
    tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"), bmpLoader.LoadBitmap("record"),
                _("Start Reverse Debug Recording"), wxITEM_CHECK);

    SetToolBar(tb);
    tb->Realize();
}

void clMainFrame::CreateNativeToolbar24()
{
    //----------------------------------------------
    // create the standard toolbar
    //----------------------------------------------
    wxToolBar* tb =
        new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORIZONTAL);
    BitmapLoader& bmpLoader = *(PluginManager::Get()->GetStdIcons());

    tb->SetToolBitmapSize(wxSize(24, 24));
    tb->AddTool(XRCID("new_file"), _("New"), bmpLoader.LoadBitmap(wxT("file_new"), 24), _("New File"));
    tb->AddTool(XRCID("open_file"), _("Open"), bmpLoader.LoadBitmap(wxT("file_open"), 24), _("Open File"));
    tb->AddTool(XRCID("refresh_file"), _("Reload"), bmpLoader.LoadBitmap(wxT("file_reload"), 24), _("Reload File"));
    tb->AddTool(XRCID("save_file"), _("Save"), bmpLoader.LoadBitmap(wxT("file_save"), 24), _("Save"));
    tb->AddTool(XRCID("save_all"), _("Save All"), bmpLoader.LoadBitmap(wxT("file_save_all"), 24), _("Save All"));
    tb->AddTool(XRCID("close_file"), _("Close"), bmpLoader.LoadBitmap(wxT("file_close"), 24), _("Close File"));
    tb->AddSeparator();
    tb->AddTool(wxID_CUT, _("Cut"), bmpLoader.LoadBitmap(wxT("cut"), 24), _("Cut"));
    tb->AddTool(wxID_COPY, _("Copy"), bmpLoader.LoadBitmap(wxT("copy"), 24), _("Copy"));
    tb->AddTool(wxID_PASTE, _("Paste"), bmpLoader.LoadBitmap(wxT("paste"), 24), _("Paste"));
    tb->AddTool(wxID_UNDO, _("Undo"), bmpLoader.LoadBitmap(wxT("undo"), 24), _("Undo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_REDO, _("Redo"), bmpLoader.LoadBitmap(wxT("redo"), 24), _("Redo"), wxITEM_DROPDOWN);
    tb->AddSeparator();
    tb->AddTool(wxID_BACKWARD, _("Backward"), bmpLoader.LoadBitmap(wxT("back"), 24), _("Backward"));
    tb->AddTool(wxID_FORWARD, _("Forward"), bmpLoader.LoadBitmap(wxT("forward"), 24), _("Forward"));
    tb->AddSeparator();

    //----------------------------------------------
    // create the search toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("bookmark"), 24),
                _("Toggle Bookmark"), wxITEM_DROPDOWN);
    tb->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));
    tb->AddTool(wxID_FIND, _("Find"), bmpLoader.LoadBitmap(wxT("find"), 24), _("Find"));
    tb->AddTool(wxID_REPLACE, _("Replace"), bmpLoader.LoadBitmap(wxT("find_and_replace"), 24), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"), bmpLoader.LoadBitmap(wxT("find_in_files"), 24),
                _("Find In Files"));
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("open_resource"), 24),
                _("Find Resource In Workspace"));
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"), bmpLoader.LoadBitmap(wxT("mark_word"), 24),
                _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    tb->AddSeparator();

    //----------------------------------------------
    // create the build toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("build_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("build"), 24),
                _("Build Active Project"), wxITEM_DROPDOWN);
    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("stop"), 24),
                _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("clean"), 24),
                _("Clean Active Project"));
    tb->AddSeparator();
    tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute"), 24),
                _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute_stop"), 24),
                _("Stop Running Program"));
    tb->AddSeparator();

    //----------------------------------------------
    // create the debugger toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("start_debugger"), _("Start or Continue debugger"),
                bmpLoader.LoadBitmap(wxT("debugger_start"), 24), _("Start or Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), bmpLoader.LoadBitmap(wxT("debugger_stop"), 24),
                _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), bmpLoader.LoadBitmap(wxT("interrupt"), 24),
                _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"), bmpLoader.LoadBitmap(wxT("debugger_restart"), 24),
                _("Restart debugger"));
    tb->AddTool(XRCID("show_cursor"), _("Show Current Line"), bmpLoader.LoadBitmap(wxT("show_current_line"), 24),
                _("Show Current Line"));
    tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), bmpLoader.LoadBitmap(wxT("step_in"), 24), _("Step In"));
    tb->AddTool(XRCID("dbg_next"), _("Next"), bmpLoader.LoadBitmap(wxT("next"), 24), _("Next"));
    tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), bmpLoader.LoadBitmap(wxT("step_out"), 24), _("Step Out"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"), bmpLoader.LoadBitmap("rewind", 24),
                _("Toggle Rewind Commands"), wxITEM_CHECK);
    tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"), bmpLoader.LoadBitmap("record", 24),
                _("Start Reverse Debug Recording"), wxITEM_CHECK);

    SetToolBar(tb);
    tb->Realize();
}

void clMainFrame::CreateToolbars16()
{
    //----------------------------------------------
    // create the standard toolbar
    //----------------------------------------------
    wxWindow* toolbar_parent(m_mainPanel);

    clToolBar* tb =
        new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));

    wxAuiPaneInfo info;
    BitmapLoader& bmpLoader = *(PluginManager::Get()->GetStdIcons());

    tb->SetToolBitmapSize(wxSize(16, 16));
    tb->AddTool(XRCID("new_file"), _("New"), bmpLoader.LoadBitmap(wxT("file_new")), _("New File"));
    tb->AddTool(XRCID("open_file"), _("Open"), bmpLoader.LoadBitmap(wxT("file_open")), _("Open File"));
    tb->AddTool(XRCID("refresh_file"), _("Reload"), bmpLoader.LoadBitmap(wxT("file_reload")), _("Reload File"));
    tb->AddTool(XRCID("save_file"), _("Save"), bmpLoader.LoadBitmap(wxT("file_save")), _("Save"));
    tb->AddTool(XRCID("save_all"), _("Save All"), bmpLoader.LoadBitmap(wxT("file_save_all")), _("Save All"));
    tb->AddSeparator();

    tb->AddTool(XRCID("close_file"), _("Close"), bmpLoader.LoadBitmap(wxT("file_close")), _("Close File"));
    tb->AddSeparator();
    tb->AddTool(wxID_CUT, _("Cut"), bmpLoader.LoadBitmap(wxT("cut")), _("Cut"));
    tb->AddTool(wxID_COPY, _("Copy"), bmpLoader.LoadBitmap(wxT("copy")), _("Copy"));
    tb->AddTool(wxID_PASTE, _("Paste"), bmpLoader.LoadBitmap(wxT("paste")), _("Paste"));
    tb->AddSeparator();
    tb->AddTool(wxID_UNDO, _("Undo"), bmpLoader.LoadBitmap(wxT("undo")), _("Undo"));
    tb->SetToolDropDown(wxID_UNDO, true);
    tb->AddTool(wxID_REDO, _("Redo"), bmpLoader.LoadBitmap(wxT("redo")), _("Redo"));
    tb->SetToolDropDown(wxID_REDO, true);
    tb->AddTool(wxID_BACKWARD, _("Backward"), bmpLoader.LoadBitmap(wxT("back")), _("Backward"));
    tb->AddTool(wxID_FORWARD, _("Forward"), bmpLoader.LoadBitmap(wxT("forward")), _("Forward"));
    tb->AddSeparator();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("bookmark")),
                _("Toggle Bookmark"));
    tb->SetToolDropDown(XRCID("toggle_bookmark"), true);

    tb->Realize();
    m_mgr.AddPane(tb, wxAuiPaneInfo()
                          .Name(wxT("Standard Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Standard"))
                          .ToolbarPane()
                          .Top()
                          .Position(0));

    //----------------------------------------------
    // create the search toolbar
    //----------------------------------------------
    info = wxAuiPaneInfo();

    tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));
    tb->SetToolBitmapSize(wxSize(16, 16));

    tb->AddTool(wxID_FIND, _("Find"), bmpLoader.LoadBitmap(wxT("find")), _("Find"));
    tb->AddTool(wxID_REPLACE, _("Replace"), bmpLoader.LoadBitmap(wxT("find_and_replace")), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"), bmpLoader.LoadBitmap(wxT("find_in_files")),
                _("Find In Files"));
    tb->AddSeparator();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("open_resource")),
                _("Find Resource In Workspace"));
    tb->AddSeparator();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"), bmpLoader.LoadBitmap(wxT("mark_word")),
                _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    tb->AddSeparator();

    tb->Realize();
    m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Search"))
                          .ToolbarPane()
                          .Top()
                          .Position(1));

    //----------------------------------------------
    // create the build toolbar
    //----------------------------------------------
    tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));
    tb->SetToolBitmapSize(wxSize(16, 16));

    tb->AddTool(XRCID("build_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("build")),
                _("Build Active Project"));
    tb->SetToolDropDown(XRCID("build_active_project"), true);
    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("stop")),
                _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"), wxEmptyString, bmpLoader.LoadBitmap(wxT("clean")),
                _("Clean Active Project"));
    tb->AddSeparator();
    tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute")),
                _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"), wxEmptyString, bmpLoader.LoadBitmap(wxT("execute_stop")),
                _("Stop Running Program"));

    tb->Realize();
    info = wxAuiPaneInfo();
    m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Build"))
                          .ToolbarPane()
                          .Top()
                          .Position(2));

    //----------------------------------------------
    // create the debugger toolbar
    //----------------------------------------------
    tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->SetArtProvider(new CLMainAuiTBArt(true));
    tb->SetToolBitmapSize(wxSize(16, 16));

    tb->AddTool(XRCID("start_debugger"), _("Start / Continue debugger"), bmpLoader.LoadBitmap(wxT("debugger_start")),
                _("Start / Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), bmpLoader.LoadBitmap(wxT("debugger_stop")),
                _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), bmpLoader.LoadBitmap(wxT("interrupt")),
                _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"), bmpLoader.LoadBitmap(wxT("debugger_restart")),
                _("Restart debugger"));
    tb->AddSeparator();
    tb->AddTool(XRCID("show_cursor"), _("Show Current Line"), bmpLoader.LoadBitmap(wxT("show_current_line")),
                _("Show Current Line"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), bmpLoader.LoadBitmap("step_in"), _("Step In"));
    tb->AddTool(XRCID("dbg_next"), _("Next"), bmpLoader.LoadBitmap("next"), _("Next"));
    tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), bmpLoader.LoadBitmap("step_out"), _("Step Out"));
    tb->AddSeparator();
    tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"), bmpLoader.LoadBitmap("rewind"),
                _("Toggle Rewind Commands"), wxITEM_CHECK);
    tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"), bmpLoader.LoadBitmap("record"),
                _("Start Reverse Debug Recording"), wxITEM_CHECK);

    tb->Realize();

    info = wxAuiPaneInfo();
    m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar"))
                          .LeftDockable(true)
                          .RightDockable(true)
                          .Caption(_("Debug"))
                          .ToolbarPane()
                          .Top()
                          .Position(3));
    RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
    RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
    RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
    RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
}

bool clMainFrame::StartSetupWizard()
{
    clBootstrapWizard wiz(this);
    if(wiz.RunWizard(wiz.GetFirstPage())) {
        {
            wxString message;

            if(wiz.IsRestartRequired()) {
                message << _("Applying your choices and restarting CodeLite");
            } else {
                message << _("Applying your choices, this may take a few seconds");
            }

            wxBusyInfo bi(message);

            clBootstrapData data = wiz.GetData();

            // update the compilers if not empty
            if(!data.compilers.empty()) {
                BuildSettingsConfigST::Get()->SetCompilers(data.compilers);
                CallAfter(&clMainFrame::UpdateParserSearchPathsFromDefaultCompiler);
            }
            OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
            options->SetIndentUsesTabs(data.useTabs);
            options->SetShowWhitspaces(data.whitespaceVisibility);
            EditorConfigST::Get()->SetOptions(options);

            // Update the theme
            ColoursAndFontsManager::Get().SetTheme(data.selectedTheme);
            ColoursAndFontsManager::Get().Save();
        }

        if(wiz.IsRestartRequired()) {
            clCommandEvent restartEvent(wxEVT_RESTART_CODELITE);
            ManagerST::Get()->AddPendingEvent(restartEvent);
            return true;
        }
    }
    return false;
}

void clMainFrame::Bootstrap()
{
    if(!clSplashScreen::g_destroyed && clSplashScreen::g_splashScreen) {
        clSplashScreen::g_splashScreen->Hide();
        clSplashScreen::g_splashScreen->Destroy();
        clSplashScreen::g_splashScreen = NULL;
    }

    if(!clConfig::Get().Read(kConfigBootstrapCompleted, false)) {
        clConfig::Get().Write(kConfigBootstrapCompleted, true);
        if(StartSetupWizard()) return;
    }

    // Load the session manager
    wxString sessConfFile;
    sessConfFile << clStandardPaths::Get().GetUserDataDir() << wxT("/config/sessions.xml");
    SessionManager::Get().Load(sessConfFile);

    // restore last session if needed
    if(clConfig::Get().Read(kConfigRestoreLastSession, true) && m_loadLastSession) {
        wxCommandEvent loadSessionEvent(wxEVT_LOAD_SESSION);
        EventNotifier::Get()->AddPendingEvent(loadSessionEvent);
    }
}

void clMainFrame::UpdateBuildTools() {}

void clMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) { Close(); }

void clMainFrame::OnTBUnRedo(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked()) {
        LEditor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) {
            editor->GetCommandsProcessor().ProcessEvent(event);

        } else if(GetMainBook()->GetCurrentPage()) {
            event.StopPropagation(); // Otherwise we'll infinitely loop if the active page doesn't handle this event
            GetMainBook()->GetCurrentPage()->GetEventHandler()->ProcessEvent(event); // Let the active plugin have a go
        }
    }

    else {
        DispatchCommandEvent(event); // Revert to standard processing
    }
}

//----------------------------------------------------
// Helper method for the event handling
//----------------------------------------------------

bool clMainFrame::IsEditorEvent(wxEvent& event)
{
#ifdef __WXGTK__
    MainBook* mainBook = GetMainBook();
    if(!mainBook || !mainBook->GetActiveEditor(true)) {
        if(event.GetId() == wxID_FIND)
            return true;
        else
            return false;
    }

    switch(event.GetId()) {
    case wxID_CUT:
    case wxID_SELECTALL:
    case wxID_COPY:
    case wxID_PASTE:
    case wxID_UNDO:
    case wxID_REDO: {
        bool isFocused;
        LEditor* editor = dynamic_cast<LEditor*>(event.GetEventObject());
        if(editor) {
            isFocused = true;

        } else {
            isFocused = mainBook->GetActiveEditor(true)->IsFocused();
        }
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
    wxWindow* focusWin = wxWindow::FindFocus();
    if(focusWin) {
        switch(event.GetId()) {
        case wxID_CUT:
        case wxID_SELECTALL:
        case wxID_COPY:
        case wxID_PASTE:
        case wxID_UNDO:
        case wxID_REDO: {
            LEditor* ed = dynamic_cast<LEditor*>(focusWin);
            if(!ed) {
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

void clMainFrame::DispatchCommandEvent(wxCommandEvent& event)
{
    if(!IsEditorEvent(event)) {
        event.Skip();
        return;
    }

    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) {
        editor->OnMenuCommand(event);
    } else if(event.GetId() == wxID_FIND) {
        // Let the plugins handle this
        GetMainBook()->ShowQuickBarForPlugins();
    }
}

void clMainFrame::DispatchUpdateUIEvent(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();

    if(!IsEditorEvent(event)) {
        event.Skip();
        return;
    }

    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(!editor) {
        event.Enable(false);
        return;
    }

    if(event.GetId() >= viewAsMenuItemID && event.GetId() <= viewAsMenuItemMaxID) {
        // keep the old id as int and override the value set in the event object
        // to trick the event system
        event.SetInt(event.GetId());
        event.SetId(viewAsMenuItemID);
    }
    editor->OnUpdateUI(event);
}

void clMainFrame::OnFileExistUpdateUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable(GetMainBook()->GetActiveEditor(true) != NULL);
}

void clMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString mainTitle;
    mainTitle = CODELITE_VERSION_STRING;

    AboutDlg dlg(this, mainTitle);
    dlg.SetInfo(mainTitle);
    dlg.ShowModal();
}

void clMainFrame::OnClose(wxCloseEvent& event)
{
    // Prompt before exit
    wxStandardID ans =
        PromptForYesNoCancelDialogWithCheckbox(_("Closing CodeLite\n\nSave perspective and exit?"), "SaveAndExit",
                                               "Save and Exit", "Exit without saving", "Don't Exit");
    if(ans == wxID_CANCEL) {
        event.Veto();
        event.Skip(false);
        return;
    }

    if(ans == wxID_YES) {
        if(!SaveLayoutAndSession()) {
            event.Veto();
            event.Skip(false);
            return;
        }
    }
    SaveGeneralSettings();

    event.Skip();

    wxString msg;
    ManagerST::Get()->SetShutdownInProgress(true);

    // Notify the plugins that we are going down
    clCommandEvent eventGoingDown(wxEVT_GOING_DOWN);
    EventNotifier::Get()->ProcessEvent(eventGoingDown);

    // Stop the retag thread
    ParseThreadST::Get()->Stop();

    // Stop the search thread
    ManagerST::Get()->KillProgram();
    SearchThreadST::Get()->StopSearch();

    // Stop any debugging session if any
    IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
    if(debugger && debugger->IsRunning()) ManagerST::Get()->DbgStop();

    // In case we got some data in the clipboard, flush it so it will be available
    // after our process exits
    wxTheClipboard->Flush();
}

void clMainFrame::LoadSession(const wxString& sessionName)
{
    SessionEntry session;
    if(SessionManager::Get().GetSession(sessionName, session)) {
        wxString wspFile = session.GetWorkspaceName();
        if(wspFile.IsEmpty() == false && wspFile != wxT("Default")) {

            // Load the workspace only if it exists
            wxFileName fnWorkspace(wspFile);

            if(fnWorkspace.Exists()) {
                wxCommandEvent eventOpenWorkspace;
                eventOpenWorkspace.SetString(fnWorkspace.GetFullPath());
                OnSwitchWorkspace(eventOpenWorkspace);
            }

        } else {
            // no workspace to open, so just restore any previously open editors
            GetMainBook()->RestoreSession(session);
        }
    }
}

void clMainFrame::OnSave(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) {
        editor->SaveFile();

    } else {

        // delegate it to the plugins
        wxCommandEvent saveEvent(XRCID("save_file"));
        EventNotifier::Get()->ProcessEvent(saveEvent);
    }
}

void clMainFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) { editor->SaveFileAs(); }
}

void clMainFrame::OnFileLoadTabGroup(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString previousgroups;
    EditorConfigST::Get()->GetRecentItems(previousgroups, wxT("RecentTabgroups"));

    // Check the previous items still exist
    for(int n = (int)previousgroups.GetCount() - 1; n >= 0; --n) {
        if(!wxFileName::FileExists(previousgroups.Item(n))) { previousgroups.RemoveAt(n); }
    }
    EditorConfigST::Get()->SetRecentItems(previousgroups, wxT("RecentTabgroups")); // In case any were deleted

    wxString path = ManagerST::Get()->IsWorkspaceOpen() ? clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath()
                                                        : wxGetHomeDir();
    LoadTabGroupDlg dlg(this, path, previousgroups);

    // Disable the 'Replace' checkbox if there aren't any editors to replace
    std::vector<LEditor*> editors;
    GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_Default);
    dlg.EnableReplaceCheck(editors.size());

    if(dlg.ShowModal() != wxID_OK) { return; }

    wxString filepath = dlg.GetListBox()->GetStringSelection();
    wxString sessionFilepath = filepath.BeforeLast(wxT('.'));

    clWindowUpdateLocker locker(this);
    TabGroupEntry session;
    if(SessionManager::Get().GetSession(sessionFilepath, session, "tabgroup", tabgroupTag)) {
        // We've 'loaded' the requested tabs. If required, delete any current ones
        if(dlg.GetReplaceCheck()) { GetMainBook()->CloseAll(true); }
        GetMainBook()->RestoreSession(session);

        // Remove any previous instance of this group from the history, then prepend it and save
        int index = previousgroups.Index(filepath);
        if(index != wxNOT_FOUND) { previousgroups.RemoveAt(index); }
        previousgroups.Insert(filepath, 0);
        EditorConfigST::Get()->SetRecentItems(previousgroups, wxT("RecentTabgroups"));
    }
}

void clMainFrame::OnFileReload(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) {
        if(editor->GetModify()) {
            // Ask user if he really wants to lose all changes
            wxString msg;
            msg << editor->GetFileName().GetFullName() << _(" has been modified, reload file anyway?");
            wxRichMessageDialog dlg(::wxGetTopLevelParent(editor), msg, _("Reload File"),
                                    wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxICON_WARNING);
            if(dlg.ShowModal() != wxID_YES) { return; }
        }
        editor->ReloadFromDisk(true);
    }
}

void clMainFrame::OnCloseWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // let the plugins close any custom workspace
    clCommandEvent e(wxEVT_CMD_CLOSE_WORKSPACE, GetId());
    e.SetEventObject(this);
    EventNotifier::Get()->ProcessEvent(e);

    // In any case, make sure that we dont have a workspace opened
    if(ManagerST::Get()->IsWorkspaceOpen()) { ManagerST::Get()->CloseWorkspace(); }
    ShowWelcomePage();
}

void clMainFrame::OnSwitchWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString wspFile;
    const wxString WSP_EXT = "workspace";

    // Is it an internal command? (usually the workspace file name to load is set in the
    // event.SetString() )
    if(event.GetString().IsEmpty()) {
        // now it is time to prompt user for new workspace to open
        const wxString ALL(wxT("CodeLite Workspace files (*.workspace)|*.workspace|") wxT("All Files (*)|*"));
        wxFileDialog dlg(this, _("Open Workspace"), wxEmptyString, wxEmptyString, ALL,
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
        if(dlg.ShowModal() == wxID_OK) { wspFile = dlg.GetPath(); }
    } else {
        wspFile = event.GetString();
    }

    if(wspFile.IsEmpty()) return;

    // Let the plugins a chance of handling this workspace first
    clCommandEvent e(wxEVT_CMD_OPEN_WORKSPACE, GetId());
    e.SetEventObject(this);
    e.SetFileName(wspFile);
    if(EventNotifier::Get()->ProcessEvent(e)) {
        // the plugin processed it by itself
        return;
    }

    // Make sure that the 'Workspace' tab is visible
    {
        wxCommandEvent showTabEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_workspace_tab"));
        showTabEvent.SetEventObject(this);
        showTabEvent.SetInt(1);
        GetEventHandler()->ProcessEvent(showTabEvent);
    }

    // Open the workspace
    ManagerST::Get()->OpenWorkspace(wspFile);
}

void clMainFrame::OnCompleteWordRefreshList(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) { editor->CompleteWord(true); }
}

void clMainFrame::OnCodeComplete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) { editor->CompleteWord(); }
}

void clMainFrame::OnFunctionCalltip(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) { editor->ShowFunctionTipFromCurrentPos(); }
}

// Open new file
void clMainFrame::OnFileNew(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetMainBook()->NewEditor();
}

void clMainFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
    const wxString ALL(wxT("All Files (*)|*"));

    // Open a file using the current editor's path
    LEditor* editor = GetMainBook()->GetActiveEditor();
    wxString open_path;

    if(editor) {
        open_path = editor->GetFileName().GetPath();
    } else {
        // Could not locate the active editor, use the project
        ProjectPtr project = ManagerST::Get()->GetProject(ManagerST::Get()->GetActiveProjectName());
        if(project) { open_path = project->GetFileName().GetPath(); }
    }

    wxFileDialog* dlg = new wxFileDialog(this, _("Open File"), open_path, wxEmptyString, ALL,
                                         wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
    if(dlg->ShowModal() == wxID_OK) {
        wxArrayString paths;
        dlg->GetPaths(paths);
        for(size_t i = 0; i < paths.GetCount(); i++) {
            GetMainBook()->OpenFile(paths.Item(i));
        }
    }
    dlg->Destroy();
}

void clMainFrame::OnFileClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(GetMainBook()->GetCurrentPage()) {
        wxWindow* winToClose = GetMainBook()->GetCurrentPage();
        GetMainBook()->CallAfter(&MainBook::ClosePageVoid, winToClose);
    }
}

void clMainFrame::OnFileSaveAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetMainBook()->SaveAll(false, true);
}

void clMainFrame::OnFileSaveTabGroup(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString previousgroups;
    EditorConfigST::Get()->GetRecentItems(previousgroups, wxT("RecentTabgroups"));

    SaveTabGroupDlg dlg(this, previousgroups);

    std::vector<LEditor*> editors;
    wxArrayString filepaths;
    GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_RetainOrder |
                                              MainBook::kGetAll_IncludeDetached); // We'll want the order of intArr
                                                                                  // to match the order in
                                                                                  // MainBook::SaveSession
    for(size_t i = 0; i < editors.size(); ++i) {
        filepaths.Add(editors[i]->GetFileName().GetFullPath());
    }
    dlg.SetListTabs(filepaths);

    while(true) {

        if(dlg.ShowModal() != wxID_OK) { return; }

        wxString sessionName = dlg.GetTabgroupName();
        if(sessionName.IsEmpty()) {
            if(wxMessageBox(_("Please enter a name for the tab group"), _("CodeLite"), wxICON_ERROR | wxOK | wxCANCEL,
                            this) != wxOK) {
                return;
            } else {
                continue;
            }
        }

        wxString path = TabGroupsManager::Get()->GetTabgroupDirectory();

        if(path.Right(1) != wxFileName::GetPathSeparator()) { path << wxFileName::GetPathSeparator(); }
        wxString filepath(path + sessionName + wxT(".tabgroup"));
        if(wxFileName::FileExists(filepath)) {
            if(wxMessageBox(_("There is already a file with this name. Do you want to overwrite it?"),
                            _("Are you sure?"), wxICON_EXCLAMATION | wxOK | wxCANCEL, this) != wxOK) {
                return;
            }
        }

        wxArrayInt intArr;
        if(dlg.GetChoices(intArr)) { // Don't bother to save if no tabs were selected
            TabGroupEntry session;
            session.SetTabgroupName(path + sessionName);
            GetMainBook()->SaveSession(session, &intArr);
            SessionManager::Get().Save(session.GetTabgroupName(), session, "tabgroup", tabgroupTag);
            // Add the new tabgroup to the tabgroup manager and pane
            GetWorkspacePane()->GetTabgroupsTab()->AddNewTabgroupToTree(filepath);

            // Remove any previous instance of this group from the history, then prepend it and save
            int index = previousgroups.Index(filepath);
            if(index != wxNOT_FOUND) { previousgroups.RemoveAt(index); }
            previousgroups.Insert(filepath, 0);
            EditorConfigST::Get()->SetRecentItems(previousgroups, wxT("RecentTabgroups"));
            GetStatusBar()->SetMessage(_("Tab group saved"));
        }

        return;
    }
}

void clMainFrame::OnCompleteWordUpdateUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();

    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    // This menu item is enabled only if the current editor belongs to a project
    event.Enable(editor);
}

void clMainFrame::OnWorkspaceOpen(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable(clWorkspaceManager::Get().GetWorkspace());
#if 0
    clCommandEvent e(wxEVT_CMD_IS_WORKSPACE_OPEN, GetId());
    e.SetEventObject(this);
    e.SetAnswer(false);
    EventNotifier::Get()->ProcessEvent(e);

    event.Enable(ManagerST::Get()->IsWorkspaceOpen() || e.IsAnswer());
#endif
}

// Project->New Workspace
void clMainFrame::OnProjectNewWorkspace(wxCommandEvent& event)
{
    // let the plugins handle this event first
    wxArrayString options = clWorkspaceManager::Get().GetAllWorkspaces();
    wxString selection;
    if(options.size() == 1) {
        // only C++ is available
        selection = clCxxWorkspaceST::Get()->GetWorkspaceType();
    } else {
        clSingleChoiceDialog dlg(this, options, 0);
        dlg.SetLabel(_("Select the workspace type:"));
        if(dlg.ShowModal() == wxID_OK) { selection = dlg.GetSelection(); }
    }

    if(selection.IsEmpty()) return;
    if(selection == clCxxWorkspaceST::Get()->GetWorkspaceType()) {
        wxUnusedVar(event);
        NewWorkspaceDlg dlg(this);
        if(dlg.ShowModal() == wxID_OK) {
            wxString fullname = dlg.GetFilePath();
            wxFileName fn(fullname);
            ManagerST::Get()->CreateWorkspace(fn.GetName(), fn.GetPath());
        }
    } else {
        // a pluing workspace, pass it to the plugins
        clCommandEvent e(wxEVT_CMD_CREATE_NEW_WORKSPACE);
        e.SetEventObject(this);
        e.SetString(selection);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

// Project->New Project
void clMainFrame::OnProjectNewProject(wxCommandEvent& event)
{
    // Let the plugin process this request first
    wxUnusedVar(event);
    ManagerST::Get()->ShowNewProjectWizard();
}

void clMainFrame::OnProjectAddProject(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // Prompt user for project path
    const wxString ALL(wxT("CodeLite Projects (*.project)|*.project|") wxT("All Files (*)|*"));
    wxFileDialog* dlg = new wxFileDialog(this, _("Open Project"), wxEmptyString, wxEmptyString, ALL,
                                         wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
    if(dlg->ShowModal() == wxID_OK) {
        // Open it
        ManagerST::Get()->AddProject(dlg->GetPath());
    }
    dlg->Destroy();
}

void clMainFrame::OnReconcileProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ManagerST::Get()->ReconcileProject();
}

void clMainFrame::OnCtagsOptions(wxCommandEvent& event)
{
    wxUnusedVar(event);

    bool colVars(false);
    bool newColVars(false);
    bool caseSensitive(false);

    size_t colourTypes(0);

    colVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
    colourTypes = m_tagsOptionsData.GetCcColourFlags();

    wxArrayString pathsBefore = m_tagsOptionsData.GetParserSearchPaths();
    TagsOptionsDlg dlg(this, m_tagsOptionsData);
    if(dlg.ShowModal() == wxID_OK) {
        m_tagsOptionsData = dlg.GetData();

        // We call here to ToString() only because ToString() internally
        // writes the content into the ctags.replacements file (used by
        // codelite_indexer)
        m_tagsOptionsData.ToString();

        wxArrayString pathsAfter = m_tagsOptionsData.GetParserSearchPaths();
        wxArrayString removedPaths;

        // Compare the paths
        for(size_t i = 0; i < pathsBefore.GetCount(); i++) {
            int where = pathsAfter.Index(pathsBefore.Item(i));
            if(where == wxNOT_FOUND) {
                removedPaths.Add(pathsBefore.Item(i));
            } else {
                pathsAfter.RemoveAt((size_t)where);
            }
        }

        if(removedPaths.IsEmpty() == false) {
            wxWindowDisabler disableAll;
            wxBusyInfo info(_T("Updating tags database, please wait..."), this);
            wxTheApp->Yield();

            // Remove all tags from the database which starts with the paths which were
            // removed from the parser include path
            ITagsStoragePtr db = TagsManagerST::Get()->GetDatabase();
            db->Begin();
            for(size_t i = 0; i < removedPaths.GetCount(); i++) {
                db->DeleteByFilePrefix(wxFileName(), removedPaths.Item(i));
                db->DeleteFromFilesByPrefix(wxFileName(), removedPaths.Item(i));
                wxTheApp->Yield();
            }
            db->Commit();
        }

        newColVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
        caseSensitive = (m_tagsOptionsData.GetFlags() & CC_IS_CASE_SENSITIVE);

        TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);
        TagsManagerST::Get()->GetDatabase()->SetEnableCaseInsensitive(!caseSensitive);

        clConfig ccConfig("code-completion.conf");
        ccConfig.WriteItem(&m_tagsOptionsData);

        // We use this method 'UpdateParserPaths' since it will also update the parser
        // thread with any workspace search/exclude paths related
        ManagerST::Get()->UpdateParserPaths(false);

        TagsManagerST::Get()->GetDatabase()->SetMaxWorkspaceTagToColour(m_tagsOptionsData.GetMaxItemToColour());

        // do we need to colourise?
        if((newColVars != colVars) || (colourTypes != m_tagsOptionsData.GetCcColourFlags())) {
            GetMainBook()->UpdateColours();
        }

        if(pathsAfter.IsEmpty() == false) {
            // a retagg is needed
            wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
            AddPendingEvent(e);
        }

#if HAS_LIBCLANG
        // Clear clang's cache
        ClangCodeCompletion::Instance()->ClearCache();
#endif
        // Update the pre-processor dimming feature
        CodeCompletionManager::Get().RefreshPreProcessorColouring();
    }
}

void clMainFrame::RegisterToolbar(int menuItemId, const wxString& name)
{
    m_toolbars[menuItemId] = name;
    Connect(menuItemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnViewToolbar), NULL, this);
    Connect(menuItemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::OnViewToolbarUI), NULL, this);
}

void clMainFrame::RegisterDockWindow(int menuItemId, const wxString& name)
{
    m_panes[menuItemId] = name;
    Connect(menuItemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnViewPane), NULL, this);
    Connect(menuItemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(clMainFrame::OnViewPaneUI), NULL, this);
}

void clMainFrame::OnViewToolbar(wxCommandEvent& event)
{
    std::map<int, wxString>::iterator iter = m_toolbars.find(event.GetId());
    if(iter != m_toolbars.end()) {
        wxAuiPaneInfo& pane = m_mgr.GetPane(iter->second);
        if(pane.IsOk() && pane.IsToolbar()) {
            pane.Show(event.IsChecked());
            m_mgr.Update();

            // Update the current perspective
            ManagerST::Get()->GetPerspectiveManager().SavePerspective();
        }
    }
}

void clMainFrame::OnViewToolbarUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    std::map<int, wxString>::iterator iter = m_toolbars.find(event.GetId());
    if(iter != m_toolbars.end()) { ViewPaneUI(iter->second, event); }
}

void clMainFrame::OnToggleMainTBars(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ToggleToolBars(true);
}

void clMainFrame::OnTogglePluginTBars(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ToggleToolBars(false);
}

void clMainFrame::ToggleToolBars(bool std)
{
    wxStringSet_t toolbars;
    {
        wxAuiPaneInfoArray& allPanes = m_mgr.GetAllPanes();
        for(size_t i = 0; i < allPanes.GetCount(); ++i) {
            wxAuiPaneInfo& pane = allPanes.Item(i);
            if(!pane.IsOk() || !pane.IsToolbar()) continue;

            if(std) {
                // collect core toolbars
                if(m_coreToolbars.count(pane.name)) toolbars.insert(pane.name);
            } else {
                // collect plugins toolbars
                if(m_coreToolbars.count(pane.name) == 0) toolbars.insert(pane.name);
            }
        }
    }

    if(toolbars.empty()) return;

    // determine that state based on the first toolbar
    bool currentStateVisible = m_mgr.GetPane((*toolbars.begin())).IsShown();

    wxStringSet_t::iterator iter = toolbars.begin();
    for(; iter != toolbars.end(); ++iter) {
        wxString name = *iter;
        wxAuiPaneInfo& pane = m_mgr.GetPane(name);
        pane.Show(!currentStateVisible);
    }
    m_mgr.Update();
}

void clMainFrame::OnViewPane(wxCommandEvent& event)
{
    std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
    if(iter != m_panes.end()) {
        // In >wxGTK-2.9.4 event.GetChecked() is invalid when coming from an accelerator; instead examine the actual
        // state
        wxAuiPaneInfo& info = m_mgr.GetPane(iter->second);
        if(info.IsOk()) { ViewPane(iter->second, !info.IsShown()); }
    }
}

void clMainFrame::OnViewPaneUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
    if(iter != m_panes.end()) { ViewPaneUI(iter->second, event); }
}

void clMainFrame::ViewPane(const wxString& paneName, bool checked)
{
    wxAuiPaneInfo& info = m_mgr.GetPane(paneName);
    if(info.IsOk()) {
        if(checked) {
            DockablePaneMenuManager::HackShowPane(info, &m_mgr);
        } else {
            DockablePaneMenuManager::HackHidePane(true, info, &m_mgr);
        }
    }

    // This is needed in >=wxGTK-2.9, otherwise output pane doesn't fully expand, or on closing the auinotebook doesn't
    // occupy its space
    SendSizeEvent(wxSEND_EVENT_POST);
}

void clMainFrame::ViewPaneUI(const wxString& paneName, wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    wxAuiPaneInfo& info = m_mgr.GetPane(paneName);
    if(info.IsOk()) { event.Check(info.IsShown()); }
}

void clMainFrame::OnViewOptions(wxCommandEvent& WXUNUSED(event))
{
    OptionsDlg2 dlg(this);
    dlg.ShowModal();

    if(dlg.restartRquired) { DoSuggestRestart(); }
}

void clMainFrame::OnTogglePanes(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ManagerST::Get()->TogglePanes();
}

void clMainFrame::OnAddEnvironmentVariable(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EnvironmentVariablesDlg dlg(this);
    dlg.ShowModal();
}

void clMainFrame::OnAdvanceSettings(wxCommandEvent& event)
{
    size_t selected_page(0);
    if(event.GetInt() == 1) { selected_page = 1; }

    AdvancedDlg dlg(this, selected_page);
    if(dlg.ShowModal() == wxID_OK) {
        // mark the whole workspace as dirty so makefile generation will take place
        // force makefile generation upon configuration change
        if(ManagerST::Get()->IsWorkspaceOpen()) {
            wxArrayString projs;
            ManagerST::Get()->GetProjectList(projs);
            for(size_t i = 0; i < projs.GetCount(); i++) {
                ProjectPtr proj = ManagerST::Get()->GetProject(projs.Item(i));
                if(proj) { proj->SetModified(true); }
            }
        }
    }
    SelectBestEnvSet();
}

void clMainFrame::OnBuildEnded(clCommandEvent& event)
{
    event.Skip();

    if(m_buildAndRun) {
        // If the build process was part of a 'Build and Run' command, check whether an erros
        // occurred during build process, if non, launch the output
        m_buildAndRun = false;
        if(ManagerST::Get()->IsBuildEndedSuccessfully() ||
           wxMessageBox(_("Build ended with errors. Continue?"), _("Confirm"), wxYES_NO | wxICON_QUESTION, this) ==
               wxYES) {
            ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
        }
    }
    // Process next command from the queue
    ManagerST::Get()->ProcessCommandQueue();
}

// Build operations
void clMainFrame::OnBuildProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
    if(enable) {

        // Make sure that the working folder is set to the correct path
        wxString workspacePath = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
        ::wxSetWorkingDirectory(workspacePath);
        wxLogMessage("Setting working directory to: %s", workspacePath);
        GetStatusBar()->SetMessage(_("Build starting..."));

        wxString conf, projectName;
        projectName = ManagerST::Get()->GetActiveProjectName();

        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand info(projectName, conf, false, QueueCommand::kBuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Build"));
        }
        ManagerST::Get()->PushQueueCommand(info);
        ManagerST::Get()->ProcessCommandQueue();

        GetStatusBar()->SetMessage("");
    }
}

void clMainFrame::OnBuildCustomTarget(wxCommandEvent& event)
{
    bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
    if(enable) {

        // get the selected configuration to be built
        BuildConfigPtr bldConf =
            clCxxWorkspaceST::Get()->GetProjBuildConf(CustomTargetsMgr::Get().GetProjectName(), wxEmptyString);
        if(bldConf) {
            CustomTargetsMgr::Pair_t target = CustomTargetsMgr::Get().GetTarget(event.GetId());
            if(target.second.IsEmpty()) {
                wxLogMessage(wxString::Format(wxT("%s=%d"), _("Failed to find Custom Build Target for event ID"),
                                              event.GetId()));
                return;
            }

            QueueCommand info(CustomTargetsMgr::Get().GetProjectName(), bldConf->GetName(), false,
                              QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(target.first);

            ManagerST::Get()->PushQueueCommand(info);
            ManagerST::Get()->ProcessCommandQueue();
        }
    }
}

void clMainFrame::OnBuildAndRunProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
    if(enable) {
        m_buildAndRun = true;

        wxString projectName = ManagerST::Get()->GetActiveProjectName();
        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand info(projectName, conf, false, QueueCommand::kBuild);

        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Build"));
        }

        ManagerST::Get()->PushQueueCommand(info);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void clMainFrame::OnRebuildProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    RebuildProject(ManagerST::Get()->GetActiveProjectName());
}

void clMainFrame::OnBuildProjectUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        event.Enable(false);
    } else {
        bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
        event.Enable(enable);
    }
}

void clMainFrame::OnStopExecutedProgramUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    clExecuteEvent e(wxEVT_CMD_IS_PROGRAM_RUNNING, GetId());
    e.SetEventObject(this);
    e.SetAnswer(false);
    EventNotifier::Get()->ProcessEvent(e);

    Manager* mgr = ManagerST::Get();
    bool enable = mgr->IsProgramRunning();
    event.Enable(enable || e.IsAnswer());
}

void clMainFrame::OnStopBuildUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        event.Enable(false);
    } else {
        Manager* mgr = ManagerST::Get();
        bool enable = mgr->IsBuildInProgress();
        event.Enable(enable);
    }
}

void clMainFrame::OnStopBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Manager* mgr = ManagerST::Get();
    if(mgr->IsBuildInProgress()) { mgr->StopBuild(); }
}

void clMainFrame::OnStopExecutedProgram(wxCommandEvent& event)
{
    clExecuteEvent evtExecute(wxEVT_CMD_STOP_EXECUTED_PROGRAM);
    if(EventNotifier::Get()->ProcessEvent(evtExecute)) return;

    wxUnusedVar(event);
    Manager* mgr = ManagerST::Get();
    if(mgr->IsProgramRunning()) { mgr->KillProgram(); }
}

void clMainFrame::OnCleanProject(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString conf, projectName;
    projectName = ManagerST::Get()->GetActiveProjectName();

    QueueCommand buildInfo(QueueCommand::kClean);
    ManagerST::Get()->PushQueueCommand(buildInfo);
    ManagerST::Get()->ProcessCommandQueue();
}

void clMainFrame::OnCleanProjectUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        event.Enable(false);
        return;
    }
    bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
    event.Enable(enable);
}

void clMainFrame::OnExecuteNoDebug(wxCommandEvent& event)
{
    // Sanity
    if(clCxxWorkspaceST::Get()->IsOpen() && !clCxxWorkspaceST::Get()->GetActiveProject()) { return; }

    // Let the plugin process this first
    clExecuteEvent evtExecute(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT);
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        // set the project name
        evtExecute.SetTargetName(clCxxWorkspaceST::Get()->GetActiveProject()->GetName());
    }
    if(EventNotifier::Get()->ProcessEvent(evtExecute)) { return; }

    // Hereon, C++ workspace only

    // Again, sanity
    if(!clCxxWorkspaceST::Get()->IsOpen()) { return; }

    // Prepare the commands to execute
    QueueCommand commandExecute(QueueCommand::kExecuteNoDebug);
    wxStandardID res =
        ::PromptForYesNoDialogWithCheckbox(_("Would you like to build the active project\nbefore executing it?"),
                                           "PromptForBuildBeforeExecute", _("Build and Execute"), _("Execute"));
    // Don't do anything if "X" is pressed
    if(res != wxID_CANCEL) {
        if(res == wxID_YES) {
            QueueCommand buildCommand(QueueCommand::kBuild);
            ManagerST::Get()->PushQueueCommand(buildCommand);
            commandExecute.SetCheckBuildSuccess(true); // execute only if build was successfull
        }

        ManagerST::Get()->PushQueueCommand(commandExecute);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void clMainFrame::OnExecuteNoDebugUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    if(!clWorkspaceManager::Get().GetWorkspace()) {
        event.Enable(false);
        return;
    }

    clExecuteEvent e(wxEVT_CMD_IS_PROGRAM_RUNNING, GetId());
    e.SetEventObject(this);
    e.SetAnswer(false);
    EventNotifier::Get()->ProcessEvent(e);

    bool normalCondition = ManagerST::Get()->GetActiveProjectName().IsEmpty() == false &&
                           !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->IsProgramRunning();
    event.Enable(normalCondition || !e.IsAnswer());
}

void clMainFrame::OnTimer(wxTimerEvent& event)
{
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif

    wxLogMessage(wxString::Format(wxT("Install path: %s"), ManagerST::Get()->GetInstallDir().c_str()));
    wxLogMessage(wxString::Format(wxT("Startup Path: %s"), ManagerST::Get()->GetStartupDirectory().c_str()));
    wxLogMessage("Using " + wxStyledTextCtrl::GetLibraryVersionInfo().ToString());
    if(::clIsCygwinEnvironment()) { wxLogMessage("Running under Cygwin environment"); }

    if(clConfig::Get().Read(kConfigCheckForNewVersion, true)) {
        m_webUpdate = new WebUpdateJob(this, false, clConfig::Get().Read("PromptForNewReleaseOnly", false));
        m_webUpdate->Check();
    }

    // enable/disable plugins toolbar functionality
    PluginManager::Get()->EnableToolbars();

    // Do we need to update the parser paths?
    bool updateParserPaths = clConfig::Get().Read(kConfigUpdateParserPaths, true);
    if(updateParserPaths) {
        UpdateParserSearchPathsFromDefaultCompiler();
        // Now that we have updated them, mark it as done, so next
        // startups we won't do this again
        clConfig::Get().Write(kConfigUpdateParserPaths, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    // clear navigation queue
    if(GetMainBook()->GetCurrentPage() == 0) { NavMgr::Get()->Clear(); }

    // ReTag workspace database if needed (this can happen due to schema version changes)
    // It is important to place the retag code here since the retag workspace should take place after
    // the parser search patha have been updated (if needed)
    if(m_workspaceRetagIsRequired) {
        m_workspaceRetagIsRequired = false;
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
        this->AddPendingEvent(evt);
        GetMainBook()->ShowMessage(_("Your workspace symbols file does not match the current version of CodeLite. "
                                     "CodeLite will perform a full retag of the workspace"));
    }

    // For some reason, under Linux we need to force the menu accelerator again
    // otherwise some shortcuts are getting lose (e.g. Ctrl-/ to comment line)
    ManagerST::Get()->UpdateMenuAccelerators();

    wxModule::InitializeModules();

    // Send initialization end event
    EventNotifier::Get()->PostCommandEvent(wxEVT_INIT_DONE, NULL);
    CallAfter(&clMainFrame::Bootstrap);

    event.Skip();
}

void clMainFrame::UpdateParserSearchPathsFromDefaultCompiler()
{
    // Check that the user has some paths set in the parser
    clConfig ccConfig("code-completion.conf");
    ccConfig.ReadItem(&m_tagsOptionsData);

    // Since the version numbers aren't the same
    // we should merge the new settings with the old ones
    TagsOptionsData tmp;
    m_tagsOptionsData.Merge(tmp);

    // Try to locate the paths automatically
    CompilerPtr pCompiler = BuildSettingsConfigST::Get()->GetDefaultCompiler(wxEmptyString);
    if(!pCompiler) return;

    wxArrayString paths;
    paths = pCompiler->GetDefaultIncludePaths();

    wxArrayString curExcludePaths = m_tagsOptionsData.GetParserExcludePaths();
    wxArrayString curIncluePaths = m_tagsOptionsData.GetParserSearchPaths();

    wxArrayString mergedPaths = ccConfig.MergeArrays(curIncluePaths, paths);
    m_tagsOptionsData.SetParserExcludePaths(curExcludePaths);
    m_tagsOptionsData.SetParserSearchPaths(mergedPaths);
    m_tagsOptionsData.SetVersion(TagsOptionsData::CURRENT_VERSION);

    //-----------------------
    // clang
    //-----------------------

    wxArrayString clangSearchPaths = m_tagsOptionsData.GetClangSearchPathsArray();
    mergedPaths = ccConfig.MergeArrays(paths, clangSearchPaths);
    m_tagsOptionsData.SetClangSearchPathsArray(mergedPaths);
    ccConfig.WriteItem(&m_tagsOptionsData);
}

void clMainFrame::OnFileCloseAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetMainBook()->CallAfter(&MainBook::CloseAllVoid, true);
}

void clMainFrame::OnQuickOutline(wxCommandEvent& event)
{
    // Sanity
    LEditor* activeEditor = GetMainBook()->GetActiveEditor(true);
    CHECK_PTR_RET(activeEditor);

    // let the plugins process this first
    clCodeCompletionEvent evt(wxEVT_CC_SHOW_QUICK_OUTLINE, GetId());
    evt.SetEventObject(this);
    evt.SetEditor(activeEditor);

    if(EventNotifier::Get()->ProcessEvent(evt)) return;

    wxUnusedVar(event);
    if(ManagerST::Get()->IsWorkspaceOpen() == false) return;

    if(activeEditor->GetProject().IsEmpty()) return;

    QuickOutlineDlg dlg(::wxGetTopLevelParent(activeEditor), activeEditor->GetFileName().GetFullPath(), wxID_ANY,
                        wxT(""), wxDefaultPosition, wxSize(400, 400), wxDEFAULT_DIALOG_STYLE);

    dlg.ShowModal();
    activeEditor->SetActive();
}

wxString clMainFrame::CreateWorkspaceTable()
{
    wxString html;
    wxArrayString files;
    Manager* mgr = ManagerST::Get();
    mgr->GetRecentlyOpenedWorkspaces(files);

    wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    bgclr = DrawingUtils::LightColour(bgclr, 7);

    html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
    if(files.GetCount() == 0) {
        html << wxT("<tr><td><font size=2 face=\"Verdana\">");
        html << _("No workspaces found.") << wxT("</font></td></tr>");
    } else {
        wxColour baseColour(232, 166, 101);
        wxColour firstColour = DrawingUtils::LightColour(baseColour, 6);
        wxColour secondColour = DrawingUtils::LightColour(baseColour, 8);
        for(int i = (int)files.GetCount(); i > 0; --i) {
            wxFileName fn(files.Item(i - 1));

            wxColour lineCol;
            if(i % 2) {
                lineCol = firstColour;
            } else {
                lineCol = secondColour;
            }

            html << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">")
                 << wxT("<td><font size=2 face=\"Verdana\">") << wxT("<a href=\"action:open-file:") << fn.GetFullPath()
                 << wxT("\" >") << fn.GetName() << wxT("</a></font></td>") << wxT("<td><font size=2 face=\"Verdana\">")
                 << fn.GetFullPath() << wxT("</font></td>") << wxT("</tr>");
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
    if(files.GetCount() == 0) {
        html << wxT("<tr><td><font size=2 face=\"Verdana\">");
        html << _("No files found.") << wxT("</font></td></tr>");
    } else {
        wxColour baseColour(232, 166, 101);
        wxColour firstColour = DrawingUtils::LightColour(baseColour, 6);
        wxColour secondColour = DrawingUtils::LightColour(baseColour, 8);
        for(int i = (int)files.GetCount(); i > 0; --i) {

            wxFileName fn(files.Item(i - 1));
            wxColour lineCol;
            if(i % 2) {
                lineCol = firstColour;
            } else {
                lineCol = secondColour;
            }
            html << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">")
                 << wxT("<td><font size=2 face=\"Verdana\">") << wxT("<a href=\"action:open-file:") << fn.GetFullPath()
                 << wxT("\" >") << fn.GetFullName() << wxT("</a></font></td>")
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
    FileHistory& hs = GetMainBook()->GetRecentlyOpenedFilesClass();
    GetMainBook()->GetRecentlyOpenedFiles(files);

    wxMenu* menu = NULL;
    wxMenuItem* item = GetMenuBar()->FindItem(XRCID("recent_files"), &menu);
    if(item && menu) {
        wxMenu* submenu = item->GetSubMenu();
        if(submenu) {
            for(size_t i = files.GetCount(); i > 0; --i) {
                hs.AddFileToHistory(files.Item(i - 1));
            }
            // set this menu as the recent file menu
            hs.SetBaseId(RecentFilesSubMenuID + 1);
            hs.UseMenu(submenu);
            hs.AddFilesToMenu();
        }
    }
}

void clMainFrame::CreateRecentlyOpenedWorkspacesMenu()
{
    wxArrayString files;
    FileHistory& hs = ManagerST::Get()->GetRecentlyOpenedWorkspacesClass();
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);

    wxMenu* menu = NULL;
    wxMenuItem* item = GetMenuBar()->FindItem(XRCID("recent_workspaces"), &menu);
    if(item && menu) {
        wxMenu* submenu = item->GetSubMenu();
        if(submenu) {
            for(size_t i = files.GetCount(); i > 0; --i) {
                hs.AddFileToHistory(files.Item(i - 1));
            }
            // set this menu as the recent file menu
            hs.SetBaseId(RecentWorkspaceSubMenuID + 1);
            hs.UseMenu(submenu);
            // Clear any stale items
            wxMenuItemList items = submenu->GetMenuItems();
            wxMenuItemList::reverse_iterator lriter = items.rbegin();
            for(; lriter != items.rend(); ++lriter) {
                submenu->Delete(*lriter);
            }
            // Add entries without their .workspace extension
            hs.AddFilesToMenuWithoutExt();
        }
    }
}

void clMainFrame::OnRecentFile(wxCommandEvent& event)
{
    size_t idx = event.GetId() - (RecentFilesSubMenuID + 1);
    FileHistory& fh = GetMainBook()->GetRecentlyOpenedFilesClass();

    wxArrayString files;
    fh.GetFiles(files);

    if(idx < files.GetCount()) {
        wxString projectName = ManagerST::Get()->GetProjectNameByFile(files.Item(idx));
        clMainFrame::Get()->GetMainBook()->OpenFile(files.Item(idx), projectName);
    }
}

void clMainFrame::OnRecentWorkspace(wxCommandEvent& event)
{
    size_t idx = event.GetId() - (RecentWorkspaceSubMenuID + 1);
    FileHistory& fh = ManagerST::Get()->GetRecentlyOpenedWorkspacesClass();

    wxArrayString files;
    fh.GetFiles(files);

    if(idx < files.GetCount()) {
        wxString file_name(files.Item(idx));

        wxCommandEvent open_workspace_event(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
        open_workspace_event.SetEventObject(this);
        open_workspace_event.SetString(file_name);
        GetEventHandler()->AddPendingEvent(open_workspace_event);
    }
}

void clMainFrame::OnBackwardForward(wxCommandEvent& event)
{
    switch(event.GetId()) {
    case wxID_FORWARD:
        NavMgr::Get()->NavigateForward(PluginManager::Get());
        break;
    case wxID_BACKWARD:
        NavMgr::Get()->NavigateBackward(PluginManager::Get());
        break;
    }
}

void clMainFrame::OnBackwardForwardUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    if(event.GetId() == wxID_FORWARD) {
        event.Enable(NavMgr::Get()->CanNext());
    } else if(event.GetId() == wxID_BACKWARD) {
        event.Enable(NavMgr::Get()->CanPrev());
    } else {
        event.Skip();
    }
}

void clMainFrame::CreateWelcomePage()
{
    WelcomePage* welcomePage = new WelcomePage(GetMainBook());
    GetMainBook()->AddPage(welcomePage, _("Welcome!"), wxEmptyString, wxNullBitmap, true);
    GetMainBook()->Layout();
    // This is needed in >=wxGTK-2.9, otherwise the auinotebook doesn't fully expand at first
    SendSizeEvent(wxSEND_EVENT_POST);
    // Ditto the workspace pane auinotebook
    GetWorkspacePane()->SendSizeEvent(wxSEND_EVENT_POST);
}

void clMainFrame::OnImportMSVS(wxCommandEvent& e)
{
    wxUnusedVar(e);
    const wxString ALL(wxT("All Solution File (*.dsw;*.sln;*.dev;*.bpr;*.cbp;*.workspace)|")
                           wxT("*.dsw;*.sln;*.dev;*.bpr;*.cbp;*.workspace|")
                               wxT("MS Visual Studio Solution File (*.dsw;*.sln)|*.dsw;*.sln|")
                                   wxT("Bloodshed Dev-C++ Solution File (*.dev)|*.dev|")
                                       wxT("Borland C++ Builder Solution File (*.bpr)|*.bpr|")
                                           wxT("Code::Blocks Solution File (*.cbp;*.workspace)|*.cbp;*.workspace"));

    wxFileDialog dlg(this, _("Open IDE Solution/Workspace File"), wxEmptyString, wxEmptyString, ALL,
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
    if(dlg.ShowModal() == wxID_OK) {

        wxArrayString cmps;
        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
        while(cmp) {
            cmps.Add(cmp->GetName());
            cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
        }

        // Get the prefered compiler type
        wxString compilerName = wxGetSingleChoice(_("Select the compiler to use:"), _("Choose compiler"), cmps);
        ManagerST::Get()->ImportMSVSSolution(dlg.GetPath(), compilerName);
    }
}

void clMainFrame::OnDebug(wxCommandEvent& e)
{
    wxUnusedVar(e);

    bool isBuiltinDebuggerRunning =
        DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning();

    if(!clCxxWorkspaceST::Get()->IsOpen()) {
        // We hae no workspace opened and yet we got here.
        // this can mean one of two:
        // 1. A non C++ workspace is opened - so we initiate the debugger start command
        // 2. User wishes to run QuickDebug// Let the plugin know that we are about to start debugging
        clDebugEvent dbgEvent(wxEVT_DBG_UI_START);
        if(EventNotifier::Get()->ProcessEvent(dbgEvent)) {

            // set the debugger features
            m_frameHelper->SetDebuggerFeatures(dbgEvent.GetFeatures());

            // the event was processed by one of the plugins, there is nothing left to
            // be done here
            return;
        }

        // Note:
        // The second scenario (launch QuickDebug) is handled later on this function
    }

    // Enable all features
    m_frameHelper->SetDebuggerFeatures(clDebugEvent::kAllFeatures);

    if(!isBuiltinDebuggerRunning) {
        // Let the plugin know that we are about to start debugging
        clDebugEvent dbgEvent(wxEVT_DBG_UI_CONTINUE);
        ProjectPtr activeProject = clCxxWorkspaceST::Get()->GetActiveProject();
        if(activeProject) {
            BuildConfigPtr buildConfig = activeProject->GetBuildConfiguration();
            if(buildConfig) { dbgEvent.SetDebuggerName(buildConfig->GetDebuggerType()); }
        }
        if(EventNotifier::Get()->ProcessEvent(dbgEvent)) return;
    }

    Manager* mgr = ManagerST::Get();
    if(isBuiltinDebuggerRunning) {
        // Debugger is already running -> continue command
        mgr->DbgContinue();

    } else if(mgr->IsWorkspaceOpen()) {

        if(clCxxWorkspaceST::Get()->GetActiveProjectName().IsEmpty()) {
            wxLogMessage(_("Attempting to debug workspace with no active project? Ignoring."));
            return;
        }

        // Debugger is not running, but workspace is opened -> start debug session
        QueueCommand dbgCmd(QueueCommand::kDebug);

        wxStandardID res =
            ::PromptForYesNoDialogWithCheckbox(_("Would you like to build the project before debugging it?"),
                                               "BuildBeforeDebug", _("Build and Debug"), _("Debug"));
        // Don't do anything if "X" is pressed
        if(res != wxID_CANCEL) {
            if(res == wxID_YES) {
                QueueCommand bldCmd(QueueCommand::kBuild);
                ManagerST::Get()->PushQueueCommand(bldCmd);
                dbgCmd.SetCheckBuildSuccess(true);
            }

            // place a debug command
            ManagerST::Get()->PushQueueCommand(dbgCmd);

            // trigger the commands queue
            ManagerST::Get()->ProcessCommandQueue();
        }
    } else if(!mgr->IsWorkspaceOpen()) {
        // Run the 'Quick Debug'
        OnQuickDebug(e);
    }
}

void clMainFrame::OnDebugUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(!ManagerST::Get()->IsBuildInProgress());
}

void clMainFrame::OnDebugRestart(wxCommandEvent& e)
{
    clDebugEvent event(wxEVT_DBG_UI_RESTART);
    if(EventNotifier::Get()->ProcessEvent(event)) return;

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        GetDebuggerPane()->GetLocalsTable()->Clear();
        dbgr->Restart();
    }
}

void clMainFrame::OnDebugRestartUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();

    clDebugEvent event(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent(event);

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    e.Enable(event.IsAnswer() || (dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()));
}

void clMainFrame::OnDebugStop(wxCommandEvent& e)
{
    wxUnusedVar(e);
    clDebugEvent debugEvent(wxEVT_DBG_UI_STOP);
    if(EventNotifier::Get()->ProcessEvent(debugEvent)) { return; }
    ManagerST::Get()->DbgStop();
}

void clMainFrame::OnDebugStopUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();

    clDebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent(eventIsRunning);

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    e.Enable(eventIsRunning.IsAnswer() || (dbgr && dbgr->IsRunning()));
}

void clMainFrame::OnDebugManageBreakpointsUI(wxUpdateUIEvent& e)
{
    if(e.GetId() == XRCID("delete_all_breakpoints")) {
        std::vector<BreakpointInfo> bps;
        ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(bps);
        e.Enable(bps.size());
    } else if(e.GetId() == XRCID("disable_all_breakpoints")) {
        e.Enable(ManagerST::Get()->GetBreakpointsMgr()->AreThereEnabledBreakpoints());
    } else if(e.GetId() == XRCID("enable_all_breakpoints")) {
        e.Enable(ManagerST::Get()->GetBreakpointsMgr()->AreThereDisabledBreakpoints());
    } else {

        e.Enable(true);
    }
}

void clMainFrame::OnDebugCmd(wxCommandEvent& e)
{
    int cmd(wxNOT_FOUND);
    int eventId(wxNOT_FOUND);

    if(e.GetId() == XRCID("pause_debugger")) {
        cmd = DBG_PAUSE;
        eventId = wxEVT_DBG_UI_INTERRUPT;

    } else if(e.GetId() == XRCID("dbg_stepin")) {
        cmd = DBG_STEPIN;
        eventId = wxEVT_DBG_UI_STEP_IN;

    } else if(e.GetId() == XRCID("dbg_stepi")) {
        cmd = DBG_STEPI;
        eventId = wxEVT_DBG_UI_STEP_I;

    } else if(e.GetId() == XRCID("dbg_stepout")) {
        cmd = DBG_STEPOUT;
        eventId = wxEVT_DBG_UI_STEP_OUT;

    } else if(e.GetId() == XRCID("dbg_next")) {
        cmd = DBG_NEXT;
        eventId = wxEVT_DBG_UI_NEXT;

    } else if(e.GetId() == XRCID("show_cursor")) {
        cmd = DBG_SHOW_CURSOR;
        eventId = wxEVT_DBG_UI_SHOW_CURSOR;

    } else if(e.GetId() == XRCID("dbg_nexti")) {
        cmd = DBG_NEXTI;
        eventId = wxEVT_DBG_UI_NEXT_INST;
    }

    // ALlow the plugins to handle this command first
    clDebugEvent evnt(eventId);
    if(EventNotifier::Get()->ProcessEvent(evnt)) { return; }

    if(cmd != wxNOT_FOUND) { ManagerST::Get()->DbgDoSimpleCommand(cmd); }
}

void clMainFrame::OnDebugCmdUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();

    clDebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent(eventIsRunning);

    if(e.GetId() == XRCID("pause_debugger") || e.GetId() == XRCID("dbg_stepin") || e.GetId() == XRCID("dbg_stepi") || e.GetId() == XRCID("dbg_stepout") ||
       e.GetId() == XRCID("dbg_next") || e.GetId() == XRCID("dbg_nexti") || e.GetId() == XRCID("show_cursor")) {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        e.Enable(eventIsRunning.IsAnswer() || (dbgr && dbgr->IsRunning()));
    }
}

void clMainFrame::OnDebuggerSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DebuggerSettingsDlg* dlg = new DebuggerSettingsDlg(this);
    dlg->ShowModal();
    dlg->Destroy();
}

void clMainFrame::OnIdle(wxIdleEvent& e)
{
    e.Skip();

    // make sure that we are always set to the working directory of the workspace
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        // Check that current working directory is set to the workspace folder
        wxString path = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
        wxString curdir = ::wxGetCwd();
        if(path != curdir) {
            // Check that it really *is* different, not just a symlink issue: see bug #942
            if(CLRealPath(path) != CLRealPath(curdir)) {
                wxLogMessage("Current working directory is reset to %s", path);
                ::wxSetWorkingDirectory(path);
            }
        }
    }
}

void clMainFrame::OnLinkClicked(wxHtmlLinkEvent& e)
{
    wxString action = e.GetLinkInfo().GetHref();
    if(!action.StartsWith(wxT("action:"), &action)) {
        wxLaunchDefaultBrowser(e.GetLinkInfo().GetHref());
        return;
    }
    wxString command = action.BeforeFirst(wxT(':'));
    wxString filename = action.AfterFirst(wxT(':'));
    if(command != wxT("switch-workspace") && command != wxT("open-file") && command != wxT("create-workspace") &&
       command != wxT("import-msvs-solution") && command != wxT("open-workspace")) {
        e.Skip();
        return;
    }

    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("link_action"));
    event.SetEventObject(this);
    StartPageData* data = new StartPageData;
    data->action = command;
    data->file_path = filename;
    if(wxFileName(filename).GetExt() == wxT("workspace")) { data->action = wxT("switch-workspace"); }
    event.SetClientData(data);
    wxPostEvent(this, event);
}

void clMainFrame::OnStartPageEvent(wxCommandEvent& e)
{
    StartPageData* data = (StartPageData*)e.GetClientData();
    if(data->action == wxT("switch-workspace")) {
        clWindowUpdateLocker locker(this);
        ManagerST::Get()->OpenWorkspace(data->file_path);

    } else if(data->action == wxT("open-file")) {
        clMainFrame::Get()->GetMainBook()->OpenFile(data->file_path, wxEmptyString);

    } else if(data->action == wxT("create-workspace")) {
        OnProjectNewWorkspace(e);

    } else if(data->action == wxT("import-msvs-solution")) {
        OnImportMSVS(e);

    } else if(data->action == wxT("open-workspace")) {
        OnSwitchWorkspace(e);
    }
    delete data;
}

void clMainFrame::SetFrameFlag(bool set, int flag)
{
    if(set) {
        m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() | flag);
    } else {
        m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() & ~(flag));
    }
}

void clMainFrame::OnShowWelcomePageUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable(GetMainBook()->FindPage(_("Welcome!")) == NULL);
}

void clMainFrame::OnShowWelcomePage(wxCommandEvent& event) { ShowWelcomePage(); }

void clMainFrame::CompleteInitialization()
{
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif

    // Populate the list of core toolbars before we start loading
    // the plugins
    wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();
    for(size_t i = 0; i < panes.GetCount(); ++i) {
        if(panes.Item(i).IsToolbar()) { m_coreToolbars.insert(panes.Item(i).name); }
    }

    // Register C++ keyboard shortcuts
    clKeyboardManager::Get()->AddGlobalAccelerator("swap_files", "F12", "C++::Swap Header/Implementation file");
    clKeyboardManager::Get()->AddGlobalAccelerator("rename_symbol", "Ctrl-Shift-H", "C++::Rename Symbol");

    // Load the plugins
    PluginManager::Get()->Load();

// Load debuggers (*must* be after the plugins)
#ifdef USE_POSIX_LAYOUT
    wxString plugdir(clStandardPaths::Get().GetPluginsDirectory());
    DebuggerMgr::Get().Initialize(this, EnvironmentConfig::Instance(), plugdir);
#else
    DebuggerMgr::Get().Initialize(this, EnvironmentConfig::Instance(), ManagerST::Get()->GetInstallDir());
#endif
    DebuggerMgr::Get().LoadDebuggers();

    // Connect some system events
    m_mgr.Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(clMainFrame::OnDockablePaneClosed), NULL, this);
    // m_mgr.Connect(wxEVT_AUI_RENDER,     wxAuiManagerEventHandler(clMainFrame::OnAuiManagerRender),   NULL, this);
    Layout();
    SelectBestEnvSet();

    // Now everything is loaded, set the saved tab-order in the workspace and the output pane
    GetWorkspacePane()->ApplySavedTabOrder();
    GetOutputPane()->ApplySavedTabOrder();

    ManagerST::Get()->GetPerspectiveManager().ConnectEvents(&m_mgr);

    wxCommandEvent evt(wxEVT_CL_THEME_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evt);

#ifndef __WXMSW__
    sigset_t child_set;
    sigemptyset(&child_set);
    sigaddset(&child_set, SIGCHLD);

    // make sure SIGCHILD is not blocked
    sigprocmask(SIG_UNBLOCK, &child_set, NULL);

    // Start the Zombie Reaper thread
    m_zombieReaper.Start();
#endif

    // Hide / Show status/tool bar (native)
    DoShowToolbars(clConfig::Get().Read(kConfigShowToolBar, false));

    if(!clConfig::Get().Read(kConfigShowStatusBar, true)) { GetStatusBar()->Show(false); }

    // Hide / Show tab bar
    wxCommandEvent eventShowTabBar;
    eventShowTabBar.SetInt(clConfig::Get().Read(kConfigShowTabBar, true));
    OnShowTabBar(eventShowTabBar);
    ShowOrHideCaptions();

    TabGroupsManager::Get(); // Ensure that the events are binded

    ManagerST::Get()->GetPerspectiveManager().LoadPerspective(NORMAL_LAYOUT);
    m_initCompleted = true;

    if(GetTheApp()->IsStartedInDebuggerMode()) {
        wxCommandEvent quickDebugEvent(wxEVT_MENU, XRCID("quick_debug"));
        quickDebugEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(quickDebugEvent);
    }

    clGotoAnythingManager::Get().Initialise();
}

void clMainFrame::OnAppActivated(wxActivateEvent& e)
{
    if(m_theFrame && e.GetActive()) {

        // if workspace or project was modified, don't prompt for
        // file(s) reload
        if(!m_theFrame->ReloadExternallyModifiedProjectFiles()) {
            m_theFrame->GetMainBook()->CallAfter(&MainBook::ReloadExternallyModified, true);
        }

        // Notify plugins that we got the focus.
        // Some plugins want to hide some frames etc
        wxCommandEvent evtGotFocus(wxEVT_CODELITE_MAINFRAME_GOT_FOCUS);
        EventNotifier::Get()->AddPendingEvent(evtGotFocus);

#ifdef __WXOSX__
        // Set the focus back to the active editor
        LEditor* activeEditor = dynamic_cast<LEditor*>(GetMainBook()->GetActiveEditor());
        if(activeEditor) { activeEditor->CallAfter(&LEditor::SetActive); }
#endif

    } else if(m_theFrame) {

#ifndef __WXMAC__
        /// this code causes crash on Mac, since it destorys an active CCBox
        LEditor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            // we are loosing the focus
            editor->DoCancelCalltip();
        }
#endif
    }

    e.Skip();
}

void clMainFrame::OnCompileFile(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Manager* mgr = ManagerST::Get();
    if(mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
        LEditor* editor = GetMainBook()->GetActiveEditor();
        if(editor && !editor->GetProject().IsEmpty()) {
            mgr->CompileFile(editor->GetProject(), editor->GetFileName().GetFullPath());
        }
    }
}

void clMainFrame::OnCompileFileUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(false);
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        return;
    }
    Manager* mgr = ManagerST::Get();
    if(mgr->IsWorkspaceOpen() && !mgr->IsBuildInProgress()) {
        LEditor* editor = GetMainBook()->GetActiveEditor();
        if(editor && !editor->GetProject().IsEmpty()) { e.Enable(true); }
    }
}

void clMainFrame::OnDebugAttach(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Start the debugger
    Manager* mgr = ManagerST::Get();
    mgr->DbgStart(1);
}

void clMainFrame::OnCloseAllButThis(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxWindow* win = GetMainBook()->GetCurrentPage();
    if(win != NULL) { GetMainBook()->CallAfter(&MainBook::CloseAllButThisVoid, win); }
}

WorkspaceTab* clMainFrame::GetWorkspaceTab() { return GetWorkspacePane()->GetWorkspaceTab(); }

FileExplorer* clMainFrame::GetFileExplorer() { return GetWorkspacePane()->GetFileExplorer(); }

void clMainFrame::OnLoadWelcomePage(wxCommandEvent& event) { SetFrameFlag(event.IsChecked(), CL_SHOW_WELCOME_PAGE); }

void clMainFrame::OnLoadWelcomePageUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_WELCOME_PAGE ? true : false);
}

void clMainFrame::OnFileCloseUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable(GetMainBook()->GetCurrentPage() != NULL);
}

void clMainFrame::OnConvertEol(wxCommandEvent& e)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) {
        int eol(wxSTC_EOL_LF);
        if(e.GetId() == XRCID("convert_eol_win")) {
            eol = wxSTC_EOL_CRLF;
        } else if(e.GetId() == XRCID("convert_eol_mac")) {
            eol = wxSTC_EOL_CR;
        }
        editor->ConvertEOLs(eol);
        editor->SetEOLMode(eol);
    }
}

void clMainFrame::OnViewDisplayEOL(wxCommandEvent& e)
{
    bool visible;
    size_t frame_flags = m_frameGeneralInfo.GetFlags();
    if(e.IsChecked()) {
        frame_flags |= CL_SHOW_EOL;
        visible = true;
    } else {
        frame_flags &= ~CL_SHOW_EOL;
        visible = false;
    }

    m_frameGeneralInfo.SetFlags(frame_flags);
    GetMainBook()->SetViewEOL(visible);
}

void clMainFrame::OnViewDisplayEOL_UI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    LEditor* editor = GetMainBook()->GetActiveEditor();
    bool hasEditor = editor ? true : false;
    if(!hasEditor) {
        e.Enable(false);
        return;
    }

    e.Enable(true);
    e.Check(m_frameGeneralInfo.GetFlags() & CL_SHOW_EOL ? true : false);
}

void clMainFrame::OnCopyFileName(wxCommandEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) {
        wxString fileName = editor->GetFileName().GetFullName();
#if wxUSE_CLIPBOARD
        if(wxTheClipboard->Open()) {
            wxTheClipboard->UsePrimarySelection(false);
            if(!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
                // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
            }
            wxTheClipboard->Close();
        } else {
            wxPrintf(wxT("Failed to open the clipboard"));
        }
#endif
    }
}

void clMainFrame::OnCopyFilePath(wxCommandEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) {
        wxString fileName = editor->GetFileName().GetFullPath();
#if wxUSE_CLIPBOARD
        if(wxTheClipboard->Open()) {
            wxTheClipboard->UsePrimarySelection(false);
            if(!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
                // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
            }
            wxTheClipboard->Close();
        } else {
            wxPrintf(wxT("Failed to open the clipboard"));
        }
#endif
    }
}
void clMainFrame::OnCopyFilePathOnly(wxCommandEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) {
        wxString fileName = editor->GetFileName().GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
#if wxUSE_CLIPBOARD
        if(wxTheClipboard->Open()) {
            wxTheClipboard->UsePrimarySelection(false);
            if(!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
                // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
            }
            wxTheClipboard->Close();
        } else {
            wxPrintf(wxT("Failed to open the clipboard"));
        }
#endif
    }
}

void clMainFrame::OnWorkspaceMenuUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    if(ManagerST::Get()->IsWorkspaceOpen() == false) {
        e.Enable(false);
        return;
    }
    if(ManagerST::Get()->IsBuildInProgress()) {
        e.Enable(false);
        return;
    }
    e.Enable(true);
}

void clMainFrame::OnManagePlugins(wxCommandEvent& e)
{
    PluginMgrDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) { DoSuggestRestart(); }
}

void clMainFrame::OnCppContextMenu(wxCommandEvent& e)
{
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if(editor) { editor->GetContext()->ProcessEvent(e); }
}

void clMainFrame::OnConfigureAccelerators(wxCommandEvent& e)
{
    AccelTableDlg dlg(this);
    dlg.ShowModal();
}

void clMainFrame::OnUpdateBuildRefactorIndexBar(wxCommandEvent& e) { wxUnusedVar(e); }

void clMainFrame::OnHighlightWord(wxCommandEvent& event)
{
    long highlightWord = EditorConfigST::Get()->GetInteger(wxT("highlight_word"), 1);

    // Notify all open editors that word hight is checked
    wxCommandEvent evtEnable(wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT);
    if(!highlightWord) {
        GetMainBook()->HighlightWord(true);
        EditorConfigST::Get()->SetInteger(wxT("highlight_word"), 1);
        evtEnable.SetInt(1);

    } else {
        GetMainBook()->HighlightWord(false);
        EditorConfigST::Get()->SetInteger(wxT("highlight_word"), 0);
        evtEnable.SetInt(0);
    }

    EventNotifier::Get()->AddPendingEvent(evtEnable);
}

void clMainFrame::OnShowNavBar(wxCommandEvent& e)
{
    GetMainBook()->ShowNavBar(e.IsChecked());
    m_mgr.Update();
}

void clMainFrame::OnShowNavBarUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Check(GetMainBook()->IsNavBarShown());
}

void clMainFrame::OnSingleInstanceOpenFiles(clCommandEvent& e)
{
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxFileName fn(files.Item(i));

        // if file is workspace, load it
        if(fn.GetExt() == wxT("workspace")) {
            wxCommandEvent workspaceEvent;
            workspaceEvent.SetString(files.Item(i));
            OnSwitchWorkspace(workspaceEvent);

        } else {
            GetMainBook()->OpenFile(files.Item(i), wxEmptyString);
        }
    }

    CallAfter(&clMainFrame::Raise);
}

void clMainFrame::OnSingleInstanceRaise(clCommandEvent& e)
{
    wxUnusedVar(e);
    Raise();
}

void clMainFrame::OnNewVersionAvailable(wxCommandEvent& e)
{
    clDEBUG() << "clMainFrame::OnNewVersionAvailable called:"
              << (e.GetEventType() == wxEVT_CMD_VERSION_UPTODATE ? "up-to-date" : "new version found") << clEndl;
    if((e.GetEventType() == wxEVT_CMD_VERSION_UPTODATE) && m_webUpdate->IsUserRequest()) {
        // All is up to date
        wxMessageBox(_("You already have the latest version of CodeLite"), "CodeLite", wxOK | wxCENTRE, this);
    } else {
        WebUpdateJobData* data = reinterpret_cast<WebUpdateJobData*>(e.GetClientData());
        if(data) {
            if(data->IsUpToDate() == false) {
                wxRichMessageDialog dlg(this, _("A new version of CodeLite is available for download"), "CodeLite",
                                        wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTRE | wxICON_INFORMATION);
                dlg.SetYesNoLabels(_("Download"), _("No"));
                if(dlg.ShowModal() == wxID_YES) { ::wxLaunchDefaultBrowser(data->GetUrl()); }
            }
            wxDELETE(data);
        }
    }
    wxDELETE(m_webUpdate);
}

void clMainFrame::OnDetachWorkspaceViewTab(wxCommandEvent& e)
{
    size_t sel = GetWorkspacePane()->GetNotebook()->GetSelection();
    wxWindow* page = GetWorkspacePane()->GetNotebook()->GetCurrentPage();
    wxString text = GetWorkspacePane()->GetNotebook()->GetPageText(sel);
    wxBitmap bmp = GetWorkspacePane()->GetNotebook()->GetPageBitmap(
        sel); // We might have a bitmap on the tab, make sure we restore it

    DockablePane* pane = new DockablePane(this, GetWorkspacePane()->GetNotebook(), text, true, bmp, wxSize(200, 200));
    page->Reparent(pane);

    // remove the page from the notebook
    GetWorkspacePane()->GetNotebook()->RemovePage(sel);
    pane->SetChildNoReparent(page);
    wxUnusedVar(e);
}

void clMainFrame::OnHideWorkspaceViewTab(wxCommandEvent& e)
{
    size_t sel = GetWorkspacePane()->GetNotebook()->GetSelection();
    wxString text = GetWorkspacePane()->GetNotebook()->GetPageText(sel);

    clCommandEvent eventHide(wxEVT_SHOW_WORKSPACE_TAB);
    eventHide.SetSelected(false).SetString(text);
    EventNotifier::Get()->AddPendingEvent(eventHide);
}

void clMainFrame::OnNewDetachedPane(wxCommandEvent& e)
{
    DockablePane* pane = (DockablePane*)(e.GetClientData());
    if(pane) {
        wxString text = pane->GetName();
        m_DPmenuMgr->AddMenu(text);

        // keep list of all detached panes
        wxArrayString panes = m_DPmenuMgr->GetDeatchedPanesList();
        DetachedPanesInfo dpi(panes);
        EditorConfigST::Get()->WriteObject(wxT("DetachedPanesList"), &dpi);
    }
}

void clMainFrame::OnDestroyDetachedPane(wxCommandEvent& e)
{
    DockablePane* pane = (DockablePane*)(e.GetClientData());
    if(pane) {
        m_mgr.DetachPane(pane);

        // remove any menu entries for this pane
        m_DPmenuMgr->RemoveMenu(pane->GetName());

        // keep list of all detached panes
        wxArrayString panes = m_DPmenuMgr->GetDeatchedPanesList();
        DetachedPanesInfo dpi(panes);
        EditorConfigST::Get()->WriteObject(wxT("DetachedPanesList"), &dpi);

        pane->Destroy();
        m_mgr.Update();
    }
}

void clMainFrame::OnAuiManagerRender(wxAuiManagerEvent& e)
{
    wxAuiManager* mgr = e.GetManager();
    wxAuiPaneInfoArray& panes = mgr->GetAllPanes();

    wxAcceleratorTable* accelTable = GetAcceleratorTable();
    if(accelTable != NULL) {
        for(size_t i = 0; i < panes.GetCount(); i++) {
            if(panes[i].frame != NULL) { panes[i].frame->SetAcceleratorTable(*accelTable); }
        }
    }
    e.Skip();
}

void clMainFrame::OnDockablePaneClosed(wxAuiManagerEvent& e)
{
    DockablePane* pane = dynamic_cast<DockablePane*>(e.GetPane()->window);
    wxAuiPaneInfo* pInfo = e.GetPane();
    if(pInfo->IsOk()) { DockablePaneMenuManager::HackHidePane(false, *pInfo, &m_mgr); }
    if(pane) {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
        pane->GetEventHandler()->ProcessEvent(evt);
    } else {
        e.Skip();
    }
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

    // let the plugins close any custom workspace
    clCommandEvent e(wxEVT_CMD_RELOAD_WORKSPACE, GetId());
    e.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(e)) return; // this event was handled by a plugin

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) { return; }

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
    if(enable) {
        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        // first we place a clean command
        QueueCommand buildInfo(projectName, conf, false, QueueCommand::kClean);
        if(bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
            buildInfo.SetCustomBuildTarget(wxT("Clean"));
        }
        ManagerST::Get()->PushQueueCommand(buildInfo);

        // now we place a build command
        buildInfo = QueueCommand(projectName, conf, false, QueueCommand::kBuild);

        if(bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
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
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        e.Enable(false);
    } else {
        bool enable = !ManagerST::Get()->IsBuildInProgress() && ManagerST::Get()->IsWorkspaceOpen();
        e.Enable(enable);
    }
}

void clMainFrame::OnBatchBuild(wxCommandEvent& e)
{
    BatchBuildDlg* batchBuild = new BatchBuildDlg(this);
    if(batchBuild->ShowModal() == wxID_OK) {
        // build the projects
        std::list<QueueCommand> buildInfoList;
        batchBuild->GetBuildInfoList(buildInfoList);
        if(buildInfoList.empty() == false) {
            std::list<QueueCommand>::iterator iter = buildInfoList.begin();

            // add all build items to queue
            for(; iter != buildInfoList.end(); iter++) {
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
    if(editor && editor->GetModify()) { title << wxT("*"); }

    wxString pattern = clConfig::Get().Read(kConfigFrameTitlePattern, wxString("$workspace $fullpath"));
    wxString username = ::wxGetUserId();
    username.Prepend("[ ").Append(" ]");

    wxString workspace = clCxxWorkspaceST::Get()->GetName();
    if(!workspace.IsEmpty()) { workspace.Prepend("[ ").Append(" ]"); }

    wxString fullname, fullpath;
    // We support the following macros:
    if(editor) {
        fullname = editor->GetFileName().GetFullName();
        fullpath = editor->GetFileName().GetFullPath();
    }

    pattern.Replace("$workspace", workspace);
    pattern.Replace("$user", username);
    pattern.Replace("$filename", fullname);
    pattern.Replace("$fullpath", fullpath);

    pattern.Trim().Trim(false);
    if(pattern.IsEmpty()) { pattern << "CodeLite"; }

    title << pattern;

    // notify the plugins
    clCommandEvent titleEvent(wxEVT_CL_FRAME_TITLE);
    titleEvent.SetString(title);
    EventNotifier::Get()->ProcessEvent(titleEvent);

    // Update the title
    SetTitle(titleEvent.GetString());
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
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        e.Enable(false);
    } else {
        e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
    }
}

void clMainFrame::OnDetachDebuggerViewTab(wxCommandEvent& e)
{
    size_t sel = GetDebuggerPane()->GetNotebook()->GetSelection();
    wxWindow* page = GetDebuggerPane()->GetNotebook()->GetCurrentPage();
    wxString text = GetDebuggerPane()->GetNotebook()->GetPageText(sel);
    wxBitmap bmp = GetDebuggerPane()->GetNotebook()->GetPageBitmap(sel);

    DockablePane* pane = new DockablePane(this, GetDebuggerPane()->GetNotebook(), text, false, bmp, wxSize(200, 200));
    page->Reparent(pane);

    // remove the page from the notebook
    GetDebuggerPane()->GetNotebook()->RemovePage(sel);
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
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        e.Enable(false);
    } else {
        e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
    }
}

void clMainFrame::OnReBuildWorkspace(wxCommandEvent& e)
{
    wxUnusedVar(e);
    ManagerST::Get()->RebuildWorkspace();
}

void clMainFrame::OnReBuildWorkspaceUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    if(!clWorkspaceManager::Get().GetWorkspace() || !clWorkspaceManager::Get().GetWorkspace()->IsBuildSupported()) {
        e.Enable(false);
    } else {
        e.Enable(ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->IsBuildInProgress());
    }
}

void clMainFrame::OnOpenShellFromFilePath(wxCommandEvent& e)
{
    // get the file path
    wxString filepath;
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) { filepath = editor->GetFileName().GetPath(); }

    if(filepath.IsEmpty()) return;
    DirSaver ds;
    wxSetWorkingDirectory(filepath);

    // Apply the environment variabels before opening the shell
    EnvSetter setter;
    FileUtils::OpenTerminal(filepath);
}

void clMainFrame::ShowWelcomePage()
{
    wxWindow* win = GetMainBook()->FindPage(_("Welcome!"));
    if(win) {
        GetMainBook()->SelectPage(win);
    } else {
        CreateWelcomePage();
    }
}

void clMainFrame::OnSyntaxHighlight(wxCommandEvent& e)
{
    SyntaxHighlightDlg dlg(this);
    dlg.ShowModal();
}

void clMainFrame::OnQuickDebug(wxCommandEvent& e)
{
    // launch the debugger
    QuickDebugDlg dlg(this);
    bool bStartedInDebugMode = GetTheApp()->IsStartedInDebuggerMode();
    if(bStartedInDebugMode || (dlg.ShowModal() == wxID_OK)) {
        // Disable the 'StartedInDebuggerMode' flag - so this will only happen once
        GetTheApp()->SetStartedInDebuggerMode(false);
        DebuggerMgr::Get().SetActiveDebugger(dlg.GetDebuggerName());
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();

        if(dbgr && !dbgr->IsRunning()) {

            std::vector<BreakpointInfo> bpList;
            wxString exepath = bStartedInDebugMode ? GetTheApp()->GetExeToDebug() : dlg.GetExe();
            wxString wd = bStartedInDebugMode ? GetTheApp()->GetDebuggerWorkingDirectory() : dlg.GetWorkingDirectory();
            wxArrayString cmds = bStartedInDebugMode ? wxArrayString() : dlg.GetStartupCmds();

            // update the debugger information
            DebuggerInformation dinfo;
            DebuggerMgr::Get().GetDebuggerInformation(dlg.GetDebuggerName(), dinfo);
            dinfo.breakAtWinMain = true;

            // Allow the quick debug to replace the debugger executable
            if(!bStartedInDebugMode && !dlg.GetAlternateDebuggerExe().IsEmpty()) {
                dinfo.path = dlg.GetAlternateDebuggerExe();
            }

            // read the console command
            dinfo.consoleCommand = EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand();

            // ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints(); TODO: Reimplement this when
            // UpdateBreakpoints() updates only alterations, rather than delete/re-enter

            wxString dbgname = dinfo.path;
            dbgname = EnvironmentConfig::Instance()->ExpandVariables(dbgname, true);

            // launch the debugger
            dbgr->SetObserver(ManagerST::Get());
            dbgr->SetDebuggerInformation(dinfo);

            // TODO: Reimplement this when UpdateBreakpoints() updates only alterations, rather than delete/re-enter
            // GetMainBook()->UpdateBreakpoints();

            // get an updated list of breakpoints
            ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(bpList);

            DebuggerStartupInfo startup_info;
            startup_info.debugger = dbgr;

            // notify plugins that we're about to start debugging
            {
                clDebugEvent eventStarting(wxEVT_DEBUG_STARTING);
                eventStarting.SetClientData(&startup_info);
                if(EventNotifier::Get()->ProcessEvent(eventStarting)) return;
            }

            wxString tty;
#ifndef __WXMSW__
            wxString title;
            title << _("Debugging: ") << exepath << wxT(" ")
                  << (bStartedInDebugMode ? GetTheApp()->GetDebuggerArgs() : dlg.GetArguments());
            if(!ManagerST::Get()->StartTTY(title, tty)) {
                wxMessageBox(_("Could not start TTY console for debugger!"), _("codelite"),
                             wxOK | wxCENTER | wxICON_ERROR);
            }
#endif

            dbgr->SetIsRemoteDebugging(false);

            // Start the debugger
            DebugSessionInfo si;
            si.debuggerPath = dbgname;
            si.exeName = exepath;
            si.cwd = wd;
            si.cmds = cmds;
            si.bpList = bpList;
            si.ttyName = tty;
            si.enablePrettyPrinting = dinfo.enableGDBPrettyPrinting;

            dbgr->Start(si);

            // notify plugins that the debugger just started
            {
                clDebugEvent eventStarted(wxEVT_DEBUG_STARTED);
                eventStarted.SetClientData(&startup_info);
                EventNotifier::Get()->ProcessEvent(eventStarted);
            }

            dbgr->Run(bStartedInDebugMode ? GetTheApp()->GetDebuggerArgs() : dlg.GetArguments(), wxEmptyString);

            // Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
            // so that it uses debugger_ids instead of internal_ids
            clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

            // Layout management
            ManagerST::Get()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
            ManagerST::Get()->GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);

        } else if(!dbgr && !bStartedInDebugMode) {

            // Fire an event, maybe a plugin wants to process this
            clDebugEvent event(wxEVT_DBG_UI_QUICK_DEBUG);
            event.SetDebuggerName(dlg.GetDebuggerName());
            event.SetExecutableName(dlg.GetExe());
            event.SetWorkingDirectory(dlg.GetWorkingDirectory());
            event.SetStartupCommands(wxJoin(dlg.GetStartupCmds(), '\n'));
            event.SetArguments(dlg.GetArguments());
            EventNotifier::Get()->AddPendingEvent(event);
        }
    }
}

void clMainFrame::OnDebugCoreDump(wxCommandEvent& e)
{
    // launch the debugger
    DebugCoreDumpDlg* dlg = new DebugCoreDumpDlg(this);
    if(dlg->ShowModal() == wxID_OK) {

        DebuggerMgr::Get().SetActiveDebugger(dlg->GetDebuggerName());
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();

        if(dbgr && !dbgr->IsRunning()) {

            wxString debuggingcommand;
            debuggingcommand << wxT("-c ") << dlg->GetCore() << wxT(" ") << dlg->GetExe();
            wxString wd = dlg->GetWorkingDirectory();

            // update the debugger information
            DebuggerInformation dinfo;
            DebuggerMgr::Get().GetDebuggerInformation(dlg->GetDebuggerName(), dinfo);
            dinfo.breakAtWinMain = false;

            // read the console command
            dinfo.consoleCommand = EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand();

            wxString dbgname = dinfo.path;
            dbgname = EnvironmentConfig::Instance()->ExpandVariables(dbgname, true);

            // launch the debugger
            dbgr->SetObserver(ManagerST::Get());
            dbgr->SetDebuggerInformation(dinfo);

            DebuggerStartupInfo startup_info;
            startup_info.debugger = dbgr;

            // notify plugins that we're about to start debugging
            {
                clDebugEvent eventStarting(wxEVT_DEBUG_STARTING);
                eventStarting.SetClientData(&startup_info);
                if(EventNotifier::Get()->ProcessEvent(eventStarting)) {
                    dlg->Destroy();
                    return;
                }
            }

            wxString tty;
            wxString title;
            title << "Debugging core: " << dlg->GetCore();
#ifndef __WXMSW__
            if(!ManagerST::Get()->StartTTY(title, tty)) {
                wxMessageBox(_("Could not start TTY console for debugger!"), _("codelite"),
                             wxOK | wxCENTER | wxICON_ERROR);
            }
#endif
            dbgr->SetIsRemoteDebugging(false);

            // The next two are empty, but are required as parameters
            std::vector<BreakpointInfo> bpList;
            wxArrayString cmds;

            DebugSessionInfo si;
            si.debuggerPath = dbgname;
            si.exeName = debuggingcommand;
            si.cwd = wd;
            si.bpList = bpList;
            si.cmds = cmds;
            si.ttyName = tty;
            si.enablePrettyPrinting = dinfo.enableGDBPrettyPrinting;
            dbgr->Start(si);

            // notify plugins that the debugger just started
            {
                clDebugEvent eventStarted(wxEVT_DEBUG_STARTED);
                eventStarted.SetClientData(&startup_info);
                EventNotifier::Get()->ProcessEvent(eventStarted);
            }

            // Coredump debugging doesn't use breakpoints, but probably we should do this here anyway...
            clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

            ManagerST::Get()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
            ManagerST::Get()->GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);

            // Make sure that the debugger pane is visible, and select the stack trace tab
            wxAuiPaneInfo& info = GetDockingManager().GetPane(wxT("Debugger"));
            if(info.IsOk() && !info.IsShown()) { ManagerST::Get()->ShowDebuggerPane(); }

            clMainFrame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::FRAMES);
            ManagerST::Get()->UpdateDebuggerPane();

            // Finally, get the call-stack and 'continue' gdb (which seems to be necessary for things to work...)
            dbgr->ListFrames();
            dbgr->Continue();

        } else if(!dbgr) {
            clDebugEvent event(wxEVT_DBG_UI_CORE_FILE);
            event.SetDebuggerName(dlg->GetDebuggerName());
            event.SetExecutableName(dlg->GetExe());
            event.SetCoreFile(dlg->GetCore());
            event.SetWorkingDirectory(dlg->GetWorkingDirectory());
            EventNotifier::Get()->AddPendingEvent(event);
        }
    }
    dlg->Destroy();
}

void clMainFrame::OnQuickDebugUI(wxUpdateUIEvent& e) // (Also used by DebugCoreDump)
{
    CHECK_SHUTDOWN();
    e.Enable(true);
}

void clMainFrame::OnShowWhitespaceUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    if(e.GetId() == XRCID("whitepsace_invisible")) {
        e.Check(options->GetShowWhitspaces() == 0);
    } else if(e.GetId() == XRCID("whitepsace_always")) {
        e.Check(options->GetShowWhitspaces() == 1);
    } else if(e.GetId() == XRCID("whitespace_visiable_after_indent")) {
        e.Check(options->GetShowWhitspaces() == 2);
    } else if(e.GetId() == XRCID("whitespace_indent_only")) {
        e.Check(options->GetShowWhitspaces() == 3);
    }
}

void clMainFrame::OnShowWhitespace(wxCommandEvent& e)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    if(e.GetId() == XRCID("whitepsace_invisible")) {
        options->SetShowWhitspaces(0);
    } else if(e.GetId() == XRCID("whitepsace_always")) {
        options->SetShowWhitspaces(1);
    } else if(e.GetId() == XRCID("whitespace_visiable_after_indent")) {
        options->SetShowWhitspaces(2);
    } else if(e.GetId() == XRCID("whitespace_indent_only")) {
        options->SetShowWhitspaces(3);
    }

    GetMainBook()->ShowWhitespace(options->GetShowWhitspaces());

    // save the settings
    EditorConfigST::Get()->SetOptions(options);
}

void clMainFrame::OnNextTab(wxCommandEvent& e)
{
    int idx = GetMainBook()->GetCurrentPageIndex();

    if(idx != wxNOT_FOUND) {
        clTab::Vec_t tabs;
        GetMainBook()->GetAllTabs(tabs);

        idx = (idx + 1) % tabs.size();
        GetMainBook()->SelectPage(GetMainBook()->GetPage(idx));
    }
}

void clMainFrame::OnPrevTab(wxCommandEvent& e)
{
    int idx = GetMainBook()->GetCurrentPageIndex();

    if(idx != wxNOT_FOUND) {
        idx--;

        if(idx < 0) {
            clTab::Vec_t tabs;
            GetMainBook()->GetAllTabs(tabs);

            idx = tabs.size() - 1;
        }

        GetMainBook()->SelectPage(GetMainBook()->GetPage(idx));
    }
}

void clMainFrame::OnNextPrevTab_UI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    LEditor* editor = GetMainBook()->GetActiveEditor();
    bool hasEditor = editor ? true : false;
    if(!hasEditor) {
        e.Enable(false);
        return;
    }

    e.Enable(true);
}

void clMainFrame::OnIncrementalSearch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetMainBook()->ShowQuickBar(true, false);
}

void clMainFrame::OnIncrementalReplace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetMainBook()->ShowQuickBar(true, true);
}

void clMainFrame::OnRetagWorkspace(wxCommandEvent& event)
{
    // See if any of the plugins want to handle this event by itself
    bool fullRetag = !(event.GetId() == XRCID("retag_workspace"));
    wxCommandEvent e(fullRetag ? wxEVT_CMD_RETAG_WORKSPACE_FULL : wxEVT_CMD_RETAG_WORKSPACE, GetId());
    e.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(e)) return;

    TagsManager::RetagType type = TagsManager::Retag_Quick_No_Scan;
    if(event.GetId() == XRCID("retag_workspace"))
        type = TagsManager::Retag_Quick;

    else if(event.GetId() == XRCID("full_retag_workspace"))
        type = TagsManager::Retag_Full;

    else if(event.GetId() == XRCID("retag_workspace_no_includes"))
        type = TagsManager::Retag_Quick_No_Scan;

    wxMenu* menu = dynamic_cast<wxMenu*>(event.GetEventObject());
    if(menu) {
        // the event was fired from the menu bar, trigger a compile_commands.json file generation
        // Generate the compile_commands files (needed for Clang)
        ManagerST::Get()->GenerateCompileCommands();
    }
    ManagerST::Get()->RetagWorkspace(type);
}

void clMainFrame::OnShowFullScreen(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(IsFullScreen()) {
        ShowFullScreen(false);

    } else {

        ShowFullScreen(true, wxFULLSCREEN_NOCAPTION | wxFULLSCREEN_NOBORDER);

        // Re-apply the menu accelerators
        ManagerST::Get()->UpdateMenuAccelerators();
    }
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

bool clMainFrame::ReloadExternallyModifiedProjectFiles()
{
    if(ManagerST::Get()->IsWorkspaceOpen() == false) { return false; }

    clCxxWorkspace* workspace = clCxxWorkspaceST::Get();
    bool workspace_modified = false, project_modified = false;

    // check if the workspace needs reloading and ask the user for confirmation
    // if it does
    if(workspace->GetWorkspaceLastModifiedTime() < workspace->GetFileLastModifiedTime()) {
        // always update last modification time: if the user chooses to reload it
        // will not matter, and it avoids the program prompting the user repeatedly
        // if he chooses not to reload the workspace
        workspace->SetWorkspaceLastModifiedTime(workspace->GetFileLastModifiedTime());
        workspace_modified = true;
    }

    // check if any of the projects in the workspace needs reloading
    wxArrayString projects;
    workspace->GetProjectList(projects);

    for(size_t i = 0; i < projects.GetCount(); ++i) {
        wxString errStr;
        ProjectPtr proj = workspace->FindProjectByName(projects[i], errStr);

        if(proj->GetProjectLastModifiedTime() < proj->GetFileLastModifiedTime()) {
            // always update last modification time: if the user chooses to reload it
            // will not matter, and it avoids the program prompting the user repeatedly
            // if he chooses not to reload some of the projects
            proj->SetProjectLastModifiedTime(proj->GetFileLastModifiedTime());
            project_modified = true;
        }
    }

    if(!project_modified && !workspace_modified) return false;

    // Make sure we don't have the mouse captured in any editor or we might get a crash somewhere
    wxStandardID res =
        ::PromptForYesNoDialogWithCheckbox(_("Workspace or project settings have been modified outside "
                                             "of CodeLite\nWould you like to reload the workspace?"),
                                           "ReloadWorkspaceWhenAltered", _("Reload workspace"), _("Not now"));
    // Don't do anything if "X" is pressed
    if(res != wxID_CANCEL) {
        if(res == wxID_YES) {
            wxCommandEvent evtReload(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
            GetEventHandler()->ProcessEvent(evtReload);

        } else {
            // user cancelled the dialog or chosed not to reload the workspace
            if(GetMainBook()->GetActiveEditor()) { GetMainBook()->GetActiveEditor()->CallAfter(&LEditor::SetActive); }
        }
    }
    return true;
}

bool clMainFrame::SaveLayoutAndSession()
{
    // save the current session before closing
    // We do this before 'CloseAll' so the session will
    // store the list of tabs

    // Let the plugin process this first
    clCommandEvent eventSaveSession(wxEVT_SAVE_SESSION_NEEDED);
    if(!EventNotifier::Get()->ProcessEvent(eventSaveSession)) {
        // Do the default session store
        if(ManagerST::Get()->IsWorkspaceOpen()) {
            wxString sessionName = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetFullPath();
            SessionEntry session;
            session.SetWorkspaceName(sessionName);
            GetMainBook()->SaveSession(session);
            ManagerST::Get()->GetBreakpointsMgr()->SaveSession(session);
            SessionManager::Get().Save(sessionName, session);
            SessionManager::Get().SetLastSession(sessionName);
        } else {
            // Create a default session
            wxString sessionName("Default");
            SessionEntry session;
            session.SetWorkspaceName(sessionName);
            GetMainBook()->SaveSession(session);
            ManagerST::Get()->GetBreakpointsMgr()->SaveSession(session);
            SessionManager::Get().Save(sessionName, session);
            SessionManager::Get().SetLastSession(sessionName);
        }
    }

    // make sure there are no 'unsaved documents'
    if(!GetMainBook()->CloseAll(true)) { return false; }

    // save general information
    if(IsMaximized()) {
        m_frameGeneralInfo.SetFrameSize(wxSize(800, 600));
    } else {
        m_frameGeneralInfo.SetFrameSize(this->GetSize());
    }
#ifdef __WXMAC__
    m_frameGeneralInfo.SetFramePosition(wxPoint(50, 50));
#else
    m_frameGeneralInfo.SetFramePosition(this->GetScreenPosition());
#endif

    EditorConfigST::Get()->Begin();

    SetFrameFlag(IsMaximized(), CL_MAXIMIZE_FRAME);
    EditorConfigST::Get()->WriteObject(wxT("GeneralInfo"), &m_frameGeneralInfo);
    EditorConfigST::Get()->SetInteger(wxT("ShowNavBar"), m_mainBook->IsNavBarShown() ? 1 : 0);
    GetWorkspacePane()->SaveWorkspaceViewTabOrder();
    GetOutputPane()->SaveTabOrder();

    // keep list of all detached panes
    wxArrayString panes = m_DPmenuMgr->GetDeatchedPanesList();
    DetachedPanesInfo dpi(panes);
    EditorConfigST::Get()->WriteObject(wxT("DetachedPanesList"), &dpi);

    // Update the current perspective as the "NORMAL" one
    ManagerST::Get()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);

    // save the notebooks styles
    EditorConfigST::Get()->SetInteger(wxT("MainBook"), GetMainBook()->GetBookStyle());
    EditorConfigST::Get()->Save();
    return true;
}

void clMainFrame::SaveGeneralSettings() { EditorConfigST::Get()->WriteObject(wxT("GeneralInfo"), &m_frameGeneralInfo); }

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

void clMainFrame::OnNextFiFMatchUI(wxUpdateUIEvent& e) { CHECK_SHUTDOWN(); }

void clMainFrame::OnPreviousFiFMatchUI(wxUpdateUIEvent& e) { CHECK_SHUTDOWN(); }

void clMainFrame::OnFindResourceXXX(wxCommandEvent& e)
{
    // Determine the search type
    // Let the plugins a chance before we handle this event
    wxCommandEvent eventOpenResource(wxEVT_CMD_OPEN_RESOURCE, GetId());
    eventOpenResource.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(eventOpenResource)) return;

    wxString initialText;
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor && editor->HasSelection()) {
        int start = editor->GetSelectionNStart(0);
        int end = editor->GetSelectionNEnd(0);
        initialText = editor->GetTextRange(start, end);
    }
    OpenResourceDialog dlg(this, PluginManager::Get(), initialText);

    if(dlg.ShowModal() == wxID_OK && !dlg.GetSelections().empty()) {
        std::vector<OpenResourceDialogItemData*> items = dlg.GetSelections();
        std::for_each(items.begin(), items.end(), [&](OpenResourceDialogItemData* item) {
            OpenResourceDialog::OpenSelection(*item, PluginManager::Get());
        });
    }
}

void clMainFrame::OnParsingThreadMessage(wxCommandEvent& e)
{
    wxString* msg = (wxString*)e.GetClientData();
    if(msg) {
        wxLogMessage(*msg);
        delete msg;
    }
}

void clMainFrame::OnDatabaseUpgradeInternally(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
    this->AddPendingEvent(evt);
    // GetMainBook()->ShowMessage(_("Your workspace symbols file does not match the current version of CodeLite.
    // CodeLite will perform a full retag of the workspace"));
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
    TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);

    clConfig ccConfig("code-completion.conf");
    ccConfig.WriteItem(&m_tagsOptionsData);

    ParseThreadST::Get()->SetSearchPaths(tod.GetParserSearchPaths(), tod.GetParserExcludePaths());
}

void clMainFrame::OnCheckForUpdate(wxCommandEvent& e)
{
    if(!m_webUpdate) {
        m_webUpdate = new WebUpdateJob(this, true, clConfig::Get().Read("PromptForNewReleaseOnly", false));
        m_webUpdate->Check();
    }
}

void clMainFrame::OnShowActiveProjectSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!clCxxWorkspaceST::Get()->IsOpen()) { return; }
    GetWorkspaceTab()->OpenProjectSettings();
}

void clMainFrame::OnShowActiveProjectSettingsUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    wxArrayString projectList;
    clCxxWorkspaceST::Get()->GetProjectList(projectList);
    e.Enable(ManagerST::Get()->IsWorkspaceOpen() && (projectList.IsEmpty() == false));
}

void clMainFrame::StartTimer() { m_timer->Start(1000, true); }

void clMainFrame::OnLoadPerspective(wxCommandEvent& e)
{
    wxString file;
    file << clStandardPaths::Get().GetUserDataDir() << wxT("/config/codelite.layout");

    wxFileName oldLayoutFile(file);
    if(oldLayoutFile.FileExists(file)) {
        wxRemoveFile(oldLayoutFile.GetFullPath());
        wxCommandEvent eventRestoreLayout(wxEVT_COMMAND_MENU_SELECTED, XRCID("restore_layout"));
        eventRestoreLayout.SetEventObject(this);
        GetEventHandler()->ProcessEvent(eventRestoreLayout);

    } else {
        ManagerST::Get()->GetPerspectiveManager().LoadPerspective(NORMAL_LAYOUT);

        // Update the current perspective
        if(ManagerST::Get()->GetPerspectiveManager().IsDefaultActive()) {
            ManagerST::Get()->GetPerspectiveManager().SavePerspective();
        }
    }
}

void clMainFrame::SelectBestEnvSet()
{
    ///////////////////////////////////////////////////
    // Select the environment variables set to use
    ///////////////////////////////////////////////////

    // Set the workspace's environment variable set to the active one
    wxString projectSetName;
    wxString projectDbgSetName;

    // First, if the project has an environment which is not '<Use Defaults>' use it
    if(ManagerST::Get()->IsWorkspaceOpen()) {
        wxString activeProj = clCxxWorkspaceST::Get()->GetActiveProjectName();
        ProjectPtr p = ManagerST::Get()->GetProject(activeProj);
        if(p) {
            BuildConfigPtr buildConf = clCxxWorkspaceST::Get()->GetProjBuildConf(activeProj, wxEmptyString);
            if(buildConf) {
                if(buildConf->GetEnvVarSet() != USE_WORKSPACE_ENV_VAR_SET &&
                   buildConf->GetEnvVarSet() != wxT("<Use Workspace Settings>") /* backward support */) {
                    projectSetName = buildConf->GetEnvVarSet();
                }

                if(buildConf->GetDbgEnvSet() != USE_GLOBAL_SETTINGS) { projectDbgSetName = buildConf->GetDbgEnvSet(); }
            }
        }
    }

    wxString workspaceSetName = LocalWorkspaceST::Get()->GetActiveEnvironmentSet();
    wxString globalActiveSet = wxT("Default");
    wxString activeSetName;
    EvnVarList vars = EnvironmentConfig::Instance()->GetSettings();

    // By default, use the global one
    activeSetName = globalActiveSet;

    if(!projectSetName.IsEmpty() && vars.IsSetExist(projectSetName)) {
        activeSetName = projectSetName;

    } else if(!workspaceSetName.IsEmpty() && vars.IsSetExist(workspaceSetName)) {
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
    if(!projectDbgSetName.IsEmpty() && preDefTypeMap.IsSetExist(projectDbgSetName)) { dbgSetName = projectDbgSetName; }

    preDefTypeMap.SetActive(dbgSetName);
    DebuggerConfigTool::Get()->WriteObject(wxT("DebuggerCommands"), &preDefTypeMap);
}

void clMainFrame::OnClearTagsCache(wxCommandEvent& e)
{
    e.Skip();
    TagsManagerST::Get()->ClearTagsCache();
    GetStatusBar()->SetMessage(_("Tags cache cleared"));
}

void clMainFrame::OnUpdateNumberOfBuildProcesses(wxCommandEvent& e)
{
    int cpus = wxThread::GetCPUCount();
    BuilderConfigPtr bs = BuildSettingsConfigST::Get()->GetBuilderConfig("Default");
    if(bs && cpus != wxNOT_FOUND) {
        wxString jobs;
        jobs << cpus;

        bs->SetToolJobs(jobs);
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

void clMainFrame::DoSuggestRestart()
{
#ifdef __WXMAC__
    GetMainBook()->ShowMessage(_("Some of the changes made requires restart of CodeLite"));
#else
    // On Winodws & GTK we offer auto-restart
    ButtonDetails btn1, btn2;
    btn1.buttonLabel = _("Restart Now!");
    btn1.commandId = wxEVT_CMD_RESTART_CODELITE;
    btn1.menuCommand = false;
    btn1.isDefault = true;
    btn1.window = ManagerST::Get();

    // set button window to NULL
    btn2.buttonLabel = _("Not now");
    btn2.window = NULL;

    GetMainBook()->ShowMessage(_("Some of the changes made require a restart of CodeLite. Restart now?"), false,
                               PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/restart")), btn1, btn2);
#endif
}

void clMainFrame::OnRestoreDefaultLayout(wxCommandEvent& e)
{
    e.Skip();

#ifndef __WXMAC__
    clWindowUpdateLocker locker(this);
#endif

    wxLogMessage(wxT("Restoring layout"));

    // Close all docking panes
    wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();

    for(size_t i = 0; i < panes.GetCount(); i++) {
        // make sure that the caption is visible
        panes.Item(i).CaptionVisible(true);
        wxAuiPaneInfo& p = panes.Item(i);

        if(p.window) {
            DockablePane* d = dynamic_cast<DockablePane*>(p.window);
            if(d) {

                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
                p.window->GetEventHandler()->AddPendingEvent(evt);
            }
        }
    }

    ManagerST::Get()->GetPerspectiveManager().DeleteAllPerspectives();

    m_mgr.LoadPerspective(m_defaultLayout, false);
    UpdateAUI();

    // Save the current layout as the 'Default' layout
    ManagerST::Get()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
}

void clMainFrame::SetAUIManagerFlags()
{
    // Set the manager flags
    unsigned int auiMgrFlags = wxAUI_MGR_ALLOW_FLOATING;

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

    auiMgrFlags |= wxAUI_MGR_ALLOW_ACTIVE_PANE;
#if defined(__WXMAC__) || defined(__WXMSW__)
    auiMgrFlags |= wxAUI_MGR_LIVE_RESIZE;
#endif

    m_mgr.SetFlags(auiMgrFlags);
}

void clMainFrame::UpdateAUI()
{
    SetAUIManagerFlags();
    m_mgr.Update();
}

void clMainFrame::OnRetaggingCompelted(wxCommandEvent& e)
{
    e.Skip();
    GetStatusBar()->SetMessage(_("Done"));
    GetWorkspacePane()->ClearProgress();

    // Clear all cached tags now that we got our database updated
    TagsManagerST::Get()->ClearAllCaches();

    // Send event notifying parsing completed
    std::vector<std::string>* files = (std::vector<std::string>*)e.GetClientData();
    if(files) {

        // Print the parsing end time
        wxLogMessage(_("INFO: Retag workspace completed in %ld seconds (%lu files were scanned)"),
                     gStopWatch.Time() / 1000, (unsigned long)files->size());
        std::vector<wxFileName> taggedFiles;
        for(size_t i = 0; i < files->size(); i++) {
            taggedFiles.push_back(wxFileName(wxString(files->at(i).c_str(), wxConvUTF8)));
        }

        SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&taggedFiles);
        delete files;

    } else {
        wxLogMessage(_("INFO: Retag workspace completed in 0 seconds (No files were retagged)"));
    }

    wxCommandEvent tagEndEvent(wxEVT_CMD_RETAG_COMPLETED);
    tagEndEvent.SetClientData(e.GetClientData()); // pass the pointer to the original caller
    EventNotifier::Get()->AddPendingEvent(tagEndEvent);
}

void clMainFrame::OnRetaggingProgress(wxCommandEvent& e)
{
    e.Skip();
    if(e.GetInt() == 1) {
        // parsing started
        gStopWatch.Start();
    }
    GetWorkspacePane()->UpdateProgress(e.GetInt());
}

void clMainFrame::OnRetagWorkspaceUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();

    // See whether we got a custom workspace open in one of the plugins
    clCommandEvent e(wxEVT_CMD_IS_WORKSPACE_OPEN, GetId());
    e.SetEventObject(this);
    e.SetAnswer(false);
    EventNotifier::Get()->ProcessEvent(e);

    event.Enable((ManagerST::Get()->IsWorkspaceOpen() && !ManagerST::Get()->GetRetagInProgress()) || e.IsAnswer());
}

wxString clMainFrame::StartTTY(const wxString& title)
{
#ifndef __WXMSW__

    // Create a new TTY Console and place it in the AUI
    ConsoleFrame* console = new ConsoleFrame(this, PluginManager::Get());
    wxAuiPaneInfo paneInfo;
    paneInfo.Name(wxT("Debugger Console")).Caption(title).Dockable().FloatingSize(300, 200).CloseButton(false);
    m_mgr.AddPane(console, paneInfo);

    // Re-set the title (it might be modified by 'LoadPerspective'
    wxAuiPaneInfo& pi = m_mgr.GetPane(wxT("Debugger Console"));
    if(pi.IsOk()) { pi.Caption(title); }

    wxAuiPaneInfo& info = m_mgr.GetPane(wxT("Debugger Console"));
    if(info.IsShown() == false) {
        info.Show();
        m_mgr.Update();
    }
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
    LEditor* editor = GetMainBook()->GetActiveEditor();
    bool hasEditor = editor ? true : false;
    if(!hasEditor) {
        e.Enable(false);
        return;
    }

    OptionsConfigPtr opts = EditorConfigST::Get()->GetOptions();
    e.Enable(true);
    e.Check(opts->GetWordWrap());
}

void clMainFrame::OnGrepWord(wxCommandEvent& e)
{
    CHECK_SHUTDOWN();
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(!editor || editor->GetSelectedText().IsEmpty()) return;

    // Prepare the search data
    bool singleFileSearch(true);
    if(e.GetId() == XRCID("grep_current_workspace")) singleFileSearch = false;

    SearchData data;
    data.SetFindString(editor->GetSelectedText());
    data.SetMatchCase(true);
    data.SetMatchWholeWord(true);
    data.SetRegularExpression(false);
    data.SetDisplayScope(false);
    data.SetEncoding(wxFontMapper::GetEncodingName(editor->GetOptions()->GetFileFontEncoding()));
    data.SetSkipComments(false);
    data.SetSkipStrings(false);
    data.SetColourComments(false);

    wxArrayString files;
    wxArrayString rootDirs;
    wxString mask;
    if(singleFileSearch) {
        rootDirs.Add(wxGetTranslation(SEARCH_IN_CURRENT_FILE));
        files.Add(editor->GetFileName().GetFullPath());
        mask << editor->GetFileName().GetFullName(); // this will ensure that this file is scanned

    } else {
        rootDirs.Add(wxGetTranslation(SEARCH_IN_WORKSPACE));
        ManagerST::Get()->GetWorkspaceFiles(files);
        wxStringSet_t masks;
        // Build a mask that matches the workspace content
        std::for_each(files.begin(), files.end(), [&](const wxString& filename) {
            wxFileName fn(filename);
            wxString curfileMask = fn.GetExt();
            if(fn.GetExt().IsEmpty()) {
                curfileMask = "*";
            } else {
                curfileMask = "*." + fn.GetExt();
            }

            if(masks.count(curfileMask) == 0) {
                masks.insert(curfileMask);
                mask << curfileMask << ";";
            }
        });
    }

    data.SetRootDirs(rootDirs);
    data.SetFiles(files);
    data.UseNewTab(true);
    data.SetOwner(GetOutputPane()->GetFindResultsTab());
    data.SetExtensions(mask);
    SearchThreadST::Get()->PerformSearch(data);
}

void clMainFrame::OnGrepWordUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(e.GetId() == XRCID("grep_current_workspace")) {
        // grep in workspace
        e.Enable(clWorkspaceManager::Get().IsWorkspaceOpened() && editor && !editor->GetSelectedText().IsEmpty());
    } else {
        // grep in file
        e.Enable(editor && !editor->GetSelectedText().IsEmpty());
    }
}

void clMainFrame::OnPchCacheEnded(wxCommandEvent& e) { e.Skip(); }
void clMainFrame::OnPchCacheStarted(wxCommandEvent& e) { e.Skip(); }

///////////////////// Helper methods /////////////////////////////

void clMainFrame::DoEnableWorkspaceViewFlag(bool enable, int flag)
{
    long flags = View_Show_Default;
    flags = EditorConfigST::Get()->GetInteger(wxT("view_workspace_view"), flags);
    if(enable) {
        flags |= flag;
    } else {
        flags &= ~flag;
    }
    EditorConfigST::Get()->SetInteger(wxT("view_workspace_view"), flags);
}

bool clMainFrame::IsWorkspaceViewFlagEnabled(int flag)
{
    long flags = View_Show_Default;
    flags = EditorConfigST::Get()->GetInteger(wxT("view_workspace_view"), flags);
    return (flags & flag);
}

void clMainFrame::DoUpdatePerspectiveMenu()
{
    // Locate the "perspective_menu"
    wxMenu* menu = NULL;
    GetMenuBar()->FindItem(XRCID("manage_perspectives"), &menu);
    if(!menu) { return; }

    std::vector<int> menuItemIds;
    const wxMenuItemList& items = menu->GetMenuItems();
    wxMenuItemList::const_iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* menuItem = *iter;
        if(menuItem->GetId() == wxID_SEPARATOR || menuItem->GetId() == XRCID("save_current_layout") ||
           menuItem->GetId() == XRCID("manage_perspectives") || menuItem->GetId() == XRCID("restore_layout"))
            continue;
        menuItemIds.push_back(menuItem->GetId());
    }

    for(size_t i = 0; i < menuItemIds.size(); i++) {
        menu->Delete(menuItemIds.at(i));
    }

    wxArrayString perspectives = ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives();
    for(size_t i = 0; i < perspectives.GetCount(); i++) {
        wxString name = perspectives.Item(i);
        menu->Prepend(ManagerST::Get()->GetPerspectiveManager().MenuIdFromName(name), name, wxT(""), true);
    }
}

// Perspective management
void clMainFrame::OnChangePerspective(wxCommandEvent& e)
{
    ManagerST::Get()->GetPerspectiveManager().LoadPerspectiveByMenuId(e.GetId());
}

void clMainFrame::OnManagePerspectives(wxCommandEvent& e)
{
    ManagePerspectivesDlg dlg(this);
    dlg.ShowModal();
    DoUpdatePerspectiveMenu();
}

void clMainFrame::OnSaveLayoutAsPerspective(wxCommandEvent& e)
{
    SavePerspectiveDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK && !dlg.GetPerspectiveName().IsEmpty()) {
        ManagerST::Get()->GetPerspectiveManager().SavePerspective(dlg.GetPerspectiveName());
    }
}

void clMainFrame::OnRefreshPerspectiveMenu(wxCommandEvent& e) { DoUpdatePerspectiveMenu(); }

void clMainFrame::OnChangePerspectiveUI(wxUpdateUIEvent& e)
{
    wxString active = ManagerST::Get()->GetPerspectiveManager().GetActive();
    wxString itemName = ManagerST::Get()->GetPerspectiveManager().NameFromMenuId(e.GetId());

    e.Check(active.CmpNoCase(itemName) == 0);
}

void clMainFrame::OnParserThreadReady(wxCommandEvent& e)
{
    e.Skip();
    ManagerST::Get()->SetRetagInProgress(false);

    if(ManagerST::Get()->IsShutdownInProgress()) {
        // we are in shutdown progress, dont do anything
        return;
    }

    wxUnusedVar(e);
    GetStatusBar()->SetMessage(wxEmptyString);

    if(e.GetInt() == ParseRequest::PR_SUGGEST_HIGHLIGHT_WORDS)
        // no need to trigger another UpdateColour
        return;

    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) { editor->UpdateColours(); }
}

void clMainFrame::OnFileSaveUI(wxUpdateUIEvent& event) { event.Enable(true); }

void clMainFrame::OnActivateEditor(wxCommandEvent& e)
{
    LEditor* editor = dynamic_cast<LEditor*>(e.GetEventObject());
    if(editor) editor->SetActive();
}

void clMainFrame::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    SelectBestEnvSet(); // Updates the statusbar bookmark display
}

void clMainFrame::OnLoadSession(wxCommandEvent& e)
{
    wxUnusedVar(e);
    LoadSession(SessionManager::Get().GetLastSession());
}

void clMainFrame::OnShowBookmarkMenu(wxAuiToolBarEvent& e)
{
    if(e.IsDropDownClicked()) {
        wxMenu* menu = BookmarkManager::Get().CreateBookmarksSubmenu(
            NULL); // Despite the name, this provides almost all the bookmark menuitems
        if(!menu) {
            e.Skip();
            return;
        }
        menu->Append(XRCID("removeall_bookmarks"), _("Remove All Bookmarks")); // This missing one

        wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(e.GetEventObject());
        if(auibar) {
            clAuiToolStickness ts(auibar, e.GetToolId());
            wxRect rect = auibar->GetToolRect(e.GetId());
            wxPoint pt = auibar->ClientToScreen(rect.GetBottomLeft());
            pt = ScreenToClient(pt);
            PopupMenu(menu, pt);
        }

        delete menu;

    } else {
        e.Skip();
    }
}

void clMainFrame::OnShowAuiBuildMenu(wxAuiToolBarEvent& e)
{
    if(e.IsDropDownClicked()) {
        wxMenu menu;
        DoCreateBuildDropDownMenu(&menu);

        wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(e.GetEventObject());
        if(auibar) {
            clAuiToolStickness ts(auibar, e.GetToolId());
            wxRect rect = auibar->GetToolRect(e.GetId());
            wxPoint pt = auibar->ClientToScreen(rect.GetBottomLeft());
            pt = ScreenToClient(pt);
            PopupMenu(&menu, pt);
        }

    } else {
        e.Skip();
    }
}

void clMainFrame::OnUpdateCustomTargetsDropDownMenu(wxCommandEvent& e)
{
    e.Skip();

    wxMenu* buildDropDownMenu = new wxMenu;
    DoCreateBuildDropDownMenu(buildDropDownMenu);
    if(GetMainToolBar() && GetMainToolBar()->FindById(XRCID("build_active_project"))) {
        GetMainToolBar()->SetDropdownMenu(XRCID("build_active_project"), buildDropDownMenu);
    }
}

void clMainFrame::DoCreateBuildDropDownMenu(wxMenu* menu)
{
    menu->Append(XRCID("build_active_project_only"), wxT("Project Only - Build"));
    menu->Append(XRCID("clean_active_project_only"), wxT("Project Only - Clean"));

    // build the menu and show it
    BuildConfigPtr bldcfg =
        clCxxWorkspaceST::Get()->GetProjBuildConf(clCxxWorkspaceST::Get()->GetActiveProjectName(), "");
    if(bldcfg && bldcfg->IsCustomBuild()) {

        // Update teh custom targets
        CustomTargetsMgr::Get().SetTargets(clCxxWorkspaceST::Get()->GetActiveProjectName(), bldcfg->GetCustomTargets());

        if(!CustomTargetsMgr::Get().GetTargets().empty()) { menu->AppendSeparator(); }

        const CustomTargetsMgr::Map_t& targets = CustomTargetsMgr::Get().GetTargets();
        CustomTargetsMgr::Map_t::const_iterator iter = targets.begin();
        for(; iter != targets.end(); ++iter) {
            int winid = iter->first; // contains the menu ID
            menu->Append(winid, iter->second.first);
        }
    }
}

void clMainFrame::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    CustomTargetsMgr::Get().Clear();

    // Reset the menu
    wxMenu* buildDropDownMenu = new wxMenu;
    if(GetMainToolBar() && GetMainToolBar()->FindById(XRCID("build_active_project"))) {
        GetMainToolBar()->SetDropdownMenu(XRCID("build_active_project"), buildDropDownMenu);
    }
}

void clMainFrame::OnIncrementalSearchUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable(m_mainBook->GetCurrentPage() != NULL);
}

void clMainFrame::OnBuildProjectOnly(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_CMD_BUILD_PROJECT_ONLY);
    EventNotifier::Get()->AddPendingEvent(e);
}

void clMainFrame::OnCleanProjectOnly(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_CMD_CLEAN_PROJECT_ONLY);
    EventNotifier::Get()->AddPendingEvent(e);
}

void clMainFrame::OnFileSaveAllUI(wxUpdateUIEvent& event)
{
    bool hasModifiedEditor = false;
    clTab::Vec_t tabs;
    GetMainBook()->GetAllTabs(tabs);
    GetMainBook()->GetDetachedTabs(tabs);

    for(size_t i = 0; i < tabs.size(); ++i) {
        if(tabs.at(i).isFile && tabs.at(i).isModified) {
            hasModifiedEditor = true;
            break;

        } else if(!tabs.at(i).isFile) {
            // Send an event
            clCommandEvent modifyEvent(wxEVT_PAGE_MODIFIED_UPDATE_UI);
            modifyEvent.SetClientData(tabs.at(i).window);
            if(EventNotifier::Get()->ProcessEvent(modifyEvent) && modifyEvent.IsAnswer()) {
                hasModifiedEditor = true;
                break;
            }
        }
    }

    event.Enable(hasModifiedEditor);
}

void clMainFrame::OnShowDebuggerWindow(wxCommandEvent& e)
{
    // load the debugger configuration
    clConfig conf("debugger-view.conf");
    DebuggerPaneConfig item;
    conf.ReadItem(&item);

    bool show = e.IsChecked();
    if(e.GetId() == XRCID("debugger_win_locals")) item.ShowDebuggerWindow(DebuggerPaneConfig::Locals, show);

    if(e.GetId() == XRCID("debugger_win_watches")) item.ShowDebuggerWindow(DebuggerPaneConfig::Watches, show);

    if(e.GetId() == XRCID("debugger_win_output")) item.ShowDebuggerWindow(DebuggerPaneConfig::Output, show);

    if(e.GetId() == XRCID("debugger_win_threads")) item.ShowDebuggerWindow(DebuggerPaneConfig::Threads, show);

    if(e.GetId() == XRCID("debugger_win_callstack")) item.ShowDebuggerWindow(DebuggerPaneConfig::Callstack, show);

    if(e.GetId() == XRCID("debugger_win_memory")) item.ShowDebuggerWindow(DebuggerPaneConfig::Memory, show);

    if(e.GetId() == XRCID("debugger_win_breakpoints")) item.ShowDebuggerWindow(DebuggerPaneConfig::Breakpoints, show);

    if(e.GetId() == XRCID("debugger_win_asciiview")) item.ShowDebuggerWindow(DebuggerPaneConfig::AsciiViewer, show);

    if(e.GetId() == XRCID("debugger_win_disassemble")) item.ShowDebuggerWindow(DebuggerPaneConfig::Disassemble, show);

    conf.WriteItem(&item);
    // Reload the perspective
    ManagerST::Get()->GetPerspectiveManager().LoadPerspective();
}

void clMainFrame::OnShowDebuggerWindowUI(wxUpdateUIEvent& e)
{
    // load the debugger configuration
    // clConfig conf("debugger-view.conf");
    DebuggerPaneConfig item;
    // conf.ReadItem( &item );

    DebuggerPaneConfig::eDebuggerWindows winid = DebuggerPaneConfig::None;

    if(e.GetId() == XRCID("debugger_win_locals")) winid = DebuggerPaneConfig::Locals;

    if(e.GetId() == XRCID("debugger_win_watches")) winid = DebuggerPaneConfig::Watches;

    if(e.GetId() == XRCID("debugger_win_output")) winid = DebuggerPaneConfig::Output;

    if(e.GetId() == XRCID("debugger_win_threads")) winid = DebuggerPaneConfig::Threads;

    if(e.GetId() == XRCID("debugger_win_callstack")) winid = DebuggerPaneConfig::Callstack;

    if(e.GetId() == XRCID("debugger_win_memory")) winid = DebuggerPaneConfig::Memory;

    if(e.GetId() == XRCID("debugger_win_breakpoints")) winid = DebuggerPaneConfig::Breakpoints;

    if(e.GetId() == XRCID("debugger_win_asciiview")) winid = DebuggerPaneConfig::AsciiViewer;

    if(winid != DebuggerPaneConfig::None) { e.Check(item.IsDebuggerWindowShown(winid)); }
}
void clMainFrame::OnRefactoringCacheStatus(wxCommandEvent& e)
{
    e.Skip();
    if(e.GetInt() == 0) {
        // start
        wxLogMessage(wxString() << "Initializing refactoring database for workspace: "
                                << clCxxWorkspaceST::Get()->GetName());
    } else {
        wxLogMessage(wxString() << "Initializing refactoring database for workspace: "
                                << clCxxWorkspaceST::Get()->GetName() << "... done");
    }
}

void clMainFrame::OnThemeChanged(wxCommandEvent& e) { e.Skip(); }

void clMainFrame::OnChangeActiveBookmarkType(wxCommandEvent& e)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) { editor->OnChangeActiveBookmarkType(e); }
}

void clMainFrame::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    SetFrameTitle(GetMainBook()->GetActiveEditor());
    ShowOrHideCaptions();

    LEditor::Vec_t editors;
    GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);

    std::for_each(editors.begin(), editors.end(), [&](LEditor* editor) { editor->PreferencesChanged(); });
}

void clMainFrame::OnDetachEditor(wxCommandEvent& e) { GetMainBook()->DetachActiveEditor(); }

void clMainFrame::OnDetachEditorUI(wxUpdateUIEvent& e) { e.Enable(GetMainBook()->GetActiveEditor() != NULL); }

void clMainFrame::OnShowStatusBar(wxCommandEvent& event)
{
    GetStatusBar()->Show(event.IsChecked());
    SendSizeEvent();
    clConfig::Get().Write(kConfigShowStatusBar, event.IsChecked());
}

void clMainFrame::OnShowStatusBarUI(wxUpdateUIEvent& event) { event.Check(m_frameHelper->IsStatusBarVisible()); }

void clMainFrame::OnShowToolbar(wxCommandEvent& event)
{
    DoShowToolbars(event.IsChecked());
    clConfig::Get().Write(kConfigShowToolBar, event.IsChecked());
}

void clMainFrame::OnShowToolbarUI(wxUpdateUIEvent& event) { event.Check(m_frameHelper->IsToolbarShown()); }

void clMainFrame::ShowOrHideCaptions()
{
    // load the current state
    bool showCaptions = EditorConfigST::Get()->GetOptions()->IsShowDockingWindowCaption();
    DoShowCaptions(showCaptions);
    m_mgr.Update();
    PostSizeEvent();
}

void clMainFrame::OnOpenFileExplorerFromFilePath(wxCommandEvent& e)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if(editor) { FileUtils::OpenFileExplorerAndSelect(editor->GetFileName()); }
}

void clMainFrame::OnSwitchWorkspaceUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable(!clWorkspaceManager::Get().IsWorkspaceOpened());
}

void clMainFrame::OnSplitSelection(wxCommandEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    CHECK_PTR_RET(editor);

    editor->SplitSelection();
}

void clMainFrame::OnSplitSelectionUI(wxUpdateUIEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    event.Enable(editor && editor->HasSelection());
}

void clMainFrame::OnProjectRenamed(clCommandEvent& event)
{
    event.Skip();
    SetFrameTitle(GetMainBook()->GetActiveEditor());
}

void clMainFrame::OnShowTabBar(wxCommandEvent& event)
{
    clConfig::Get().Write(kConfigShowTabBar, event.IsChecked());
    GetMainBook()->ShowTabBar(event.IsChecked());
}

void clMainFrame::OnShowTabBarUI(wxUpdateUIEvent& event) { event.Check(clConfig::Get().Read(kConfigShowTabBar, true)); }

void clMainFrame::OnRunSetupWizard(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!StartSetupWizard()) { GetMainBook()->ApplySettingsChanges(); }
}

void clMainFrame::OnCloseTabsToTheRight(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxWindow* win = GetMainBook()->GetCurrentPage();
    if(win) { GetMainBook()->CallAfter(&MainBook::CloseTabsToTheRight, win); }
}

void clMainFrame::OnMarkEditorReadonly(wxCommandEvent& e)
{
    wxUnusedVar(e);
    LEditor* editor = GetMainBook()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    editor->SetReadOnly(e.IsChecked());
    GetMainBook()->MarkEditorReadOnly(editor);
}

void clMainFrame::OnMarkEditorReadonlyUI(wxUpdateUIEvent& e)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    e.Check(!editor->IsEditable());
}

void clMainFrame::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    // If the workspace tab is visible, make it active
    int where = GetWorkspacePane()->GetNotebook()->GetPageIndex(_("Workspace"));
    if(where != wxNOT_FOUND) { GetWorkspacePane()->GetNotebook()->SetSelection(where); }
}

void clMainFrame::OnFileOpenFolder(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("Select Folder"));
    if(path.IsEmpty()) return;
    GetWorkspacePane()->GetFileExplorer()->OpenFolder(path);
    GetWorkspacePane()->SelectTab(GetWorkspacePane()->GetFileExplorer()->GetCaption());
}

void clMainFrame::OnNewWorkspaceUI(wxUpdateUIEvent& event)
{
    event.Enable(!clWorkspaceManager::Get().IsWorkspaceOpened());
}

void clMainFrame::OnNewProjectUI(wxUpdateUIEvent& event)
{
    event.Enable(clWorkspaceManager::Get().IsWorkspaceOpened() &&
                 clWorkspaceManager::Get().GetWorkspace()->IsProjectSupported());
}

void clMainFrame::OnDebugStarted(clDebugEvent& event)
{
    event.Skip();
    m_toggleToolBar = false;
    if(GetMainToolBar() && !GetMainToolBar()->IsShown()) {
        // We have a native toolbar which is not visible, show it during debug session
        clGetManager()->ShowToolBar();
        m_toggleToolBar = true;
    }
}

void clMainFrame::OnDebugEnded(clDebugEvent& event)
{
    event.Skip();
    if(m_toggleToolBar && GetMainToolBar()) { clGetManager()->ShowToolBar(false); }
    m_toggleToolBar = false;
}

void clMainFrame::OnPrint(wxCommandEvent& event)
{
    if(GetMainBook()->GetActiveEditor(true)) { GetMainBook()->GetActiveEditor(true)->Print(); }
}

void clMainFrame::OnPageSetup(wxCommandEvent& event)
{
    if(GetMainBook()->GetActiveEditor(true)) { GetMainBook()->GetActiveEditor(true)->PageSetup(); }
}

void clMainFrame::OnRecentWorkspaceUI(wxUpdateUIEvent& e)
{
    // We don't allow reloading of recent workspace while another is opened
    e.Enable(!clWorkspaceManager::Get().IsWorkspaceOpened());
}

void clMainFrame::OnToggleReverseDebugging(wxCommandEvent& e)
{
    wxUnusedVar(e);
    // Currently only supported on GDB
    if(DebuggerMgr::Get().GetActiveDebugger()) {
        DebuggerMgr::Get().GetActiveDebugger()->EnableReverseDebugging(e.IsChecked());
    }
}

void clMainFrame::OnToggleReverseDebuggingUI(wxUpdateUIEvent& e)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    bool enable = clCxxWorkspaceST::Get()->IsOpen() && (dbgr && (dbgr->GetName() == "GNU gdb debugger"));
    if(enable && dbgr->IsRecording()) {
        e.Enable(true);
        e.Check(dbgr->IsReverseDebuggingEnabled());
    } else {
        e.Check(false);
        e.Enable(false);
    }
}

void clMainFrame::OnToggleReverseDebuggingRecording(wxCommandEvent& e)
{
    wxUnusedVar(e);
    // Currently only supported on GDB
    if(DebuggerMgr::Get().GetActiveDebugger()) {
        DebuggerMgr::Get().GetActiveDebugger()->EnableRecording(e.IsChecked());
    }
}

void clMainFrame::OnToggleReverseDebuggingRecordingUI(wxUpdateUIEvent& e)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    bool enable = clCxxWorkspaceST::Get()->IsOpen() && (dbgr && (dbgr->GetName() == "GNU gdb debugger") &&
                                                        dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract());
    if(enable) {
        e.Enable(true);
        e.Check(DebuggerMgr::Get().GetActiveDebugger()->IsRecording());
    } else {
        e.Enable(false);
    }
}

void clMainFrame::OnCopyFilePathRelativeToWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(clWorkspaceManager::Get().IsWorkspaceOpened());

    wxFileName fn(editor->GetFileName());
    fn.MakeRelativeTo(clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath());

    ::CopyToClipboard(fn.GetFullPath());
}

void clMainFrame::OnCopyFilePathRelativeToWorkspaceUI(wxUpdateUIEvent& event)
{
    event.Enable(clWorkspaceManager::Get().IsWorkspaceOpened() && clGetManager()->GetActiveEditor());
}

void clMainFrame::SetToolBar(wxToolBar* tb)
{
    if(m_mainToolBar) {
        GetSizer()->Detach(m_mainToolBar);
        m_mainToolBar->Hide();
        wxDELETE(m_mainToolBar);
    }
    m_mainToolBar = tb;

    if(m_mainToolBar) {
        GetSizer()->Insert(0, m_mainToolBar, 0, wxEXPAND);
        Layout();
    }
}

void clMainFrame::InitializeLogo()
{
    BitmapLoader& bmpLoader = *(PluginManager::Get()->GetStdIcons());

    wxString baseLogoName = "-codelite-logo";
#ifdef __WXGTK__
    if(getuid() == 0) {
        // ROOT_INFO_LUID
        baseLogoName = "-codelite-logo-root";
    }
#endif

    wxIconBundle app_icons;
    {
        wxBitmap iconBmp = bmpLoader.LoadBitmap("16" + baseLogoName);
        wxIcon icn;
        icn.CopyFromBitmap(iconBmp);
        app_icons.AddIcon(icn);
    }

    {
        wxBitmap iconBmp = bmpLoader.LoadBitmap("32" + baseLogoName);
        wxIcon icn;
        icn.CopyFromBitmap(iconBmp);
        app_icons.AddIcon(icn);
    }

    {
        wxBitmap iconBmp = bmpLoader.LoadBitmap("64" + baseLogoName);
        wxIcon icn;
        icn.CopyFromBitmap(iconBmp);
        app_icons.AddIcon(icn);
    }

    {
        wxBitmap iconBmp = bmpLoader.LoadBitmap("128" + baseLogoName);
        wxIcon icn;
        icn.CopyFromBitmap(iconBmp);
        app_icons.AddIcon(icn);
    }
    {
        wxBitmap iconBmp = bmpLoader.LoadBitmap("256" + baseLogoName);
        wxIcon icn;
        icn.CopyFromBitmap(iconBmp);
        app_icons.AddIcon(icn);
    }
    SetIcons(app_icons);
}

void clMainFrame::OnDuplicateTab(wxCommandEvent& event)
{
    // Create a new empty tab
    IEditor* currentFile = clGetManager()->GetActiveEditor();
    if(currentFile) {
        IEditor* newEditor = clGetManager()->NewEditor();
        if(newEditor) {
            newEditor->GetCtrl()->SetText(currentFile->GetCtrl()->GetText());
            // Open the 'Save As' dialog, with some sensible defaults
            if(!newEditor->SaveAs(currentFile->GetFileName().GetFullName(), currentFile->GetFileName().GetPath())) {
                // If the "Save As" failed for any reason, remove the current editor
                clGetManager()->CloseEditor(newEditor, false);
            }
        }
    }
}

void clMainFrame::DoShowCaptions(bool show)
{
    if(!show) {
        wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();
        for(size_t i = 0; i < panes.GetCount(); ++i) {
            if(panes.Item(i).IsOk() && !panes.Item(i).IsToolbar()) { panes.Item(i).CaptionVisible(false); }
        }
    } else {
        wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();
        for(size_t i = 0; i < panes.GetCount(); ++i) {
            // Editor is the center pane - don't add it a caption
            if(panes.Item(i).IsOk() && !panes.Item(i).IsToolbar() && panes.Item(i).name != "Editor") {
                panes.Item(i).CaptionVisible(true);
            }
        }
    }
}

void clMainFrame::OnToggleMinimalView(wxCommandEvent& event)
{
    // Hide the toolbars + captions
    // Hide the _native_ toolbar
    bool minimalView = clConfig::Get().Read("MinimalView", true);
    if(minimalView) {
        if(m_frameHelper->IsToolbarShown()) {
            // Hide the toolbar
            DoShowToolbars(false, false);
        }
        if(m_frameHelper->IsCaptionsVisible()) { DoShowCaptions(false); }
    } else {
        if(!m_frameHelper->IsToolbarShown()) { DoShowToolbars(true, false); }
        if(!m_frameHelper->IsCaptionsVisible()) { DoShowCaptions(true); }
    }

    // Update the various configurations
    clConfig::Get().Write(kConfigShowToolBar, !minimalView);
    clConfig::Get().Write("MinimalView", !minimalView); // for next time

    // Update the captions settings
    OptionsConfigPtr opts = EditorConfigST::Get()->GetOptions();
    opts->SetShowDockingWindowCaption(!minimalView);
    EditorConfigST::Get()->SetOptions(opts);

    m_mgr.Update();
    PostSizeEvent();
}

void clMainFrame::OnToggleMinimalViewUI(wxUpdateUIEvent& event)
{
    bool inMinimalView = clConfig::Get().Read("MinimalView", false);
    event.Check(!inMinimalView);
}

void clMainFrame::OnDebugStepInstUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & clDebugEvent::kStepInst);
}

void clMainFrame::OnDebugJumpToCursorUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & clDebugEvent::kJumpToCursor);
}

void clMainFrame::OnDebugRunToCursorUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & clDebugEvent::kRunToCursor);
}

void clMainFrame::OnDebugInterruptUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & clDebugEvent::kInterrupt);
}

void clMainFrame::OnDebugShowCursorUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & clDebugEvent::kShowCursor);
}

void clMainFrame::OnDebugRunToCursor(wxCommandEvent& e)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor && dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        BreakpointInfo bp;
        bp.Create(editor->GetFileName().GetFullPath(), editor->GetCurrentLine() + 1,
                  ManagerST::Get()->GetBreakpointsMgr()->GetNextID());
        bp.bp_type = BP_type_tempbreak;
        dbgr->Break(bp);
        dbgr->Continue();
    }
}

void clMainFrame::OnDebugJumpToCursor(wxCommandEvent& e)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor && dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        dbgr->Jump(editor->GetFileName().GetFullPath(), editor->GetCurrentLine() + 1);
    }
}

CodeLiteApp* clMainFrame::GetTheApp()
{
    CodeLiteApp* app = dynamic_cast<CodeLiteApp*>(wxApp::GetInstance());
    wxASSERT(app);
    return app;
}

void clMainFrame::OnEnvironmentVariablesModified(clCommandEvent& e)
{
    e.Skip();
    SelectBestEnvSet();
    {
        if(clCxxWorkspaceST::Get()->IsOpen()) {
            // mark all the projects as dirty
            wxArrayString projects;
            clCxxWorkspaceST::Get()->GetProjectList(projects);
            for(size_t i = 0; i < projects.size(); i++) {
                ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(projects.Item(i));
                if(proj) { proj->SetModified(true); }
            }
        }
    }
}

void clMainFrame::OnWordComplete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    CHECK_PTR_RET(editor);

    // Get the filter
    wxStyledTextCtrl* stc = editor->GetCtrl();
    int curPos = stc->GetCurrentPos();
    int start = stc->WordStartPosition(stc->GetCurrentPos(), true);
    if(curPos < start) return;
    clCodeCompletionEvent ccEvent(wxEVT_CC_WORD_COMPLETE);
    ccEvent.SetEditor(editor);
    ccEvent.SetEventObject(this);
    ccEvent.SetWord(stc->GetTextRange(start, curPos));
    EventNotifier::Get()->ProcessEvent(ccEvent);

    const wxCodeCompletionBoxEntry::Vec_t& entries = ccEvent.GetEntries();
    if(entries.empty()) return;
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(
        editor->GetCtrl(), entries,
        wxCodeCompletionBox::kNoShowingEvent, // Don't fire the "wxEVT_CCBOX_SHOWING event
        wxNOT_FOUND);
}

void clMainFrame::OnGotoAnything(wxCommandEvent& e) { clGotoAnythingManager::Get().ShowDialog(); }

void clMainFrame::OnVersionCheckError(wxCommandEvent& e)
{
    ::wxMessageBox(e.GetString(), "CodeLite", wxICON_ERROR | wxOK, this);
    wxDELETE(m_webUpdate);
}
