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
#include "my_menu_bar.h"
#include "bitmap_loader.h"
#include <wx/wupdlock.h>
#include "file_logger.h"
#include "event_notifier.h"
#include "cl_aui_tb_are.h"
#include "manage_perspective_dlg.h"
#include "save_perspective_as_dlg.h"
#include "open_resource_dialog.h" // New open resource
#include <wx/busyinfo.h>
#include "tags_parser_search_path_dlg.h"
#include "includepathlocator.h"
#include "localstable.h"
#include "console_frame.h"
#include "build_custom_targets_menu_manager.h"
#include "cl_config.h"
#include "cl_aui_dock_art.h"
#include <wx/splash.h>
#include "clsplashscreen.h"
#include "WelcomePage.h"
#include "code_completion_box.h"
#include "cl_aui_tool_stickness.h"
#include "cl_command_event.h"
#include "refactoring_storage.h"
#include "refactorengine.h"
#include "bookmark_manager.h"
#include <wx/richmsgdlg.h>
#include "code_completion_manager.h"
#include "clang_compilation_db_thread.h"
#include "cl_unredo.h"
#include "NewProjectWizard.h"
#include <CompilersDetectorManager.h>
#include "CompilersFoundDlg.h"

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif
#include "imanager.h"
#include "newversiondlg.h"
#include "quickdebugdlg.h"
#include "debugcoredump.h"
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
#include "jobqueue.h"
#include "acceltabledlg.h"
#include "drawingutils.h"
#include "newworkspacedlg.h"
#include "replaceinfilespanel.h"
#include "pluginmgrdlg.h"
#include "environmentconfig.h"
#include "findresultstab.h"
#include "new_build_tab.h"
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

clSplashScreen* clMainFrame::m_splashScreen = NULL;

//////////////////////////////////////////////////

// from auto-generated file svninfo.cpp:
extern wxString CODELITE_VERSION_STR;
extern const wxChar *clGitRevision;

static wxStopWatch gStopWatch;

// from iconsextra.cpp:
extern char *cubes_xpm[];
extern unsigned char cubes_alpha[];

static int FrameTimerId = wxNewId();

const wxEventType wxEVT_UPDATE_STATUS_BAR        = XRCID("update_status_bar");
const wxEventType wxEVT_LOAD_PERSPECTIVE         = XRCID("load_perspective");
const wxEventType wxEVT_REFRESH_PERSPECTIVE_MENU = XRCID("refresh_perspective_menu");
const wxEventType wxEVT_ACTIVATE_EDITOR          = XRCID("activate_editor");
const wxEventType wxEVT_LOAD_SESSION             = ::wxNewEventType();

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

#ifdef __WXMAC__
#   define TB_SEPARATOR()
#else
#   define TB_SEPARATOR() tb->AddSeparator()
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
    EVT_IDLE(clMainFrame::OnIdle)
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
    EVT_MENU_RANGE(ID_MENU_CUSTOM_TARGET_FIRST, ID_MENU_CUSTOM_TARGET_MAX, clMainFrame::OnBuildCustomTarget)
    EVT_MENU(wxID_EXIT,                         clMainFrame::OnQuit)

    EVT_UPDATE_UI(XRCID("refresh_file"),        clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("save_file"),           clMainFrame::OnFileSaveUI)
    EVT_UPDATE_UI(XRCID("save_file_as"),        clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("save_all"),            clMainFrame::OnFileSaveAllUI)
    EVT_UPDATE_UI(XRCID("save_tab_group"),      clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("close_file"),          clMainFrame::OnFileCloseUI)

    //--------------------------------------------------
    // Edit menu
    //--------------------------------------------------
    EVT_MENU(wxID_UNDO,                           clMainFrame::DispatchCommandEvent)
    EVT_MENU(wxID_REDO,                           clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("label_current_state"),        clMainFrame::DispatchCommandEvent)
    EVT_MENU(wxID_DUPLICATE,                      clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_line"),                clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_line_end"),            clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_line_start"),          clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("transpose_lines"),            clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("trim_trailing"),              clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("to_upper"),                   clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("to_lower"),                   clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("match_brace"),                clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("select_to_brace"),            clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("complete_word"),              clMainFrame::OnCompleteWord)
    EVT_MENU(XRCID("complete_word_refresh_list"), clMainFrame::OnCompleteWordRefreshList)
    EVT_MENU(XRCID("function_call_tip"),          clMainFrame::OnFunctionCalltip)
    EVT_MENU(XRCID("convert_eol_win"),            clMainFrame::OnConvertEol)
    EVT_MENU(XRCID("convert_eol_unix"),           clMainFrame::OnConvertEol)
    EVT_MENU(XRCID("convert_eol_mac"),            clMainFrame::OnConvertEol)
    EVT_MENU(XRCID("move_line_down"),             clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("move_line_up"),               clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("center_line"),                clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("center_line_roll"),           clMainFrame::DispatchCommandEvent)

    EVT_UPDATE_UI(wxID_UNDO,                    clMainFrame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(wxID_REDO,                    clMainFrame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(wxID_DUPLICATE,               clMainFrame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("delete_line"),         clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("delete_line_end"),     clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("delete_line_start"),   clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("transpose_lines"),     clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("trim_trailing"),       clMainFrame::DispatchUpdateUIEvent)
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
    EVT_UPDATE_UI(XRCID("center_line_roll"),    clMainFrame::OnFileExistUpdateUI)

    //-------------------------------------------------------
    // View menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("restore_layout"),           clMainFrame::OnRestoreDefaultLayout)
    EVT_MENU(XRCID("word_wrap"),                clMainFrame::OnViewWordWrap)
    EVT_MENU(XRCID("toggle_fold"),              clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("fold_all"),                 clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("fold_all_in_selection"),    clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("fold_topmost_in_selection"),clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("display_eol"),              clMainFrame::OnViewDisplayEOL)
    EVT_MENU(XRCID("whitepsace_invisible"),     clMainFrame::OnShowWhitespace)
    EVT_MENU(XRCID("whitepsace_always"),        clMainFrame::OnShowWhitespace)
    EVT_MENU(XRCID("whitespace_visiable_after_indent"), clMainFrame::OnShowWhitespace)
    EVT_MENU(XRCID("whitespace_indent_only"),   clMainFrame::OnShowWhitespace)
    EVT_MENU(XRCID("full_screen"),              clMainFrame::OnShowFullScreen)
    EVT_MENU(XRCID("view_welcome_page"),        clMainFrame::OnShowWelcomePage)
    EVT_MENU(XRCID("view_welcome_page_at_startup"), clMainFrame::OnLoadWelcomePage)
    EVT_MENU(XRCID("show_nav_toolbar"),         clMainFrame::OnShowNavBar)
    EVT_MENU(XRCID("toogle_main_toolbars"),     clMainFrame::OnToggleMainTBars)
    EVT_MENU(XRCID("toogle_plugin_toolbars"),   clMainFrame::OnTogglePluginTBars)
    EVT_MENU(XRCID("toggle_panes"),             clMainFrame::OnTogglePanes)
    EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, clMainFrame::DispatchCommandEvent)

    EVT_UPDATE_UI(XRCID("word_wrap"),           clMainFrame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("toggle_fold"),         clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("fold_all"),            clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("fold_all_in_selection"), clMainFrame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("fold_topmost_in_selection"), clMainFrame::DispatchUpdateUIEvent)
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

    EVT_MENU(XRCID("show_workspace_tab"), clMainFrame::OnViewShowWorkspaceTab)
    EVT_MENU(XRCID("show_explorer_tab"),  clMainFrame::OnViewShowExplorerTab)
    EVT_MENU(XRCID("show_tabs_tab"),      clMainFrame::OnViewShowTabs)
    EVT_MENU(XRCID("show_tabgroups_tab"), clMainFrame::OnViewShowTabgroups)

    EVT_UPDATE_UI(XRCID("show_workspace_tab"), clMainFrame::OnViewShowWorkspaceTabUI)
    EVT_UPDATE_UI(XRCID("show_explorer_tab"),  clMainFrame::OnViewShowExplorerTabUI)
    EVT_UPDATE_UI(XRCID("show_tabs_tab"),      clMainFrame::OnViewShowTabsUI)
    EVT_UPDATE_UI(XRCID("show_tabgroups_tab"), clMainFrame::OnViewShowTabgroupsUI)

    //-------------------------------------------------------
    // Search menu
    //-------------------------------------------------------
    EVT_MENU(wxID_FIND,                             clMainFrame::DispatchCommandEvent  )
    EVT_MENU(wxID_REPLACE,                          clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("find_resource"),                clMainFrame::OnFindResourceXXX     )
    EVT_MENU(XRCID("incremental_search"),           clMainFrame::OnIncrementalSearch   )
    EVT_MENU(XRCID("find_symbol"),                  clMainFrame::OnQuickOutline        )
    EVT_MENU(XRCID("goto_definition"),              clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("goto_previous_definition"),     clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("goto_linenumber"),              clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("toggle_bookmark"),              clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("next_bookmark"),                clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("previous_bookmark"),            clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("removeall_current_bookmarks"),  clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("removeall_bookmarks"),          clMainFrame::DispatchCommandEvent  )
    EVT_MENU(XRCID("next_fif_match"),               clMainFrame::OnNextFiFMatch        )
    EVT_MENU(XRCID("previous_fif_match"),           clMainFrame::OnPreviousFiFMatch    )
    EVT_MENU(XRCID("grep_current_file"),            clMainFrame::OnGrepWord            )
    EVT_MENU(XRCID("grep_current_workspace"),       clMainFrame::OnGrepWord            )

    EVT_AUITOOLBAR_TOOL_DROPDOWN(XRCID("toggle_bookmark"),clMainFrame::OnShowBookmarkMenu)

    EVT_UPDATE_UI(wxID_FIND,                        clMainFrame::OnIncrementalSearchUI )
    EVT_UPDATE_UI(wxID_REPLACE,                     clMainFrame::OnFileExistUpdateUI   )
    EVT_UPDATE_UI(XRCID("find_next"),               clMainFrame::OnFileExistUpdateUI   )
    EVT_UPDATE_UI(XRCID("find_previous"),           clMainFrame::OnFileExistUpdateUI   )
    EVT_UPDATE_UI(XRCID("find_next_at_caret"),      clMainFrame::OnFileExistUpdateUI   )
    EVT_UPDATE_UI(XRCID("find_previous_at_caret"),  clMainFrame::OnFileExistUpdateUI   )
    EVT_UPDATE_UI(XRCID("incremental_search"),      clMainFrame::OnIncrementalSearchUI )
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
    EVT_UPDATE_UI(XRCID("grep_current_file"),       clMainFrame::OnGrepWordUI          )
    EVT_UPDATE_UI(XRCID("grep_current_workspace"),  clMainFrame::OnGrepWordUI          )

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
    EVT_MENU(XRCID("reconcile_project"),        clMainFrame::OnReconcileProject)
    EVT_MENU(XRCID("retag_workspace"),          clMainFrame::OnRetagWorkspace)
    EVT_MENU(XRCID("full_retag_workspace"),     clMainFrame::OnRetagWorkspace)
    EVT_MENU(XRCID("project_properties"),       clMainFrame::OnShowActiveProjectSettings)

    EVT_UPDATE_UI(XRCID("local_workspace_prefs"),    clMainFrame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("local_workspace_settings"), clMainFrame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("close_workspace"),          clMainFrame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("reload_workspace"),         clMainFrame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("add_project"),              clMainFrame::OnWorkspaceMenuUI)
    EVT_UPDATE_UI(XRCID("reconcile_project"),        clMainFrame::OnShowActiveProjectSettingsUI)
    EVT_UPDATE_UI(XRCID("retag_workspace"),          clMainFrame::OnRetagWorkspaceUI)
    EVT_UPDATE_UI(XRCID("full_retag_workspace"),     clMainFrame::OnRetagWorkspaceUI)
    EVT_UPDATE_UI(XRCID("project_properties"),       clMainFrame::OnShowActiveProjectSettingsUI)

    //-------------------------------------------------------
    // Build menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("execute_no_debug"),         clMainFrame::OnExecuteNoDebug)
    EVT_MENU(XRCID("stop_executed_program"),    clMainFrame::OnStopExecutedProgram)
    EVT_MENU(XRCID("build_active_project"),     clMainFrame::OnBuildProject)
    EVT_MENU(XRCID("build_active_project_only"),     clMainFrame::OnBuildProjectOnly)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(XRCID("build_active_project"), clMainFrame::OnShowAuiBuildMenu)
    EVT_MENU(XRCID("compile_active_file"),      clMainFrame::OnCompileFile)
    EVT_MENU(XRCID("clean_active_project"),     clMainFrame::OnCleanProject)
    EVT_MENU(XRCID("clean_active_project_only"),     clMainFrame::OnCleanProjectOnly)
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
    EVT_MENU(XRCID("debugger_win_locals"),      clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_watches"),     clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_output"),      clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_threads"),     clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_callstack"),   clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_memory"),      clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_breakpoints"), clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_asciiview"),   clMainFrame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_disassemble"), clMainFrame::OnShowDebuggerWindow)
    EVT_UPDATE_UI(XRCID("debugger_win_locals"),      clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_watches"),     clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_output"),      clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_threads"),     clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_callstack"),   clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_memory"),      clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_breakpoints"), clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_asciiview"),   clMainFrame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_disassemble"), clMainFrame::OnShowDebuggerWindowUI)
    EVT_MENU(XRCID("start_debugger"),           clMainFrame::OnDebug)
    EVT_MENU(XRCID("restart_debugger"),         clMainFrame::OnDebugRestart)
    EVT_MENU(XRCID("attach_debugger"),          clMainFrame::OnDebugAttach)
    EVT_MENU(XRCID("pause_debugger"),           clMainFrame::OnDebugCmd)
    EVT_MENU(XRCID("stop_debugger"),            clMainFrame::OnDebugStop)
    EVT_MENU(XRCID("dbg_stepin"),               clMainFrame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_stepout"),              clMainFrame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_next"),                 clMainFrame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_nexti"),                clMainFrame::OnDebugCmd)
    EVT_MENU(XRCID("show_cursor"),              clMainFrame::OnDebugCmd)
    EVT_MENU(XRCID("add_breakpoint"),           clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_breakpoint"),        clMainFrame::DispatchCommandEvent) // Toggles
    EVT_MENU(XRCID("disable_all_breakpoints"),  clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("enable_all_breakpoints"),  	clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_all_breakpoints"),  	clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_temp_breakpoint"),   clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_disabled_breakpoint"),clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_cond_breakpoint"),   clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("edit_breakpoint"),          clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("show_breakpoint_dlg"),      clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_watchpoint"),        clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("toggle_breakpoint_enabled_status"),  clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("ignore_breakpoint"),        clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_breakpoint"),        clMainFrame::DispatchCommandEvent)
    EVT_MENU(XRCID("quick_debug"),              clMainFrame::OnQuickDebug)
    EVT_MENU(XRCID("debug_core_dump"),          clMainFrame::OnDebugCoreDump)

    EVT_UPDATE_UI(XRCID("start_debugger"),      clMainFrame::OnDebugUI)
    EVT_UPDATE_UI(XRCID("restart_debugger"),    clMainFrame::OnDebugRestartUI)
    EVT_UPDATE_UI(XRCID("pause_debugger"),      clMainFrame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("stop_debugger"),       clMainFrame::OnDebugStopUI)
    EVT_UPDATE_UI(XRCID("dbg_stepin"),          clMainFrame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("dbg_stepout"),         clMainFrame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("dbg_next"),            clMainFrame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("dbg_nexti"),            clMainFrame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("show_cursor"),         clMainFrame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("insert_breakpoint"),   clMainFrame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("disable_all_breakpoints"), clMainFrame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("enable_all_breakpoints"),  clMainFrame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("delete_all_breakpoints"),  clMainFrame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("quick_debug"),         clMainFrame::OnQuickDebugUI)
    EVT_UPDATE_UI(XRCID("debug_core_dump"),     clMainFrame::OnQuickDebugUI)

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

    //-------------------------------------------------------
    // Perspective menu
    //-------------------------------------------------------
    EVT_MENU_RANGE(PERSPECTIVE_FIRST_MENU_ID, PERSPECTIVE_LAST_MENU_ID, clMainFrame::OnChangePerspective)
    EVT_UPDATE_UI_RANGE(PERSPECTIVE_FIRST_MENU_ID, PERSPECTIVE_LAST_MENU_ID, clMainFrame::OnChangePerspectiveUI)
    EVT_MENU(XRCID("manage_perspectives"),      clMainFrame::OnManagePerspectives)
    EVT_MENU(XRCID("save_current_layout"),      clMainFrame::OnSaveLayoutAsPerspective)

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
    EVT_MENU(XRCID("ID_DETACH_EDITOR"),                 clMainFrame::OnDetachEditor)
    EVT_UPDATE_UI(XRCID("copy_file_name"),              clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("copy_file_path"),              clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("open_shell_from_filepath"),    clMainFrame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("ID_DETACH_EDITOR"),            clMainFrame::OnDetachEditorUI)

    //-----------------------------------------------------------------
    // Default editor context menu
    //-----------------------------------------------------------------
    EVT_MENU(wxID_DELETE,                       clMainFrame::DispatchCommandEvent)
    EVT_UPDATE_UI(wxID_DELETE,                  clMainFrame::DispatchUpdateUIEvent)

    //-----------------------------------------------------------------
    // C++ editor context menu
    //-----------------------------------------------------------------
    EVT_MENU(XRCID("open_include_file"),        clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("add_include_file"),         clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("add_forward_decl"),         clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("swap_files"),               clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("find_decl"),                clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("find_impl"),                clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("go_to_function_start"),     clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("go_to_next_function"),      clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("insert_doxy_comment"),      clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("setters_getters"),          clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("move_impl"),                clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("add_impl"),                 clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("sync_signatures"),          clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("add_multi_impl"),           clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("add_virtual_impl"),         clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("add_pure_virtual_impl"),    clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("rename_symbol"),            clMainFrame::OnCppContextMenu)
    EVT_MENU(XRCID("find_references"),          clMainFrame::OnCppContextMenu)
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
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_MESSAGE             , clMainFrame::OnParsingThreadMessage)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE,     clMainFrame::OnClearTagsCache)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_COMPLETED,  clMainFrame::OnRetaggingCompelted)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS,   clMainFrame::OnRetaggingProgress)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_READY,                clMainFrame::OnParserThreadReady)

    EVT_COMMAND(wxID_ANY, wxEVT_ACTIVATE_EDITOR,                   clMainFrame::OnActivateEditor)

    EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_STATUS_BAR,                 clMainFrame::OnSetStatusMessage)
    EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE,                   clMainFrame::OnDatabaseUpgrade )
    EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE_INTER,             clMainFrame::OnDatabaseUpgradeInternally)
    EVT_COMMAND(wxID_ANY, wxEVT_REFRESH_PERSPECTIVE_MENU,          clMainFrame::OnRefreshPerspectiveMenu)
    EVT_MENU   (XRCID("update_num_builders_count"),                clMainFrame::OnUpdateNumberOfBuildProcesses)
    EVT_MENU   (XRCID("goto_codelite_download_url"),               clMainFrame::OnGotoCodeLiteDownloadPage)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, clMainFrame::OnSingleInstanceOpenFiles)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP,  clMainFrame::OnSingleInstanceRaise)

    EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_VERSION_AVAILABLE, clMainFrame::OnNewVersionAvailable)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_UPTODATE,      clMainFrame::OnNewVersionAvailable)

    EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_DOCKPANE,    clMainFrame::OnNewDetachedPane)
    EVT_COMMAND(wxID_ANY, wxEVT_LOAD_PERSPECTIVE,    clMainFrame::OnLoadPerspective)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_DELETE_DOCKPANE, clMainFrame::OnDestroyDetachedPane)
END_EVENT_TABLE()

clMainFrame* clMainFrame::m_theFrame = NULL;

clMainFrame::clMainFrame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(pParent, id, title, pos, size, style)
    , m_buildAndRun             (false)
    , m_cppMenu                 (NULL)
    , m_highlightWord           (false)
    , m_workspaceRetagIsRequired(false)
    , m_bookmarksDropDownMenu   (NULL)
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

    m_statusbarTimer = new StatusbarTimer(this);

    CreateGUIControls();

    ManagerST::Get();          // Dummy call
    RefactoringEngine::Instance(); // Dummy call

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

    EventNotifier::Get()->Connect(wxEVT_LOAD_SESSION, wxCommandEventHandler(clMainFrame::OnLoadSession), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_PROCESS_ENDED, clCommandEventHandler(clMainFrame::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_PROJECT_CHANGED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_PROJ_SETTINGS_SAVED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(clMainFrame::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING, wxCommandEventHandler(clMainFrame::OnRefactoringCacheStatus), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(clMainFrame::OnThemeChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(clMainFrame::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(clMainFrame::OnSettingsChanged), this);
    EventNotifier::Get()->Bind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, wxCommandEventHandler(clMainFrame::OnReloadExternallModifiedNoPrompt), this);
    EventNotifier::Get()->Bind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED, wxCommandEventHandler(clMainFrame::OnReloadExternallModified), this);

    Connect(wxID_UNDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo), NULL, this);
    Connect(wxID_REDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo), NULL, this);

    // Start the code completion manager, we do this by calling it once
    CodeCompletionManager::Get();
}

clMainFrame::~clMainFrame(void)
{
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

    if ( m_splashScreen ) {
        m_splashScreen->Destroy();
        m_splashScreen = NULL;
    }

    wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::DispatchCommandEvent), NULL, this);
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
    wxTheApp->Disconnect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);
    wxTheApp->Disconnect(wxID_CUT,       wxEVT_UPDATE_UI, wxUpdateUIEventHandler( clMainFrame::DispatchUpdateUIEvent ), NULL, this);

    EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_PROCESS_ENDED, clCommandEventHandler(clMainFrame::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_LOAD_SESSION, wxCommandEventHandler(clMainFrame::OnLoadSession), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_PROJECT_CHANGED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PROJ_SETTINGS_SAVED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(clMainFrame::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(clMainFrame::OnUpdateCustomTargetsDropDownMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING, wxCommandEventHandler(clMainFrame::OnRefactoringCacheStatus), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(clMainFrame::OnThemeChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(clMainFrame::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(clMainFrame::OnSettingsChanged), this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, wxCommandEventHandler(clMainFrame::OnReloadExternallModifiedNoPrompt), this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED, wxCommandEventHandler(clMainFrame::OnReloadExternallModified), this);

    Disconnect(wxID_UNDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo), NULL, this);
    Disconnect(wxID_REDO, wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(clMainFrame::OnTBUnRedo), NULL, this);

    wxDELETE(m_timer);
    wxDELETE(m_statusbarTimer);

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
    //set the revision number in the frame title
    wxString title(_("CodeLite "));
    title << clGitRevision;

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

#if defined(NDEBUG) && !defined(__WXMAC__)
    // we show splash only when using Release builds of codelite
    if ( inf.GetFlags() & CL_SHOW_SPLASH ) {
        wxBitmap bitmap;
        wxString splashName(ManagerST::Get()->GetStarupDirectory() + wxT("/images/splashscreen.png"));
        if (bitmap.LoadFile(splashName, wxBITMAP_TYPE_PNG)) {
            wxString mainTitle = CODELITE_VERSION_STR;
            clMainFrame::m_splashScreen = new clSplashScreen(NULL, bitmap);
        }
    }
#endif

    m_theFrame = new clMainFrame( NULL,
                                  wxID_ANY,
                                  title,
                                  inf.GetFramePosition(),
                                  inf.GetFrameSize(),
                                  wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);
    m_theFrame->m_frameGeneralInfo = inf;
    m_theFrame->m_loadLastSession = loadLastSession;
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

    // Save the current layout as the "Default" layout (unless we already got one ...)
    ManagerST::Get()->GetPerspectiveManager().SavePerspectiveIfNotExists(NORMAL_LAYOUT);

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
    //this->Freeze();

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
    m_mgr.SetArtProvider( new clAuiDockArt(PluginManager::Get()) );
    SetAUIManagerFlags();

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 22);

    m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR,       DrawingUtils::GetPanelBgColour());
    m_mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, DrawingUtils::GetPanelBgColour());

    //initialize debugger configuration tool
    DebuggerConfigTool::Get()->Load(wxT("config/debuggers.xml"), wxT("5.4"));
    WorkspaceST::Get()->SetStartupDir(ManagerST::Get()->GetStarupDirectory());

#if wxCHECK_VERSION(2, 9, 5)
    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
#else
    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
#endif

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 6);

    // Load the menubar from XRC and set this frame's menubar to it.
    wxMenuBar *mb = wxXmlResource::Get()->LoadMenuBar(wxT("main_menu"));

    // Under wxGTK < 2.9.4 we need this wrapper class to avoid warnings on ubuntu when codelite exits
    m_myMenuBar = new MyMenuBar();
    m_myMenuBar->Set(mb);
    SetMenuBar(mb);

    // Set up dynamic parts of menu.
    CreateViewAsSubMenu();
    CreateRecentlyOpenedWorkspacesMenu();
    DoUpdatePerspectiveMenu();

    // Connect to Edit menu, so that its labelled-state submenu can be added on the fly when necessary
    wxMenu* editmenu = NULL;
    wxMenuItem* menuitem = GetMenuBar()->FindItem(wxID_UNDO, &editmenu);
    if (menuitem && editmenu) {
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

    // Add the explorer pane
    m_workspacePane = new WorkspacePane(this, wxT("Workspace View"), &m_mgr);
    m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo().MinimizeButton().MaximizeButton().Name(m_workspacePane->GetCaption()).Caption(m_workspacePane->GetCaption()).Left().BestSize(250, 300).Layer(1).Position(0).CloseButton(true));
    RegisterDockWindow(XRCID("workspace_pane"), wxT("Workspace View"));

    //add the debugger locals tree, make it hidden by default
    m_debuggerPane = new DebuggerPane(this, wxT("Debugger"), &m_mgr);
    m_mgr.AddPane(m_debuggerPane,
                  wxAuiPaneInfo().Name(m_debuggerPane->GetCaption()).Caption(m_debuggerPane->GetCaption()).Bottom().Layer(1).Position(1).CloseButton(true).MinimizeButton().Hide().MaximizeButton());
    RegisterDockWindow(XRCID("debugger_pane"), wxT("Debugger"));

    m_mainBook = new MainBook(this);
    m_mgr.AddPane(m_mainBook, wxAuiPaneInfo().Name(wxT("Editor")).CenterPane().PaneBorder(false));
    CreateRecentlyOpenedFilesMenu();

    m_outputPane = new OutputPane(this, wxT("Output View"));
    wxAuiPaneInfo paneInfo;
    m_mgr.AddPane(m_outputPane, paneInfo.Name(wxT("Output View")).Caption(wxT("Output View")).Bottom().Layer(1).Position(0).CaptionVisible(true).MinimizeButton().Show().BestSize(wxSize(400, 200)).MaximizeButton());
    RegisterDockWindow(XRCID("output_pane"), wxT("Output View"));

    long show_nav(1);
    EditorConfigST::Get()->GetLongValue(wxT("ShowNavBar"), show_nav);
    if ( !show_nav ) {
        m_mainBook->ShowNavBar( false );
    }
    
    if ( !BuildSettingsConfigST::Get()->Load(wxT("2.1")) ) {
        CL_ERROR("Could not locate build configuration! CodeLite installation is broken this might cause unwanted behavior!");
    }

    clConfig ccConfig("code-completion.conf");
    ccConfig.ReadItem( &m_tagsOptionsData );

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
    ClangCompilationDbThreadST::Get()->Start();
    
    // Connect this tree to the parse thread
    ParseThreadST::Get()->SetNotifyWindow( this );

    // And finally create a status bar
    wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
    SetStatusBar(statusBar);
    m_status.resize(3);
    GetStatusBar()->SetFieldsCount(m_status.size());
    SetStatusMessage(_("Ready"), 0);
    SetStatusMessage(_("Done"), m_status.size()-1);

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
        Bind(wxEVT_TOOL_DROPDOWN, wxCommandEventHandler(clMainFrame::OnNativeTBUnRedoDropdown), this, wxID_UNDO, wxID_REDO);
    } else {
        CreateToolbars24();
    }

    // Connect the custom build target events range: !USE_AUI_TOOLBAR only
    if ( GetToolBar() ) {
        GetToolBar()->Connect(ID_MENU_CUSTOM_TARGET_FIRST, ID_MENU_CUSTOM_TARGET_MAX, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnBuildCustomTarget), NULL, this);
    }

    Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(clMainFrame::OnChangeActiveBookmarkType), this, XRCID("BookmarkTypes[start]"), XRCID("BookmarkTypes[end]"));

    GetWorkspacePane()->GetNotebook()->SetRightClickMenu( wxXmlResource::Get()->LoadMenu(wxT("workspace_view_rmenu")) );
    GetDebuggerPane()->GetNotebook()->SetRightClickMenu(wxXmlResource::Get()->LoadMenu( wxT("debugger_view_rmenu") ) );

    m_mgr.Update();
    SetAutoLayout (true);

    wxString sessConfFile;
    sessConfFile << clStandardPaths::Get().GetUserDataDir() << wxT("/config/sessions.xml");
    SessionManager::Get().Load(sessConfFile);

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

    wxMenu* menu = NULL;
    wxMenuItem* menuitem = GetMenuBar()->FindItem( XRCID("display_eol"), &menu );
    if (menuitem && menu) {
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
            lexName.MakeLower();
            if(lexName.IsEmpty())
                continue;

            item = new wxMenuItem(submenu, minId, lex->GetName(), wxEmptyString, wxITEM_CHECK);
            m_viewAsMap[minId] = lexName;
            minId++;
            submenu->Append(item);
        }
        menu->Append(viewAsSubMenuID, _("View As"), submenu);
    }
}

void clMainFrame::OnEditMenuOpened(wxMenuEvent& event)
{
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    wxMenuItem* labelCurrentState = event.GetMenu()->FindChildItem(XRCID("label_current_state"));
    if (labelCurrentState) { // Here seems to be the only reliable place to do 'updateui' for this; a real UpdateUI handler is only hit when there's no editor :/
        labelCurrentState->Enable(editor != NULL);
    }

    if (editor) {
        editor->GetCommandsProcessor().PrepareLabelledStatesMenu(event.GetMenu());

    } else {
        // There's no active editor, so remove any stale submenu; otherwise it'll display but the contents won't work
        wxMenuItem* menuitem = event.GetMenu()->FindChildItem(XRCID("goto_labelled_state"));
        if (menuitem) {
            event.GetMenu()->Delete(menuitem);
        }
        event.Skip();
    }
}

void clMainFrame::OnNativeTBUnRedoDropdown(wxCommandEvent& event)
{    
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if (editor && GetToolBar()) {
        bool undoing = event.GetId()==wxID_UNDO;
        wxMenu* menu = new wxMenu;
        editor->GetCommandsProcessor().DoPopulateUnRedoMenu(*menu, undoing);
        if (!menu->GetMenuItemCount()) {
            delete menu;
            return;
        }

        if (undoing) {
            menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem), &editor->GetCommandsProcessor());
        } else {
            menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem), &editor->GetCommandsProcessor());
        }
        GetToolBar()->SetDropdownMenu(event.GetId(), menu);
        
        event.Skip();
    }
    // Don't skip if there's no active editor/toolbar, otherwise a stale menu will show
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
        toolbar_parent = this;
    }
#else
    wxWindow *toolbar_parent (this);
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
    tb->AddTool(XRCID("new_file"),        _("New"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_new")),     _("New File"));
    tb->AddTool(XRCID("open_file"),       _("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_open")),    _("Open File"));
    tb->AddTool(XRCID("refresh_file"),    _("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_reload")),  _("Reload File"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("save_file"),       _("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save")),    _("Save"));
    tb->AddTool(XRCID("save_all"),    _("Save All"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save_all")),     _("Save All"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("close_file"),      _("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_close")),   _("Close File"));
    TB_SEPARATOR();
    tb->AddTool(wxID_CUT,                 _("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/cut")),          _("Cut"));
    tb->AddTool(wxID_COPY,                _("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/copy")),         _("Copy"));
    tb->AddTool(wxID_PASTE,               _("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/paste")),        _("Paste"));
    TB_SEPARATOR();
    tb->AddTool(wxID_UNDO,                _("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/undo")),         _("Undo"));
    tb->SetToolDropDown(wxID_UNDO, true);
    tb->AddTool(wxID_REDO,                _("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/redo")),         _("Redo"));
    tb->SetToolDropDown(wxID_REDO, true);
    tb->AddTool(wxID_BACKWARD,            _("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/24/standard/back")),         _("Backward"));
    tb->AddTool(wxID_FORWARD,             _("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/24/standard/forward")),      _("Forward"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/24/standard/bookmark")),     _("Toggle Bookmark"));
    tb->SetToolDropDown(XRCID("toggle_bookmark"), true);


    if (PluginManager::Get()->AllowToolbar()) {
        tb->Realize();
        m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Standard")).ToolbarPane().Top());
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

    tb->AddTool(wxID_FIND,              _("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/24/search/find")),             _("Find"));
    tb->AddTool(wxID_REPLACE,           _("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_and_replace")), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_in_files")),    _("Find In Files"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/24/search/open_resource")),    _("Find Resource In Workspace"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/24/search/mark_word")),        _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    TB_SEPARATOR();

    if (PluginManager::Get()->AllowToolbar()) {
        tb->Realize();
        m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Search")).ToolbarPane().Top());
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

    tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/build")),        _("Build Active Project"));
    tb->SetToolDropDown(XRCID("build_active_project"), true);

    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/stop")),         _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/clean")),        _("Clean Active Project"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute")),      _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute_stop")), _("Stop Running Program"));

    if (PluginManager::Get()->AllowToolbar()) {
        tb->Realize();
        info = wxAuiPaneInfo();
        m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Build")).ToolbarPane().Top());
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

    tb->AddTool(XRCID("start_debugger"),   _("Start or Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/start")),            _("Start or Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"),    _("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/stop")),              _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"),   _("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/interrupt")),         _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/restart")),           _("Restart debugger"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("show_cursor"),      _("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/show_current_line")), _("Show Current Line"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("dbg_stepin"),       _("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_in")),           _("Step In"));
    tb->AddTool(XRCID("dbg_next"),         _("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/next")),              _("Next"));
    tb->AddTool(XRCID("dbg_nexti"),         _("Next Instruction"),         bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/nexti")),              _("Next Instruction"));
    tb->AddTool(XRCID("dbg_stepout"),      _("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_out")),          _("Step Out"));
    tb->Realize();

    if (PluginManager::Get()->AllowToolbar()) {
        info = wxAuiPaneInfo();
        m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Debug")).ToolbarPane().Top());

        RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
        RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
        RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
        RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
    } else {
#if !USE_AUI_TOOLBAR
        SetToolBar(tb);
#else
        m_mgr.AddPane(tb, info.Name(wxT("Main Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Main Toolbar")).ToolbarPane().Top());
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
    tb->AddTool(XRCID("new_file"),        _("New"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_new")),     _("New File"));
    tb->AddTool(XRCID("open_file"),       _("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_open")),    _("Open File"));
    tb->AddTool(XRCID("refresh_file"),    _("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_reload")),  _("Reload File"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("save_file"),       _("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save")),    _("Save"));
    tb->AddTool(XRCID("save_all"),    _("Save All"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save_all")),     _("Save All"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("close_file"),      _("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_close")),   _("Close File"));
    TB_SEPARATOR();
    tb->AddTool(wxID_CUT,                 _("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/cut")),          _("Cut"));
    tb->AddTool(wxID_COPY,                _("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/copy")),         _("Copy"));
    tb->AddTool(wxID_PASTE,               _("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/paste")),        _("Paste"));
    TB_SEPARATOR();
    tb->AddTool(wxID_UNDO,                _("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/undo")),         _("Undo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_REDO,                _("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/redo")),         _("Redo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_BACKWARD,            _("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/16/standard/back")),         _("Backward"));
    tb->AddTool(wxID_FORWARD,             _("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/16/standard/forward")),      _("Forward"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/16/standard/bookmark")),     _("Toggle Bookmark"), wxITEM_DROPDOWN);
    tb->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));

    //----------------------------------------------
    //create the search toolbar
    //----------------------------------------------
    TB_SEPARATOR();
    tb->AddTool(wxID_FIND,              _("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/16/search/find")),             _("Find"));
    tb->AddTool(wxID_REPLACE,           _("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_and_replace")), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_in_files")),    _("Find In Files"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/16/search/open_resource")),    _("Find Resource In Workspace"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/16/search/mark_word")),        _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    TB_SEPARATOR();


    //----------------------------------------------
    //create the build toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/build")),        _("Build Active Project"), wxITEM_DROPDOWN);

    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/stop")),         _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/clean")),        _("Clean Active Project"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute")),      _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute_stop")), _("Stop Running Program"));
    TB_SEPARATOR();

    //----------------------------------------------
    //create the debugger toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("start_debugger"),   _("Start or Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/start")),            _("Start or Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"),    _("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/stop")),              _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"),   _("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/interrupt")),         _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/restart")),           _("Restart debugger"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("show_cursor"),      _("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/show_current_line")), _("Show Current Line"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("dbg_stepin"),       _("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_in")),           _("Step In"));
    tb->AddTool(XRCID("dbg_next"),         _("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/next")),              _("Next"));
    tb->AddTool(XRCID("dbg_nexti"),         _("Next Instruction"),         bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/nexti")),             _("Next Instruction"));
    tb->AddTool(XRCID("dbg_stepout"),      _("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_out")),          _("Step Out"));

    SetToolBar(tb);
    tb->Realize();
}

void clMainFrame::CreateNativeToolbar24()
{
    //----------------------------------------------
    //create the standard toolbar
    //----------------------------------------------
    wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORIZONTAL);
    BitmapLoader &bmpLoader = *(PluginManager::Get()->GetStdIcons());

    tb->SetToolBitmapSize(wxSize(24, 24));
    tb->AddTool(XRCID("new_file"),        _("New"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_new")),     _("New File"));
    tb->AddTool(XRCID("open_file"),       _("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_open")),    _("Open File"));
    tb->AddTool(XRCID("refresh_file"),    _("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_reload")),  _("Reload File"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("save_file"),       _("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save")),    _("Save"));
    tb->AddTool(XRCID("save_all"),    _("Save All"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_save_all")),     _("Save All"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("close_file"),      _("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/file_close")),   _("Close File"));
    TB_SEPARATOR();
    tb->AddTool(wxID_CUT,                 _("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/24/standard/cut")),          _("Cut"));
    tb->AddTool(wxID_COPY,                _("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/copy")),         _("Copy"));
    tb->AddTool(wxID_PASTE,               _("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/24/standard/paste")),        _("Paste"));
    TB_SEPARATOR();
    tb->AddTool(wxID_UNDO,                _("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/undo")),         _("Undo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_REDO,                _("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/24/standard/redo")),         _("Redo"), wxITEM_DROPDOWN);
    tb->AddTool(wxID_BACKWARD,            _("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/24/standard/back")),         _("Backward"));
    tb->AddTool(wxID_FORWARD,             _("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/24/standard/forward")),      _("Forward"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/24/standard/bookmark")),     _("Toggle Bookmark"), wxITEM_DROPDOWN);
    tb->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));

    //----------------------------------------------
    //create the search toolbar
    //----------------------------------------------
    TB_SEPARATOR();
    tb->AddTool(wxID_FIND,              _("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/24/search/find")),             _("Find"));
    tb->AddTool(wxID_REPLACE,           _("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_and_replace")), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/24/search/find_in_files")),    _("Find In Files"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/24/search/open_resource")),    _("Find Resource In Workspace"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/24/search/mark_word")),        _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    TB_SEPARATOR();

    //----------------------------------------------
    //create the build toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/build")),        _("Build Active Project"), wxITEM_DROPDOWN);
    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/stop")),         _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/clean")),        _("Clean Active Project"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute")),      _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/24/build/execute_stop")), _("Stop Running Program"));
    TB_SEPARATOR();

    //----------------------------------------------
    //create the debugger toolbar
    //----------------------------------------------
    tb->AddTool(XRCID("start_debugger"),   _("Start or Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/start")),            _("Start or Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"),    _("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/stop")),              _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"),   _("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/interrupt")),         _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/restart")),           _("Restart debugger"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("show_cursor"),      _("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/show_current_line")), _("Show Current Line"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("dbg_stepin"),       _("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_in")),           _("Step In"));
    tb->AddTool(XRCID("dbg_next"),         _("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/next")),              _("Next"));
    tb->AddTool(XRCID("dbg_nexti"),         _("Next Instruction"),         bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/nexti")),             _("Next Instruction"));
    tb->AddTool(XRCID("dbg_stepout"),      _("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/24/debugger/step_out")),          _("Step Out"));

    SetToolBar(tb);
    tb->Realize();

}

void clMainFrame::CreateToolbars16()
{
    //----------------------------------------------
    //create the standard toolbar
    //----------------------------------------------
#if !USE_AUI_TOOLBAR
    wxWindow *toolbar_parent (this);
    if (PluginManager::Get()->AllowToolbar()) {
        toolbar_parent = this;
    }
#else
    wxWindow *toolbar_parent (this);
#endif

    clToolBar *tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
    tb->SetArtProvider(new CLMainAuiTBArt());
#endif
    wxAuiPaneInfo info;

    BitmapLoader &bmpLoader = *(PluginManager::Get()->GetStdIcons());

    tb->SetToolBitmapSize(wxSize(16, 16));
    tb->AddTool(XRCID("new_file"),        _("New"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_new")),     _("New File"));
    tb->AddTool(XRCID("open_file"),       _("Open"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_open")),    _("Open File"));
    tb->AddTool(XRCID("refresh_file"),    _("Reload"),          bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_reload")),  _("Reload File"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("save_file"),       _("Save"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save")),    _("Save"));
    tb->AddTool(XRCID("save_all"),    _("Save All"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_save_all")),     _("Save All"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("close_file"),      _("Close"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/file_close")),   _("Close File"));
    TB_SEPARATOR();
    tb->AddTool(wxID_CUT,                 _("Cut"),             bmpLoader.LoadBitmap(wxT("toolbars/16/standard/cut")),          _("Cut"));
    tb->AddTool(wxID_COPY,                _("Copy"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/copy")),         _("Copy"));
    tb->AddTool(wxID_PASTE,               _("Paste"),           bmpLoader.LoadBitmap(wxT("toolbars/16/standard/paste")),        _("Paste"));
    TB_SEPARATOR();
    tb->AddTool(wxID_UNDO,                _("Undo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/undo")),         _("Undo"));
    tb->SetToolDropDown(wxID_UNDO, true);
    tb->AddTool(wxID_REDO,                _("Redo"),            bmpLoader.LoadBitmap(wxT("toolbars/16/standard/redo")),         _("Redo"));
    tb->SetToolDropDown(wxID_REDO, true);
    tb->AddTool(wxID_BACKWARD,            _("Backward"),        bmpLoader.LoadBitmap(wxT("toolbars/16/standard/back")),         _("Backward"));
    tb->AddTool(wxID_FORWARD,             _("Forward"),         bmpLoader.LoadBitmap(wxT("toolbars/16/standard/forward")),      _("Forward"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader.LoadBitmap(wxT("toolbars/16/standard/bookmark")),     _("Toggle Bookmark"));
    tb->SetToolDropDown(XRCID("toggle_bookmark"), true);


    if (PluginManager::Get()->AllowToolbar()) {
        tb->Realize();
        m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Standard")).ToolbarPane().Top());
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

    tb->AddTool(wxID_FIND,              _("Find"),                       bmpLoader.LoadBitmap(wxT("toolbars/16/search/find")),             _("Find"));
    tb->AddTool(wxID_REPLACE,           _("Replace"),                    bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_and_replace")), _("Replace"));
    tb->AddTool(XRCID("find_in_files"), _("Find In Files"),              bmpLoader.LoadBitmap(wxT("toolbars/16/search/find_in_files")),    _("Find In Files"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("find_resource"), _("Find Resource In Workspace"), bmpLoader.LoadBitmap(wxT("toolbars/16/search/open_resource")),    _("Find Resource In Workspace"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("highlight_word"), _("Highlight Word"),            bmpLoader.LoadBitmap(wxT("toolbars/16/search/mark_word")),        _("Highlight Matching Words"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("highlight_word"), m_highlightWord);
    TB_SEPARATOR();


    if (PluginManager::Get()->AllowToolbar()) {
        tb->Realize();
        m_mgr.AddPane(tb, info.Name(wxT("Search Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Search")).ToolbarPane().Top());

        //----------------------------------------------
        //create the build toolbar
        //----------------------------------------------
        tb = new clToolBar(toolbar_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
#if USE_AUI_TOOLBAR
        tb->SetArtProvider(new CLMainAuiTBArt());
#endif
        tb->SetToolBitmapSize(wxSize(16, 16));
    }

    tb->AddTool(XRCID("build_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/build")), _("Build Active Project"));
    tb->SetToolDropDown(XRCID("build_active_project"), true);
    tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/stop")),  _("Stop Current Build"));
    tb->AddTool(XRCID("clean_active_project"),      wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/clean")), _("Clean Active Project"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("execute_no_debug"),          wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute")),      _("Run Active Project"));
    tb->AddTool(XRCID("stop_executed_program"),     wxEmptyString, bmpLoader.LoadBitmap(wxT("toolbars/16/build/execute_stop")), _("Stop Running Program"));

    if (PluginManager::Get()->AllowToolbar()) {
        tb->Realize();
        info = wxAuiPaneInfo();
        m_mgr.AddPane(tb, info.Name(wxT("Build Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Build")).ToolbarPane().Top());
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

    tb->AddTool(XRCID("start_debugger"),   _("Start / Continue debugger"), bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/start")),             _("Start / Continue debugger"));
    tb->AddTool(XRCID("stop_debugger"),    _("Stop debugger"),             bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/stop")),              _("Stop debugger"));
    tb->AddTool(XRCID("pause_debugger"),   _("Pause debugger"),            bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/interrupt")),         _("Pause debugger"));
    tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"),          bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/restart")),           _("Restart debugger"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("show_cursor"),      _("Show Current Line"),         bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/show_current_line")), _("Show Current Line"));
    TB_SEPARATOR();
    tb->AddTool(XRCID("dbg_stepin"),       _("Step Into"),                 bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_in")),           _("Step In"));
    tb->AddTool(XRCID("dbg_next"),         _("Next"),                      bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/next")),              _("Next"));
    tb->AddTool(XRCID("dbg_nexti"),         _("Next Instruction"),         bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/nexti")),              _("Next Instruction"));
    tb->AddTool(XRCID("dbg_stepout"),      _("Step Out"),                  bmpLoader.LoadBitmap(wxT("toolbars/16/debugger/step_out")),          _("Step Out"));

    tb->Realize();
    if (PluginManager::Get()->AllowToolbar()) {
        info = wxAuiPaneInfo();
        m_mgr.AddPane(tb, info.Name(wxT("Debugger Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Debug")).ToolbarPane().Top());
        RegisterToolbar(XRCID("show_std_toolbar"), wxT("Standard Toolbar"));
        RegisterToolbar(XRCID("show_search_toolbar"), wxT("Search Toolbar"));
        RegisterToolbar(XRCID("show_build_toolbar"), wxT("Build Toolbar"));
        RegisterToolbar(XRCID("show_debug_toolbar"), wxT("Debugger Toolbar"));
    } else {
#if !USE_AUI_TOOLBAR
        SetToolBar(tb);
#else
        m_mgr.AddPane(tb, info.Name(wxT("Main Toolbar")).LeftDockable(true).RightDockable(true).Caption(_("Main Toolbar")).ToolbarPane().Top());
#endif
    }
}

void clMainFrame::LocateCompilersIfNeeded()
{
    bool bAutoDetectCompilers = clConfig::Get().Read("AutoDetectCompilerOnStartup", true);
    if ( bAutoDetectCompilers ) {
        
        // Unset the flag so next time we won't get this
        clConfig::Get().Write("AutoDetectCompilerOnStartup", false);
        
        // First time, trigger the auto-compiler detection code
        CompilersDetectorManager detector;
        if ( detector.Locate() ) {
            const ICompilerLocator::CompilerVec_t& compilersFound = detector.GetCompilersFound();
            CompilersFoundDlg dlg(this, compilersFound);
            if ( dlg.ShowModal() == wxID_OK ) {
                // Replace the current compilers with a new one
                BuildSettingsConfigST::Get()->SetCompilers( compilersFound );
            }
        }
    }
}

void clMainFrame::UpdateBuildTools()
{
}

void clMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void clMainFrame::OnTBUnRedo(wxAuiToolBarEvent& event)
{
    if (event.IsDropDownClicked()) {
        LEditor* editor = GetMainBook()->GetActiveEditor(true);
        if (editor) {
            editor->GetCommandsProcessor().ProcessEvent(event);

        } else if (GetMainBook()->GetCurrentPage()) {
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

bool clMainFrame::IsEditorEvent(wxEvent &event)
{
#ifdef __WXGTK__
    MainBook *mainBook = GetMainBook();
    if(!mainBook || !mainBook->GetActiveEditor(true)) {
        if ( event.GetId() == wxID_FIND )
            return true;
        else
            return false;
    }

    switch (event.GetId()) {
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
    wxWindow *focusWin = wxWindow::FindFocus();
    if ( focusWin ) {
        switch (event.GetId()) {
        case wxID_CUT:
        case wxID_SELECTALL:
        case wxID_COPY:
        case wxID_PASTE:
        case wxID_UNDO:
        case wxID_REDO: {
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
    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    if ( !editor && event.GetId() != wxID_FIND ) {
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

    LEditor* editor = GetMainBook()->GetActiveEditor(true);
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

    LEditor* editor = GetMainBook()->GetActiveEditor(true);
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
    wxString msg;
    bool boDoNotClose = false;

    if (!boDoNotClose) {
        ManagerST::Get()->SetShutdownInProgress(true);

        // Stop the retag thread
        ParseThreadST::Get()->Stop();

        // Stop the search thread
        ManagerST::Get()->KillProgram();
        SearchThreadST::Get()->StopSearch();

        // Stop any debugging session if any
        IDebugger *debugger = DebuggerMgr::Get().GetActiveDebugger();
        if(debugger && debugger->IsRunning())
            ManagerST::Get()->DbgStop();

        SaveLayoutAndSession();

        // In case we got some data in the clipboard, flush it so it will be available
        // after our process exits
        wxTheClipboard->Flush();

        event.Skip();
    }
}

void clMainFrame::LoadSession(const wxString &sessionName)
{
    SessionEntry session;

    if (SessionManager::Get().GetSession(sessionName, session)) {
        wxString wspFile = session.GetWorkspaceName();
        if (wspFile.IsEmpty() == false && wspFile != wxT("Default")) {
            
            // Load the workspace only if it exists
            wxFileName fnWorkspace( wspFile );
            if ( fnWorkspace.Exists() ) {
                ManagerST::Get()->OpenWorkspace(wspFile);
            }
            
        } else {
            // no workspace to open, so just restore any previously open editors
            GetMainBook()->RestoreSession(session);
        }
    }
}

void clMainFrame::OnSave(wxCommandEvent& event)
{
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
    if (editor) {
        editor->SaveFile();

    } else {

        // delegate it to the plugins
        wxCommandEvent saveEvent(XRCID("save_file"));
        EventNotifier::Get()->ProcessEvent(saveEvent);

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
    GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_Default);
    dlg.EnableReplaceCheck(editors.size());

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString filepath = dlg.GetListBox()->GetStringSelection();
    wxString sessionFilepath = filepath.BeforeLast(wxT('.'));

    clWindowUpdateLocker locker(this);
    TabGroupEntry session;
    if (SessionManager::Get().GetSession(sessionFilepath, session, wxString(wxT(".tabgroup")), tabgroupTag) ) {
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
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
    if (editor) {
        if ( editor->GetModify() ) {
            // Ask user if he really wants to lose all changes
            wxString msg;
            msg << _("The file") << wxT(" '") << editor->GetFileName().GetFullName() << wxT(" '") << _("has been altered.") << wxT("\n");
            msg << _("Are you sure you want to lose all changes?");
            if ( wxMessageBox(msg, _("Confirm"), wxYES_NO, ::wxGetTopLevelParent(editor) ) != wxYES ) {
                return;
            }
        }
        editor->ReloadFile();
    }
}

void clMainFrame::OnCloseWorkspace(wxCommandEvent &event)
{
    wxUnusedVar(event);

    // let the plugins close any custom workspace
    clCommandEvent e(wxEVT_CMD_CLOSE_WORKSPACE, GetId());
    e.SetEventObject(this);
    EventNotifier::Get()->ProcessEvent(e);

    // In any case, make sure that we dont have a workspace opened
    if (ManagerST::Get()->IsWorkspaceOpen()) {
        ManagerST::Get()->CloseWorkspace();
    }
    ShowWelcomePage();
}

void clMainFrame::OnSwitchWorkspace(wxCommandEvent &event)
{
    wxUnusedVar(event);

    bool promptUser = true;
    wxString wspFile;
    const wxString WSP_EXT = "workspace";

    // Is it an internal command? (usually the workspace file name to load is set in the
    // event.SetString() )
    if ( event.GetString().IsEmpty() || wxFileName(event.GetString()).GetExt() != WSP_EXT) {

        clCommandEvent e(wxEVT_CMD_OPEN_WORKSPACE, GetId());
        e.SetEventObject(this);
        e.SetFileName( event.GetString() );
        if(EventNotifier::Get()->ProcessEvent(e)) {
            // the plugin processed it by itself
            return;
        }

        if( e.GetFileName().IsEmpty() == false) {
            // the plugin processed it by itself and the user already selected a file
            // don't bother to prompt the user again just use what he already selected
            promptUser = false;
            wspFile = e.GetFileName();
        }

    } else if ( !event.GetString().IsEmpty() ) {
        promptUser = false;
        wspFile = event.GetString();
    }

    if(promptUser) {
        // now it is time to prompt user for new workspace to open
        const wxString ALL(wxT("CodeLite Workspace files (*.workspace)|*.workspace|")
                           wxT("All Files (*)|*"));
        wxFileDialog dlg(this, _("Open Workspace"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE , wxDefaultPosition);
        if (dlg.ShowModal() == wxID_OK) {
            wspFile = dlg.GetPath();
        }
    }

    if(!wspFile.IsEmpty()) {

        // Make sure that the 'Workspace' tab is visible
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_workspace_tab"));
        event.SetEventObject(this);
        event.SetInt(1);
        GetEventHandler()->ProcessEvent(event);

        // Open the workspace
        ManagerST::Get()->OpenWorkspace(wspFile);
    }
}

void clMainFrame::OnCompleteWordRefreshList(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
    if (editor) {
        editor->CompleteWord(true);
    }
}

void clMainFrame::OnCompleteWord(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
    if (editor) {
        editor->CompleteWord();
    }
}

void clMainFrame::OnFunctionCalltip(wxCommandEvent& event)
{
    wxUnusedVar(event);
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
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
    const wxString ALL( wxT("All Files (*)|*") );

    // Open a file using the current editor's path
    LEditor * editor = GetMainBook()->GetActiveEditor();
    wxString open_path;

    if ( editor ) {
        open_path = editor->GetFileName().GetPath();
    } else {
        // Could not locate the active editor, use the project
        ProjectPtr project = ManagerST::Get()->GetProject( ManagerST::Get()->GetActiveProjectName() );
        if ( project ) {
            open_path = project->GetFileName().GetPath();
        }
    }

    wxFileDialog *dlg = new wxFileDialog(this, _("Open File"), open_path, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
    if (dlg->ShowModal() == wxID_OK) {
        wxArrayString paths;
        dlg->GetPaths(paths);
        for (size_t i=0; i<paths.GetCount(); i++) {
            GetMainBook()->OpenFile(paths.Item(i));
        }
    }
    dlg->Destroy();
}

void clMainFrame::OnFileClose(wxCommandEvent &event)
{
    wxUnusedVar( event );
    if ( GetMainBook()->GetCurrentPage() ) {
        wxWindow *winToClose = GetMainBook()->GetCurrentPage();
        GetMainBook()->CallAfter( &MainBook::ClosePageVoid, winToClose);
    }
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
    GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_RetainOrder|MainBook::kGetAll_IncludeDetached); // We'll want the order of intArr to match the order in MainBook::SaveSession
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
            if ( wxMessageBox(_("Please enter a name for the tab group"), _("CodeLite"), wxICON_ERROR|wxOK|wxCANCEL, this) != wxOK ) {
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
            SessionManager::Get().Save(session.GetTabgroupName(), session, "tabgroup", tabgroupTag);
            // Add the new tabgroup to the tabgroup manager and pane
            GetWorkspacePane()->GetTabgroupsTab()->AddNewTabgroupToTree(filepath);

            // Remove any previous instance of this group from the history, then prepend it and save
            int index = previousgroups.Index(filepath);
            if (index != wxNOT_FOUND) {
                previousgroups.RemoveAt(index);
            }
            previousgroups.Insert(filepath, 0);
            EditorConfigST::Get()->SetRecentItems( previousgroups, wxT("RecentTabgroups") );

            SetStatusMessage(_("Tab group saved"), 0);
        }

        return;
    }
}

void clMainFrame::OnCompleteWordUpdateUI(wxUpdateUIEvent &event)
{
    CHECK_SHUTDOWN();

    LEditor* editor = GetMainBook()->GetActiveEditor(true);
    // This menu item is enabled only if the current editor belongs to a project
    event.Enable( editor );
}

void clMainFrame::OnWorkspaceOpen(wxUpdateUIEvent &event)
{
    CHECK_SHUTDOWN();
    clCommandEvent e(wxEVT_CMD_IS_WORKSPACE_OPEN, GetId());
    e.SetEventObject(this);
    e.SetAnswer(false);
    EventNotifier::Get()->ProcessEvent(e);

    event.Enable(ManagerST::Get()->IsWorkspaceOpen() || e.IsAnswer());
}

// Project->New Workspace
void clMainFrame::OnProjectNewWorkspace(wxCommandEvent &event)
{
    // let the plugins handle this event first
    wxCommandEvent e(wxEVT_CMD_CREATE_NEW_WORKSPACE, GetId());
    e.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(e))
        return;

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
    // Let the plugin process this request first
    clNewProjectEvent newProjectEvent(wxEVT_NEW_PROJECT_WIZARD_SHOWING);
    newProjectEvent.SetEventObject(this);
    if( EventNotifier::Get()->ProcessEvent( newProjectEvent ) ) {
        return;
    }

    
    wxUnusedVar(event);
    NewProjectWizard wiz(this, newProjectEvent.GetTemplates());
    if ( wiz.RunWizard( wiz.GetFirstPage() ) ) {
        
        ProjectData data = wiz.GetProjectData();
        
        // Try the plugins first
        clNewProjectEvent finishEvent(wxEVT_NEW_PROJECT_WIZARD_FINISHED);
        finishEvent.SetEventObject(this);
        finishEvent.SetToolchain( data.m_cmpType );
        finishEvent.SetDebugger( data.m_debuggerType );
        finishEvent.SetProjectName( data.m_name );
        finishEvent.SetProjectFolder( data.m_path );
        finishEvent.SetTemplateName( data.m_sourceTemplate );
        if( EventNotifier::Get()->ProcessEvent( finishEvent ) ) {
            return;
        }
        
        // Carry on with the default behaviour
        ManagerST::Get()->CreateProject(data);
    }
}

void clMainFrame::OnProjectAddProject(wxCommandEvent &event)
{
    wxUnusedVar(event);

    // Prompt user for project path
    const wxString ALL( wxT("CodeLite Projects (*.project)|*.project|")
                        wxT("All Files (*)|*"));
    wxFileDialog *dlg = new wxFileDialog(this, _("Open Project"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST , wxDefaultPosition);
    if (dlg->ShowModal() == wxID_OK) {
        // Open it
        ManagerST::Get()->AddProject(dlg->GetPath());
    }
    dlg->Destroy();
}

void clMainFrame::OnReconcileProject(wxCommandEvent &event)
{
    wxUnusedVar(event);
    ManagerST::Get()->ReconcileProject();
}

void clMainFrame::OnCtagsOptions(wxCommandEvent &event)
{
    wxUnusedVar(event);

    bool colVars(false);
    bool colTags(false);
    bool newColVars(false);
    bool newColTags(false);
    bool caseSensitive(false);

    size_t colourTypes(0);

    colVars     = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
    colTags     = (m_tagsOptionsData.GetFlags() & CC_COLOUR_WORKSPACE_TAGS ? true : false);
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
            ITagsStoragePtr db = TagsManagerST::Get()->GetDatabase();
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
        caseSensitive      = (m_tagsOptionsData.GetFlags() & CC_IS_CASE_SENSITIVE);

        TagsManagerST::Get()->SetCtagsOptions( m_tagsOptionsData );
        TagsManagerST::Get()->GetDatabase()->SetEnableCaseInsensitive(!caseSensitive);

        clConfig ccConfig("code-completion.conf");
        ccConfig.WriteItem( &m_tagsOptionsData );

        // We use this method 'UpdateParserPaths' since it will also update the parser
        // thread with any workspace search/exclude paths related
        ManagerST::Get()->UpdateParserPaths();

        TagsManagerST::Get()->GetDatabase()->SetMaxWorkspaceTagToColour( m_tagsOptionsData.GetMaxItemToColour() );

        //do we need to colourise?
        if (newColTags != colTags || newColVars != colVars || colourTypes != m_tagsOptionsData.GetCcColourFlags()) {
            GetMainBook()->UpdateColours();
        }

        if (pathsAfter.IsEmpty() == false) {
            // a retagg is needed
            wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
            AddPendingEvent(e);
        }

#if HAS_LIBCLANG
        // Clear clang's cache
        ClangCodeCompletion::Instance()->ClearCache();
#endif
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
        ViewPane(iter->second, event.IsChecked());
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

void clMainFrame::OnToggleMainTBars(wxCommandEvent &event)
{
    wxUnusedVar(event);
    ToggleToolBars(true);
}

void clMainFrame::OnTogglePluginTBars(wxCommandEvent &event)
{
    wxUnusedVar(event);
    ToggleToolBars(false);
}

void clMainFrame::ToggleToolBars(bool std)
{
    wxMenu *menu;
    wxMenuItem* item = GetMenuBar()->FindItem(XRCID("show_std_toolbar"), &menu);
    if (!item || !item->IsCheckable()) {
        CL_DEBUG1(wxT("In clMainFrame::ToggleToolBars: menuitem not found"));
        return;
    }

    if (std) {
        // The standard items are the first 4
        // Use the first menuitem's state to decide whether to show or hide them all
        bool checked = item->IsChecked();

        if (!menu || (menu->GetMenuItemCount() < 4) ) {
            CL_DEBUG1(wxT("In clMainFrame::ToggleToolBars: menu not found, or has too few items"));
            return;
        }

        for (size_t n=0; n < 4; ++n) {
            wxMenuItem* item = menu->FindItemByPosition(n);
            if (!item || !item->IsCheckable()) {
                CL_DEBUG1(wxT("In clMainFrame::ToggleToolBars: standard menuitem not found, or is no longer checkable :/"));
                continue;
            }

            std::map<int, wxString>::iterator iter = m_toolbars.find(item->GetId());
            if (iter != m_toolbars.end()) {
                ViewPane(iter->second, !checked);
            }
        }
        return;
    }

    // We don't know in advance the number of plugin toolbars, but we do know that they are at the end, following a separator
    // So show/hide them backwards until we hit something uncheckable
    size_t count = menu->GetMenuItemCount();
    if (count < 8) {
        CL_DEBUG1(wxT("In clMainFrame::ToggleToolBars: An implausibly small number of non-plugin menuitem found"));
        return;
    }

    bool checked = true;
    for (size_t n=count; n; --n) {
        wxMenuItem* item = menu->FindItemByPosition(n-1);
        if (!item || !item->IsCheckable()) {
            return;
        }
        // Arbitrarily use the last menuitem's state to decide whether to show or hide them all
        if (n == count) {
            checked = item->IsChecked();
        }
        std::map<int, wxString>::iterator iter = m_toolbars.find(item->GetId());
        if (iter != m_toolbars.end()) {
            ViewPane(iter->second, !checked);
        }
    }
}

void clMainFrame::OnViewPane(wxCommandEvent &event)
{
    std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
    if (iter != m_panes.end()) {
        // In >wxGTK-2.9.4 event.GetChecked() is invalid when coming from an accelerator; instead examine the actual state
        wxAuiPaneInfo &info = m_mgr.GetPane(iter->second);
        if (info.IsOk()) {
            ViewPane(iter->second, !info.IsShown());
        }
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

void clMainFrame::ViewPane(const wxString &paneName, bool checked)
{
    wxAuiPaneInfo &info = m_mgr.GetPane(paneName);
    if (info.IsOk()) {
        if ( checked ) {
            DockablePaneMenuManager::HackShowPane(info,&m_mgr);
        } else {
            DockablePaneMenuManager::HackHidePane(true,info,&m_mgr);
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

void clMainFrame::OnBuildEnded(clCommandEvent& event)
{
    event.Skip();
    
    if (m_buildAndRun) {
        //If the build process was part of a 'Build and Run' command, check whether an erros
        //occured during build process, if non, launch the output
        m_buildAndRun = false;
        if ( ManagerST::Get()->IsBuildEndedSuccessfully() ||
             wxMessageBox(_("Build ended with errors. Continue?"), _("Confirm"), wxYES_NO| wxICON_QUESTION, this) == wxYES) {
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

        // Make sure that the working folder is set to the correct path
        wxString workspacePath = WorkspaceST::Get()->GetWorkspaceFileName().GetPath();
        ::wxSetWorkingDirectory( workspacePath );
        wxLogMessage("Setting working directory to: %s", workspacePath);
        SetStatusMessage(_("Build starting..."), 0);

        wxString conf, projectName;
        projectName = ManagerST::Get()->GetActiveProjectName();

        // get the selected configuration to be built
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if (bldConf) {
            conf = bldConf->GetName();
        }

        QueueCommand info(projectName, conf, false, QueueCommand::kBuild);
        if (bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Build"));
        }
        ManagerST::Get()->PushQueueCommand( info );
        ManagerST::Get()->ProcessCommandQueue();

        SetStatusMessage(_("Done"), 0);
    }
}

void clMainFrame::OnBuildCustomTarget(wxCommandEvent& event)
{
    bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
    if (enable) {

        // get the selected configuration to be built
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(CustomTargetsMgr::Get().GetProjectName(), wxEmptyString);
        if ( bldConf ) {
            CustomTargetsMgr::Pair_t target = CustomTargetsMgr::Get().GetTarget( event.GetId() );
            if ( target.second.IsEmpty() ) {
                wxLogMessage(wxString::Format(wxT("%s=%d"), _("Failed to find Custom Build Target for event ID"), event.GetId()));
                return;
            }

            QueueCommand info(CustomTargetsMgr::Get().GetProjectName(), bldConf->GetName(), false, QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(target.first);

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

        QueueCommand info(projectName, conf, false, QueueCommand::kBuild);

        if (bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
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
    wxCommandEvent e(wxEVT_CMD_IS_PROGRAM_RUNNING, GetId());
    e.SetEventObject(this);
    e.SetInt(0);
    EventNotifier::Get()->ProcessEvent(e);

    Manager *mgr = ManagerST::Get();
    bool enable = mgr->IsProgramRunning();
    event.Enable(enable || e.GetInt());
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
    wxCommandEvent evtExecute(wxEVT_CMD_STOP_EXECUTED_PROGRAM);
    if ( EventNotifier::Get()->ProcessEvent( evtExecute ) )
        return;

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

    QueueCommand buildInfo(QueueCommand::kClean);
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
    // Test to see if any plugin wants to execute it
    wxCommandEvent evtExecute(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT);
    if ( EventNotifier::Get()->ProcessEvent( evtExecute ) )
        return;
    
    if ( !WorkspaceST::Get()->IsOpen() ) {
        return;
    }
    
    // Prepare the commands to execute
    QueueCommand commandExecute(QueueCommand::kExecuteNoDebug);
    wxStandardID res = ::PromptForYesNoDialogWithCheckbox( _("Would you like to build the active project\nbefore executing it?"), 
                                                           "PromptForBuildBeforeExecute", 
                                                           _("Build and Execute"), 
                                                           _("Execute"));
    // Don't do anything if "X" is pressed
    if ( res != wxID_CANCEL ) {
        if ( res == wxID_YES ) {
            QueueCommand buildCommand( QueueCommand::kBuild );
            ManagerST::Get()->PushQueueCommand( buildCommand );
            commandExecute.SetCheckBuildSuccess( true ); // execute only if build was successfull
        }

        ManagerST::Get()->PushQueueCommand( commandExecute );
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void clMainFrame::OnExecuteNoDebugUI(wxUpdateUIEvent &event)
{
    CHECK_SHUTDOWN();
    wxCommandEvent e(wxEVT_CMD_IS_PROGRAM_RUNNING, GetId());
    e.SetEventObject(this);
    e.SetInt(0);
    EventNotifier::Get()->ProcessEvent(e);

    bool normalCondition = ManagerST::Get()->GetActiveProjectName().IsEmpty() == false
                 &&
                 !ManagerST::Get()->IsBuildInProgress()
                 &&
                 !ManagerST::Get()->IsProgramRunning();
    event.Enable(normalCondition || !e.GetInt());
}

void clMainFrame::OnTimer(wxTimerEvent &event)
{
    // since there is a bug in wxURL, which it can not be used while constucting a wxFrame,
    // it must be called *after* the frame constuction
    // add new version notification updater
    long updatePaths(1);

    EditorConfigST::Get()->GetLongValue(wxT("UpdateParserPaths"), updatePaths);
    if ( clConfig::Get().Read("CheckForNewVersion", true) ) {
        JobQueueSingleton::Instance()->PushJob(new WebUpdateJob(this, false));
    }

    // enable/disable plugins toolbar functionality
    PluginManager::Get()->EnableToolbars();

    // Check that the user has some paths set in the parser
    clConfig ccConfig("code-completion.conf");
    ccConfig.ReadItem( &m_tagsOptionsData );


    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    bool isUpdateRequired = (m_tagsOptionsData.GetVersion() != TagsOptionsData::CURRENT_VERSION);
    if ( isUpdateRequired ) {

        // Since the version numbers aren't the same
        // we should merge the new settings with the old ones
        TagsOptionsData tmp;
        m_tagsOptionsData.Merge( tmp );

        // Try to locate the paths automatically
        wxArrayString paths;
        wxArrayString excludePaths;
        IncludePathLocator locator(PluginManager::Get());
        locator.Locate( paths, excludePaths );

        wxArrayString curExcludePaths = m_tagsOptionsData.GetParserExcludePaths();
        wxArrayString curIncluePaths  = m_tagsOptionsData.GetParserSearchPaths();

        excludePaths = ccConfig.MergeArrays(curExcludePaths, excludePaths);
        paths = ccConfig.MergeArrays(curIncluePaths, paths);
        m_tagsOptionsData.SetParserExcludePaths( excludePaths );
        m_tagsOptionsData.SetParserSearchPaths( paths );
        m_tagsOptionsData.SetVersion( TagsOptionsData::CURRENT_VERSION );

        //-----------------------
        // clang
        //-----------------------

        wxArrayString clangSearchPaths = m_tagsOptionsData.GetClangSearchPathsArray();
        IncludePathLocator clangLocator(PluginManager::Get());

        wxArrayString clang_paths, clang_excludes;
        clangLocator.Locate(clang_paths, clang_excludes, false);

        clang_paths = ccConfig.MergeArrays(clang_paths, clangSearchPaths);
        m_tagsOptionsData.SetClangSearchPathsArray( clang_paths );

        ccConfig.WriteItem( &m_tagsOptionsData );



    }

    //clear navigation queue
    if (GetMainBook()->GetCurrentPage() == 0) {
        NavMgr::Get()->Clear();
    }

    // Load last session?
    if ( clConfig::Get().Read("RestoreLastSession", true) && m_loadLastSession) {
        wxCommandEvent loadSessionEvent(wxEVT_LOAD_SESSION);
        EventNotifier::Get()->AddPendingEvent(loadSessionEvent);
    }

    // ReTag workspace database if needed (this can happen due to schema version changes)
    // It is important to place the retag code here since the retag workspace should take place after
    // the parser search patha have been updated (if needed)
    if( m_workspaceRetagIsRequired ) {
        m_workspaceRetagIsRequired = false;
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
        this->AddPendingEvent(evt);
        GetMainBook()->ShowMessage(_("Your workspace symbols file does not match the current version of CodeLite. CodeLite will perform a full retag of the workspace"));
    }

    // For some reason, under Linux we need to force the menu accelerator again
    // otherwise some shortcuts are getting lose (e.g. Ctrl-/ to comment line)
    ManagerST::Get()->UpdateMenuAccelerators();

    wxModule::InitializeModules();

    // Send initialization end event
    EventNotifier::Get()->PostCommandEvent(wxEVT_INIT_DONE, NULL);
    CallAfter( &clMainFrame::LocateCompilersIfNeeded );
    
    event.Skip();
}

void clMainFrame::OnFileCloseAll(wxCommandEvent &event)
{
    wxUnusedVar(event);
    GetMainBook()->CallAfter( &MainBook::CloseAllVoid, true );
}

void clMainFrame::OnQuickOutline(wxCommandEvent &event)
{
    // Sanity
    LEditor *activeEditor = GetMainBook()->GetActiveEditor(true);
    CHECK_PTR_RET(activeEditor);
    
    // let the plugins process this first
    clCodeCompletionEvent evt(wxEVT_CC_SHOW_QUICK_OUTLINE, GetId());
    evt.SetEventObject(this);
    evt.SetEditor( activeEditor );

    if(EventNotifier::Get()->ProcessEvent(evt))
        return;

    wxUnusedVar(event);
    if (ManagerST::Get()->IsWorkspaceOpen() == false)
        return;

    if (activeEditor->GetProject().IsEmpty())
        return;

    QuickOutlineDlg dlg(::wxGetTopLevelParent(activeEditor),
                        activeEditor->GetFileName().GetFullPath(),
                        wxID_ANY,
                        wxT(""),
                        wxDefaultPosition,
                        wxSize(400, 400),
                        wxDEFAULT_DIALOG_STYLE
                       );

    dlg.ShowModal();
    activeEditor->SetActive();
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
        html << _("No workspaces found.") << wxT("</font></td></tr>");
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
        html << _("No files found.") << wxT("</font></td></tr>");
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

    wxMenu* menu = NULL;
    wxMenuItem* item = GetMenuBar()->FindItem( XRCID("recent_files"), &menu );
    if (item && menu) {
        wxMenu *submenu = item->GetSubMenu();
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

    wxMenu* menu = NULL;
    wxMenuItem* item = GetMenuBar()->FindItem( XRCID("recent_workspaces"), &menu );
    if (item && menu) {
        wxMenu *submenu = item->GetSubMenu();
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

        wxCommandEvent open_workspace_event(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
        open_workspace_event.SetEventObject(this);
        open_workspace_event.SetString( file_name );
        GetEventHandler()->AddPendingEvent( open_workspace_event );
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
    /*
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

    content.Replace(wxT("$(QuickLinks)"), _("Quick Links:"));
    content.Replace(wxT("$(CodeLiteWiki)"), _("CodeLite Wiki"));
    content.Replace(wxT("$(CodeLiteForums)"), _("CodeLite Forums"));
    content.Replace(wxT("$(CreateNewWorkspace)"), _("Create a New Workspace"));
    content.Replace(wxT("$(OpenWorkspace)"), _("Open a Workspace"));
    content.Replace(wxT("$(RecentWorkspaces)"), _("Recently opened workspaces:"));
    content.Replace(wxT("$(RecentFiles)"), _("Recently opened files:"));

    welcomePage->SetPage(content);*/
    WelcomePage* welcomePage = new WelcomePage(GetMainBook());
    GetMainBook()->AddPage(welcomePage, _("Welcome!"), wxNullBitmap, true);

    GetMainBook()->Layout();
    // This is needed in >=wxGTK-2.9, otherwise the auinotebook doesn't fully expand at first
    SendSizeEvent(wxSEND_EVENT_POST);
    // Ditto the workspace pane auinotebook
    GetWorkspacePane()->SendSizeEvent(wxSEND_EVENT_POST);
}

void clMainFrame::OnImportMSVS(wxCommandEvent &e)
{
    wxUnusedVar(e);
    const wxString ALL(wxT("MS Visual Studio Solution File (*.sln)|*.sln|")
                       wxT("All Files (*)|*"));
    wxFileDialog dlg(this, _("Open MS Solution File"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
    if (dlg.ShowModal() == wxID_OK) {

        wxArrayString cmps;
        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
        while (cmp) {
            cmps.Add(cmp->GetName());
            cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
        }

        // Get the prefered compiler type
        wxString compilerName = wxGetSingleChoice(_("Select the compiler to use:"), _("Choose compiler"), cmps);
        ManagerST::Get()->ImportMSVSSolution(dlg.GetPath(), compilerName);
    }
}

void clMainFrame::OnDebug(wxCommandEvent &e)
{
    wxUnusedVar(e);
    
    // Let the plugin know that we are about to start debugging
    clDebugEvent dbgEvent(wxEVT_DBG_UI_START_OR_CONT);
    if ( WorkspaceST::Get()->IsOpen() ) {
        // Pass as much as info as we can (i.e. project name and the selected debugger)
        dbgEvent.SetProjectName( WorkspaceST::Get()->GetActiveProjectName() );
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf( dbgEvent.GetProjectName() , "" );
        if ( bldConf ) {
            dbgEvent.SetDebuggerName( bldConf->GetDebuggerType() );
        }
    }
    
    // If a built-in debugger is already running, don't fire an event to the 
    // plugins
    // We know that it is a built-in debugger if 'dbgr' is not NULL
    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if ( !(dbgr && dbgr->IsRunning()) ) {
        if ( EventNotifier::Get()->ProcessEvent(dbgEvent) ) {
            return;
        }
    }
    
    Manager *mgr = ManagerST::Get();
    
    if (dbgr && dbgr->IsRunning()) {
        
        // Debugger is already running -> probably a continue command
        mgr->DbgContinue();
        
    } else if (mgr->IsWorkspaceOpen()) {

        if (WorkspaceST::Get()->GetActiveProjectName().IsEmpty()) {
            wxLogMessage(_("Attempting to debug workspace with no active project? Ignoring."));
            return;
        }

        // Debugger is not running, but workspace is opened -> start debug session
        QueueCommand dbgCmd(QueueCommand::kDebug);
        
        wxStandardID res = ::PromptForYesNoDialogWithCheckbox(_("Would you like to build the project before debugging it?"), 
                                                              "BuildBeforeDebug",
                                                              _("Build and Debug"), _("Debug"));
        // Don't do anything if "X" is pressed
        if ( res != wxID_CANCEL ) {
            if ( res == wxID_YES ) {
                QueueCommand bldCmd( QueueCommand::kBuild );
                ManagerST::Get()->PushQueueCommand(bldCmd);
                dbgCmd.SetCheckBuildSuccess(true);
            }

            // place a debug command
            ManagerST::Get()->PushQueueCommand(dbgCmd);

            // trigger the commands queue
            ManagerST::Get()->ProcessCommandQueue();
        }
    } else if ( !mgr->IsWorkspaceOpen() ) {
        // Run the 'Quick Debug'
        OnQuickDebug( e );
    }
}

void clMainFrame::OnDebugUI(wxUpdateUIEvent &e)
{
    CHECK_SHUTDOWN();
    e.Enable( !ManagerST::Get()->IsBuildInProgress() );
}

void clMainFrame::OnDebugRestart(wxCommandEvent &e)
{
    clDebugEvent event(wxEVT_DBG_UI_RESTART);
    if ( EventNotifier::Get()->ProcessEvent( event ) ) 
        return;
        
    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        GetDebuggerPane()->GetLocalsTable()->Clear();
        dbgr->Restart();
    }
}

void clMainFrame::OnDebugRestartUI(wxUpdateUIEvent &e)
{
    CHECK_SHUTDOWN();
    
    clDebugEvent event(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent( event );
    
    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    e.Enable(event.IsAnswer() || (dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) );
}

void clMainFrame::OnDebugStop(wxCommandEvent &e)
{
    wxUnusedVar(e);
    clDebugEvent debugEvent(wxEVT_DBG_UI_STOP);
    if ( EventNotifier::Get()->ProcessEvent( debugEvent ) ) {
        return;
    }
    ManagerST::Get()->DbgStop();
}

void clMainFrame::OnDebugStopUI(wxUpdateUIEvent &e)
{
    CHECK_SHUTDOWN();
    
    clDebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent( eventIsRunning );

    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    e.Enable( eventIsRunning.IsAnswer() || (dbgr && dbgr->IsRunning()) );
}

void clMainFrame::OnDebugManageBreakpointsUI(wxUpdateUIEvent &e)
{
    if (e.GetId() == XRCID("delete_all_breakpoints")) {
        std::vector<BreakpointInfo> bps;
        ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(bps);
        e.Enable(bps.size());
    } else if (e.GetId() == XRCID("disable_all_breakpoints")) {
        e.Enable(ManagerST::Get()->GetBreakpointsMgr()->AreThereEnabledBreakpoints());
    } else if (e.GetId() == XRCID("enable_all_breakpoints")) {
        e.Enable(ManagerST::Get()->GetBreakpointsMgr()->AreThereDisabledBreakpoints());
    } else {

        e.Enable(true);
    }
}

void clMainFrame::OnDebugCmd(wxCommandEvent &e)
{
    int cmd(wxNOT_FOUND);
    int eventId(wxNOT_FOUND);
    
    if (e.GetId() == XRCID("pause_debugger")) {
        cmd = DBG_PAUSE;
        eventId = wxEVT_DBG_UI_INTERRUPT;
        
    } else if (e.GetId() == XRCID("dbg_stepin")) {
        cmd = DBG_STEPIN;
        eventId = wxEVT_DBG_UI_STEP_IN;
        
    } else if (e.GetId() == XRCID("dbg_stepout")) {
        cmd = DBG_STEPOUT;
        eventId = wxEVT_DBG_UI_STEP_OUT;
        
    } else if (e.GetId() == XRCID("dbg_next")) {
        cmd = DBG_NEXT;
        eventId = wxEVT_DBG_UI_NEXT;
        
    } else if (e.GetId() == XRCID("show_cursor")) {
        cmd = DBG_SHOW_CURSOR;
        eventId = wxEVT_DBG_UI_SHOW_CURSOR;
        
    } else if ( e.GetId() == XRCID("dbg_nexti")) {
        cmd = DBG_NEXTI;
        eventId = wxEVT_DBG_UI_NEXT_INST;
        
    }
    
    // ALlow the plugins to handle this command first
    clDebugEvent evnt(eventId);
    if ( EventNotifier::Get()->ProcessEvent( evnt ) ) {
        return;
    }
    
    if (cmd != wxNOT_FOUND) {
        ManagerST::Get()->DbgDoSimpleCommand(cmd);
    }
}

void clMainFrame::OnDebugCmdUI(wxUpdateUIEvent &e)
{
    CHECK_SHUTDOWN();
    
    clDebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
    EventNotifier::Get()->ProcessEvent( eventIsRunning );
    
    if (e.GetId() == XRCID("pause_debugger") ||
        e.GetId() == XRCID("dbg_stepin") ||
        e.GetId() == XRCID("dbg_stepout") ||
        e.GetId() == XRCID("dbg_next") ||
        e.GetId() == XRCID("dbg_nexti") ||
        e.GetId() == XRCID("show_cursor")) {
        IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
        e.Enable(eventIsRunning.IsAnswer() || (dbgr && dbgr->IsRunning()) );
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
    e.Skip();

    // make sure that we are always set to the working directory of the workspace
    if ( WorkspaceST::Get()->IsOpen() ) {
        // Check that current working directory is set to the workspace folder
        wxString path = WorkspaceST::Get()->GetWorkspaceFileName().GetPath();
        wxString curdir = ::wxGetCwd();
        if ( path != curdir ) {
            // Check that it really *is* different, not just a symlink issue: see bug #942
            if ( CLRealPath(path) != CLRealPath(curdir) ) {
                wxLogMessage("Current working directory is reset to %s", path);
                ::wxSetWorkingDirectory( path );
            }
        }
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
        clWindowUpdateLocker locker(this);
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

void clMainFrame::SetFrameFlag(bool set, int flag)
{
    if (set) {
        m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() | flag);
    } else {
        m_frameGeneralInfo.SetFlags(m_frameGeneralInfo.GetFlags() & ~(flag));
    }
}

void clMainFrame::OnShowWelcomePageUI(wxUpdateUIEvent &event)
{
    CHECK_SHUTDOWN();
    event.Enable(GetMainBook()->FindPage(_("Welcome!")) == NULL);
}

void clMainFrame::OnShowWelcomePage(wxCommandEvent &event)
{
    ShowWelcomePage();
}

void clMainFrame::CompleteInitialization()
{
    // Load the plugins
    PluginManager::Get()->Load();

    // Load debuggers (*must* be after the plugins)
    DebuggerMgr::Get().Initialize(this, EnvironmentConfig::Instance(), ManagerST::Get()->GetInstallDir());
    DebuggerMgr::Get().LoadDebuggers();

    // Connect some system events
    m_mgr.Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(clMainFrame::OnDockablePaneClosed), NULL, this);
    //m_mgr.Connect(wxEVT_AUI_RENDER,     wxAuiManagerEventHandler(clMainFrame::OnAuiManagerRender),   NULL, this);
    Layout();
    SelectBestEnvSet();

    // Now everything is loaded, set the saved tab-order in the workspace pane
    GetWorkspacePane()->ApplySavedTabOrder();
    ManagerST::Get()->GetPerspectiveManager().ConnectEvents( &m_mgr );

    wxCommandEvent evt(wxEVT_CL_THEME_CHANGED);
    EventNotifier::Get()->AddPendingEvent( evt );
    
#ifndef __WXMSW__
    sigset_t child_set;
    sigemptyset (&child_set);
    sigaddset (&child_set, SIGCHLD);
    
    // make sure SIGCHILD is not blocked
    sigprocmask (SIG_UNBLOCK, &child_set, NULL);

    // Start the Zombie Reaper thread
    m_zombieReaper.Start();

#endif
}

void clMainFrame::OnAppActivated(wxActivateEvent &e)
{
#ifndef __WXMAC__
    CodeCompletionBox::Get().CancelTip();
#endif
    if (m_theFrame && e.GetActive()) {

        // if workspace or project was modified, don't prompt for 
        // file(s) reload
        if ( !m_theFrame->ReloadExternallyModifiedProjectFiles() ) {
            m_theFrame->GetMainBook()->ReloadExternallyModified( true );
        }
        
        // Notify plugins that we got the focus.
        // Some plugins want to hide some frames etc
        wxCommandEvent evtGotFocus(wxEVT_CODELITE_MAINFRAME_GOT_FOCUS);
        EventNotifier::Get()->AddPendingEvent( evtGotFocus );

    } else if(m_theFrame) {

#ifndef __WXMAC__
        /// this code causes crash on Mac, since it destorys an active CCBox
        LEditor *editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            // we are loosing the focus
            editor->CallTipCancel();
            editor->HideCompletionBox();
        }
#endif

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
        GetMainBook()->CallAfter( &MainBook::CloseAllButThisVoid, win );
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
        int eol(wxSTC_EOL_LF);
        if (e.GetId() == XRCID("convert_eol_win")) {
            eol = wxSTC_EOL_CRLF;
        } else if (e.GetId() == XRCID("convert_eol_mac")) {
            eol = wxSTC_EOL_CR;
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
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
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

    // Notify all open editors that word hight is checked
    wxCommandEvent evtEnable(wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT);
    if ( !highlightWord ) {
        GetMainBook()->HighlightWord(true);
        EditorConfigST::Get()->SaveLongValue(wxT("highlight_word"), 1);
        evtEnable.SetInt(1);

    } else {
        GetMainBook()->HighlightWord(false);
        EditorConfigST::Get()->SaveLongValue(wxT("highlight_word"), 0);
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

void clMainFrame::OnSingleInstanceOpenFiles(wxCommandEvent& e)
{
    wxArrayString *arr = reinterpret_cast<wxArrayString*>(e.GetClientData());
    if (arr) {
        for (size_t i=0; i<arr->GetCount(); i++) {
            wxFileName fn(arr->Item(i));

            // if file is workspace, load it
            if (fn.GetExt() == wxT("workspace")) {
                if ( ManagerST::Get()->IsWorkspaceOpen() ) {
                    if (wxMessageBox(_("Close this workspace, and load workspace '") + fn.GetFullName() + wxT("'"), _("CodeLite"), wxICON_QUESTION|wxYES_NO, this) == wxNO) {
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
            btn.buttonLabel = _("Download Now!");
            btn.commandId   = XRCID("goto_codelite_download_url");
            btn.isDefault   = true;
            btn.window      = this;

            GetMainBook()->ShowMessage(_("A new version of codelite is available"), true, PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/software_upgrade")), btn);

        } else {
            if (!data->GetShowMessage()) {
                wxLogMessage(wxString() << "Info: codelite is up-to-date (or newer), version used: "
                                        << data->GetCurVersion()
                                        << ", version on site: "
                                        << data->GetNewVersion());
            } else {
                // User initiated the version check request
                GetMainBook()->ShowMessage(_("CodeLite is up-to-date"));
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
    wxBitmap  bmp  = GetWorkspacePane()->GetNotebook()->GetPageBitmap(sel); // We might have a bitmap on the tab, make sure we restore it

#if !CL_USE_NATIVEBOOK
    DockablePane *pane = new DockablePane(this, GetWorkspacePane()->GetNotebook(), text, bmp, wxSize(200, 200));
    page->Reparent(pane);

    // remove the page from the notebook
    GetWorkspacePane()->GetNotebook()->RemovePage(sel, false);
    pane->SetChildNoReparent(page);

#else

    DockablePane *pane = new DockablePane(this, GetWorkspacePane()->GetNotebook(), text, bmp, wxSize(200, 200));
    GetWorkspacePane()->GetNotebook()->RemovePage(sel, false);
#if wxVERSION_NUMBER < 2903
    // HACK: since Reparent will remove the widget from the parent, we need to place it back... (This was fixed in wxGTK-2.9.3)
    gtk_container_add( GTK_CONTAINER(GetWorkspacePane()->GetNotebook()->m_widget), page->m_widget );
#endif
    page->Reparent(pane);
    pane->SetChildNoReparent(page);

#endif

    wxUnusedVar(e);
}

void clMainFrame::OnNewDetachedPane(wxCommandEvent &e)
{
    DockablePane *pane = (DockablePane*)(e.GetClientData());
    if (pane) {
        wxString text = pane->GetName();
        m_DPmenuMgr->AddMenu(text);

        // keep list of all detached panes
        wxArrayString panes = m_DPmenuMgr->GetDeatchedPanesList();
        DetachedPanesInfo dpi(panes);
        EditorConfigST::Get()->WriteObject(wxT("DetachedPanesList"), &dpi);

        m_mgr.AddPane(pane, wxAuiPaneInfo().Name(text).Caption(text).Float());
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

        // keep list of all detached panes
        wxArrayString panes = m_DPmenuMgr->GetDeatchedPanesList();
        DetachedPanesInfo dpi(panes);
        EditorConfigST::Get()->WriteObject(wxT("DetachedPanesList"), &dpi);

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
    wxAuiPaneInfo* pInfo = e.GetPane();
    if (pInfo->IsOk()) {
        DockablePaneMenuManager::HackHidePane(false,*pInfo,&m_mgr);
    }
    if (pane) {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
        pane->GetEventHandler()->ProcessEvent(evt);
    } else {
        e.Skip();
    }
}

void clMainFrame::SetStatusMessage(const wxString &msg, int col, int seconds_to_live /*=wxID_ANY*/)
{
    SetStatusText(msg, col);
    // if ((col > 0)						// We only auto-delete in column 0
    //     || (seconds_to_live == 0)	// which means keep forever
    //     || msg.empty()) {			// not much point deleting an empty string
    //     return;
    // }
    // 
    // int seconds = (seconds_to_live > 0 ? seconds_to_live : 30);
    // m_statusbarTimer->Start(seconds * 1000, wxTIMER_ONE_SHOT);
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
    if ( EventNotifier::Get()->ProcessEvent(e) )
        return; // this event was handled by a plugin

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if ( dbgr && dbgr->IsRunning() ) {
        return;
    }

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
        QueueCommand buildInfo(projectName, conf, false, QueueCommand::kClean);
        if (bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
            buildInfo.SetCustomBuildTarget(wxT("Clean"));
        }
        ManagerST::Get()->PushQueueCommand(buildInfo);

        // now we place a build command
        buildInfo = QueueCommand(projectName, conf, false, QueueCommand::kBuild);

        if (bldConf && bldConf->IsCustomBuild()) {
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
    
    wxString pattern   = clConfig::Get().Read("FrameTitlePattern", wxString("$workspace $fullpath"));
    wxString username  = ::wxGetUserId();
    username.Prepend("[ ").Append(" ]");
    
    wxString workspace = WorkspaceST::Get()->GetName();
    if ( !workspace.IsEmpty() ) {
        workspace.Prepend("[ ").Append(" ]");
    }
    
    wxString fullname, fullpath;
    // We support the following macros:
    if ( editor ) {
        fullname = editor->GetFileName().GetFullName();
        fullpath = editor->GetFileName().GetFullPath();
    }
    
    pattern.Replace("$workspace", workspace);
    pattern.Replace("$user",      username);
    pattern.Replace("$filename",  fullname);
    pattern.Replace("$fullpath",  fullpath);
    
    pattern.Trim().Trim(false);
    if ( pattern.IsEmpty() ) {
        pattern << "CodeLite";
    }
    
    title << pattern;
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
    wxBitmap  bmp  = GetDebuggerPane()->GetNotebook()->GetPageBitmap(sel);

#if !CL_USE_NATIVEBOOK
    DockablePane *pane = new DockablePane(this, GetDebuggerPane()->GetNotebook(), text, bmp, wxSize(200, 200));
    page->Reparent(pane);

    // remove the page from the notebook
    GetDebuggerPane()->GetNotebook()->RemovePage(sel, false);
    pane->SetChildNoReparent(page);

#else

    DockablePane *pane = new DockablePane(this, GetDebuggerPane()->GetNotebook(), text, bmp, wxSize(200, 200));
    GetDebuggerPane()->GetNotebook()->RemovePage(sel, false);
    // HACK: since Reparent will remove the widget from the parent, we need to place it back...
    gtk_container_add( GTK_CONTAINER(GetDebuggerPane()->GetNotebook()->m_widget), page->m_widget );
    page->Reparent(pane);
    pane->SetChildNoReparent(page);

#endif
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
        if (!ProcUtils::Shell( EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand() )) {
            wxMessageBox(wxString::Format(wxT("Failed to open shell at '%s'"), filepath.c_str()), _("CodeLite"), wxICON_WARNING|wxOK);
        }
    }
}

void clMainFrame::ShowWelcomePage()
{
    wxWindow *win = GetMainBook()->FindPage(_("Welcome!"));
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

    if (dlg->restartRequired) {
        DoSuggestRestart();
    }

    dlg->Destroy();
}

void clMainFrame::OnQuickDebug(wxCommandEvent& e)
{
    // launch the debugger
    QuickDebugDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK) {

        DebuggerMgr::Get().SetActiveDebugger(dlg.GetDebuggerName());
        IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();

        if (dbgr && !dbgr->IsRunning()) {

            std::vector<BreakpointInfo> bpList;
            wxString exepath = dlg.GetExe();
            wxString wd = dlg.GetWorkingDirectory();
            wxArrayString cmds = dlg.GetStartupCmds();

            // update the debugger information
            DebuggerInformation dinfo;
            DebuggerMgr::Get().GetDebuggerInformation(dlg.GetDebuggerName(), dinfo);
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
            title << _("Debugging: ") << exepath << wxT(" ") << dlg.GetArguments();
            if ( !ManagerST::Get()->StartTTY( title, tty ) ) {
                wxMessageBox(_("Could not start TTY console for debugger!"), _("codelite"), wxOK|wxCENTER|wxICON_ERROR);
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

            dbgr->Start( si );

            // notify plugins that the debugger just started
            SendCmdEvent(wxEVT_DEBUG_STARTED, &startup_info);

            dbgr->Run(dlg.GetArguments(), wxEmptyString);

            // Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
            // so that it uses debugger_ids instead of internal_ids
            clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

            // Layout management
            ManagerST::Get()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
            ManagerST::Get()->GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);
            
        } else if ( !dbgr ) {
            
            // Fire an event, maybe a plugin wants to process this
            clDebugEvent event( wxEVT_DBG_UI_QUICK_DEBUG);
            event.SetDebuggerName(dlg.GetDebuggerName());
            event.SetExecutableName( dlg.GetExe() );
            event.SetWorkingDirectory( dlg.GetWorkingDirectory() );
            event.SetStartupCommands( wxJoin(dlg.GetStartupCmds(), '\n') );
            event.SetArguments( dlg.GetArguments() );
            EventNotifier::Get()->AddPendingEvent( event );
            
        }
    }
}

void clMainFrame::OnDebugCoreDump(wxCommandEvent& e)
{
    // launch the debugger
    DebugCoreDumpDlg *dlg = new DebugCoreDumpDlg(this);
    if (dlg->ShowModal() == wxID_OK) {

        DebuggerMgr::Get().SetActiveDebugger(dlg->GetDebuggerName());
        IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();

        if (dbgr && !dbgr->IsRunning()) {

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
            if (SendCmdEvent(wxEVT_DEBUG_STARTING, &startup_info)) {
                dlg->Destroy();
                // plugin stopped debugging
                return;
            }
            wxString tty;
            wxString title;
            title << "Debugging core: " << dlg->GetCore();
#ifndef __WXMSW__
            if ( !ManagerST::Get()->StartTTY( title, tty ) ) {
                wxMessageBox(_("Could not start TTY console for debugger!"), _("codelite"), wxOK|wxCENTER|wxICON_ERROR);
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
            dbgr->Start( si );

            // notify plugins that the debugger just started
            SendCmdEvent(wxEVT_DEBUG_STARTED, &startup_info);

            // Coredump debugging doesn't use breakpoints, but probably we should do this here anyway...
            clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

            ManagerST::Get()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
            ManagerST::Get()->GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);

            // Make sure that the debugger pane is visible, and select the stack trace tab
            wxAuiPaneInfo &info = GetDockingManager().GetPane(wxT("Debugger"));
            if ( info.IsOk() && !info.IsShown() ) {
                ManagerST::Get()->ShowDebuggerPane();
            }

            clMainFrame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::FRAMES);
            ManagerST::Get()->UpdateDebuggerPane();

            // Finally, get the call-stack and 'continue' gdb (which seems to be necessary for things to work...)
            dbgr->ListFrames();
            dbgr->Continue();
            
        } else if ( !dbgr ) {
            clDebugEvent event( wxEVT_DBG_UI_CORE_FILE);
            event.SetDebuggerName(dlg->GetDebuggerName());
            event.SetExecutableName( dlg->GetExe());
            event.SetCoreFile( dlg->GetCore() );
            event.SetWorkingDirectory( dlg->GetWorkingDirectory() );
            EventNotifier::Get()->AddPendingEvent( event );

        }
    }
    dlg->Destroy();
}

void clMainFrame::OnQuickDebugUI(wxUpdateUIEvent& e)   // (Also used by DebugCoreDump)
{
    CHECK_SHUTDOWN();
    e.Enable(true);
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
    // See if any of the plugins want to handle this event by itself
    bool fullRetag = !(event.GetId() == XRCID("retag_workspace"));
    wxCommandEvent e(fullRetag ? wxEVT_CMD_RETAG_WORKSPACE_FULL : wxEVT_CMD_RETAG_WORKSPACE, GetId());
    e.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(e))
        return;

    TagsManager::RetagType type = TagsManager::Retag_Quick_No_Scan;
    if(event.GetId() == XRCID("retag_workspace"))
        type = TagsManager::Retag_Quick;

    else if(event.GetId() == XRCID("full_retag_workspace"))
        type = TagsManager::Retag_Full;

    else if(event.GetId() == XRCID("retag_workspace_no_includes"))
        type = TagsManager::Retag_Quick_No_Scan;
    
    wxMenu *menu = dynamic_cast<wxMenu*>(event.GetEventObject());
    if ( menu ) {
        // the event was fired from the menu bar, trigger a compile_commands.json file generation
        // Generate the compile_commands files (needed for Clang)
        ManagerST::Get()->GenerateCompileCommands();
    }
    ManagerST::Get()->RetagWorkspace(type);
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
    CHECK_SHUTDOWN();

    wxString msg = e.GetString();
    int col = e.GetInt();
    int seconds_to_live = e.GetId();
    SetStatusMessage(msg, col, seconds_to_live);
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
    if ( ManagerST::Get()->IsWorkspaceOpen() == false ) {
        return false;
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
        return false;
    
    // Make sure we don't have the mouse captured in any editor or we might get a crash somewhere
    wxStandardID res = ::PromptForYesNoDialogWithCheckbox(  _("Workspace or project settings have been modified outside of CodeLite\nWould you like to reload the workspace?"), 
                                                            "ReloadWorkspaceWhenAltered", 
                                                            _("Reload workspace"), 
                                                            _("Not now"));
    // Don't do anything if "X" is pressed
    if ( res != wxID_CANCEL ) {
        if ( res == wxID_YES ) {
            wxCommandEvent evtReload(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
            GetEventHandler()->ProcessEvent( evtReload );

        } else {
            // user cancelled the dialog or chosed not to reload the workspace
            if ( GetMainBook()->GetActiveEditor() ) {
                GetMainBook()->GetActiveEditor()->CallAfter( &LEditor::SetActive );
            }
        }
    }
    return true;
}

void clMainFrame::SaveLayoutAndSession()
{
    EditorConfigST::Get()->SaveLexers();

    //save general information
    if (IsMaximized()) {
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
    EditorConfigST::Get()->SaveLongValue(wxT("ShowNavBar"), m_mainBook->IsNavBarShown() ? 1 : 0);
    GetWorkspacePane()->SaveWorkspaceViewTabOrder();

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

    // Update the current perspective - but only if it is the default perspective
    if(ManagerST::Get()->GetPerspectiveManager().IsDefaultActive()) {
        ManagerST::Get()->GetPerspectiveManager().SavePerspective();
    }

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
    // Let the plugins a chance before we handle this event
    wxCommandEvent eventOpenResource(wxEVT_CMD_OPEN_RESOURCE, GetId());
    eventOpenResource.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(eventOpenResource))
        return;

    OpenResourceDialog dlg(this, PluginManager::Get());
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

void clMainFrame::OnDatabaseUpgradeInternally(wxCommandEvent &e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
    this->AddPendingEvent(evt);
    //GetMainBook()->ShowMessage(_("Your workspace symbols file does not match the current version of CodeLite. CodeLite will perform a full retag of the workspace"));
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

    clConfig ccConfig("code-completion.conf");
    ccConfig.WriteItem( &m_tagsOptionsData );

    ParseThreadST::Get()->SetSearchPaths( tod.GetParserSearchPaths(), tod.GetParserExcludePaths() );
}

void clMainFrame::OnCheckForUpdate(wxCommandEvent& e)
{
    JobQueueSingleton::Instance()->PushJob( new WebUpdateJob(this, true) );
}

void clMainFrame::OnShowActiveProjectSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if ( !WorkspaceST::Get()->IsOpen() ) {
        return;
    }
    GetWorkspaceTab()->OpenProjectSettings();
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

    wxString displayString = wxString::Format(wxT("Env: %s, Dbg: %s"),
                                      activeSetName.c_str(),
                                      preDefTypeMap.GetActiveSet().GetName().c_str());

    SetStatusMessage(displayString, 2);
}

void clMainFrame::OnClearTagsCache(wxCommandEvent& e)
{
    e.Skip();
    TagsManagerST::Get()->ClearTagsCache();
    SetStatusMessage(_("Tags cache cleared"), 0);
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

void clMainFrame::DoSuggestRestart()
{
#ifdef __WXMAC__
    GetMainBook()->ShowMessage(_("Some of the changes made requires restart of CodeLite"));
#else
    // On Winodws & GTK we offer auto-restart
    ButtonDetails btn1, btn2;
    btn1.buttonLabel = _("Restart Now!");
    btn1.commandId   = wxEVT_CMD_RESTART_CODELITE;
    btn1.menuCommand = false;
    btn1.isDefault   = true;
    btn1.window      = ManagerST::Get();

    // set button window to NULL
    btn2.buttonLabel = _("Not now");
    btn2.window      = NULL;

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

    m_mgr.SetFlags( auiMgrFlags );
}

void clMainFrame::UpdateAUI()
{
    SetAUIManagerFlags();
    // Once loaded, update the output pane caption
    wxAuiPaneInfo& paneInfo = m_mgr.GetPane(wxT("Output View"));

    if (paneInfo.IsOk()) {
        paneInfo.CaptionVisible(true);
        m_mgr.Update();
    }
}

void clMainFrame::OnRetaggingCompelted(wxCommandEvent& e)
{
    e.Skip();
    SetStatusMessage(_("Done"), 0);
    GetWorkspacePane()->ClearProgress();

    // Clear all cached tags now that we got our database updated
    TagsManagerST::Get()->ClearAllCaches();

    // Send event notifying parsing completed
    std::vector<std::string>* files = (std::vector<std::string>*) e.GetClientData();
    if(files) {

        // Print the parsing end time
        wxLogMessage(_("INFO: Retag workspace completed in %ld seconds (%lu files were scanned)"), gStopWatch.Time()/1000, (unsigned long)files->size());
        std::vector<wxFileName> taggedFiles;
        for(size_t i=0; i<files->size(); i++) {
            taggedFiles.push_back( wxFileName(wxString(files->at(i).c_str(), wxConvUTF8)) );
        }

        SendCmdEvent ( wxEVT_FILE_RETAGGED, ( void* ) &taggedFiles );
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
    GetWorkspacePane()->UpdateProgress( e.GetInt() );
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

wxString clMainFrame::StartTTY(const wxString &title)
{
#ifndef __WXMSW__

    // Create a new TTY Console and place it in the AUI
    ConsoleFrame *console = new ConsoleFrame(this, PluginManager::Get());
    wxAuiPaneInfo paneInfo;
    paneInfo.Name(wxT("Debugger Console")).Caption(title).Dockable().FloatingSize(300, 200).CloseButton(false);
    m_mgr.AddPane(console, paneInfo);

    // Re-set the title (it might be modified by 'LoadPerspective'
    wxAuiPaneInfo& pi = m_mgr.GetPane(wxT("Debugger Console"));
    if(pi.IsOk()) {
        pi.Caption(title);
    }

    wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Debugger Console"));
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

void clMainFrame::OnGrepWord(wxCommandEvent& e)
{
    CHECK_SHUTDOWN();
    LEditor *editor = GetMainBook()->GetActiveEditor();
    if(!editor || editor->GetSelectedText().IsEmpty())
        return;

    // Prepare the search data
    bool singleFileSearch(true);
    if(e.GetId() == XRCID("grep_current_workspace")) singleFileSearch = false;

    SearchData data;
    data.SetFindString(editor->GetSelectedText());
    data.SetMatchCase        (true);
    data.SetMatchWholeWord   (true);
    data.SetRegularExpression(false);
    data.SetDisplayScope     (false);
    data.SetEncoding         (wxFontMapper::GetEncodingName(editor->GetOptions()->GetFileFontEncoding()));
    data.SetSkipComments     (false);
    data.SetSkipStrings      (false);
    data.SetColourComments   (false);

    wxArrayString files;
    wxArrayString rootDirs;
    if(singleFileSearch) {
        rootDirs.Add(wxGetTranslation(SEARCH_IN_CURRENT_FILE));
        files.Add(editor->GetFileName().GetFullPath());
    } else {
        rootDirs.Add(wxGetTranslation(SEARCH_IN_WORKSPACE));
        ManagerST::Get()->GetWorkspaceFiles(files);
    }

    data.SetRootDirs(rootDirs);
    data.SetFiles(files);
    data.UseNewTab(true);
    data.SetOwner(GetOutputPane()->GetFindResultsTab());
    SearchThreadST::Get()->PerformSearch(data);
}

void clMainFrame::OnGrepWordUI(wxUpdateUIEvent& e)
{
    CHECK_SHUTDOWN();
    LEditor *editor = GetMainBook()->GetActiveEditor();
    e.Enable(editor && !editor->GetSelectedText().IsEmpty());
}

void clMainFrame::OnPchCacheEnded(wxCommandEvent& e)
{
    e.Skip();
//	SetStatusMessage(wxString::Format(wxT("Generating PCH completed")), 0);
}

void clMainFrame::OnPchCacheStarted(wxCommandEvent& e)
{
    e.Skip();
//	wxString *filename = (wxString*)e.GetClientData();
//	wxFileName fn(*filename);
//	delete filename;
//
//	SetStatusMessage(wxString::Format(wxT("Generating PCH for file: %s"), fn.GetFullName().c_str()), 0);
}

////////////////// View -> Workspace View -> /////////////////////////////////////

void clMainFrame::OnViewShowExplorerTab(wxCommandEvent& e)
{
    DoEnableWorkspaceViewFlag(e.IsChecked(), View_Show_Explorer_Tab);
    GetWorkspacePane()->UpdateTabs();
}

void clMainFrame::OnViewShowExplorerTabUI(wxUpdateUIEvent& event)
{
    event.Check(GetWorkspacePane()->IsTabVisible(View_Show_Explorer_Tab));
}

void clMainFrame::OnViewShowTabgroups(wxCommandEvent& e)
{
    DoEnableWorkspaceViewFlag(e.IsChecked(), View_Show_Tabgroups_Tab);
    GetWorkspacePane()->UpdateTabs();
}

void clMainFrame::OnViewShowTabgroupsUI(wxUpdateUIEvent& event)
{
    event.Check(GetWorkspacePane()->IsTabVisible(View_Show_Tabgroups_Tab));
}

void clMainFrame::OnViewShowTabs(wxCommandEvent& e)
{
    DoEnableWorkspaceViewFlag(e.IsChecked(), View_Show_Tabs_Tab);
    GetWorkspacePane()->UpdateTabs();
}

void clMainFrame::OnViewShowTabsUI(wxUpdateUIEvent& event)
{
    event.Check(GetWorkspacePane()->IsTabVisible(View_Show_Tabs_Tab));
}

void clMainFrame::OnViewShowWorkspaceTab(wxCommandEvent& e)
{
    DoEnableWorkspaceViewFlag(e.IsChecked(), View_Show_Workspace_Tab);
    GetWorkspacePane()->UpdateTabs();
}

void clMainFrame::OnViewShowWorkspaceTabUI(wxUpdateUIEvent& event)
{
    event.Check(GetWorkspacePane()->IsTabVisible(View_Show_Workspace_Tab));
}

///////////////////// Helper methods /////////////////////////////

void clMainFrame::DoEnableWorkspaceViewFlag(bool enable, int flag)
{
    long flags = View_Show_Default;
    EditorConfigST::Get()->GetLongValue(wxT("view_workspace_view"), flags);
    if(enable) {
        flags |= flag;
    } else {
        flags &= ~flag;
    }
    EditorConfigST::Get()->SaveLongValue(wxT("view_workspace_view"), flags);
}

bool clMainFrame::IsWorkspaceViewFlagEnabled(int flag)
{
    long flags = View_Show_Default;
    EditorConfigST::Get()->GetLongValue(wxT("view_workspace_view"), flags);
    return (flags & flag);
}

void clMainFrame::DoUpdatePerspectiveMenu()
{
    // Locate the "perspective_menu"
    wxMenu* menu = NULL;
    GetMenuBar()->FindItem(XRCID("manage_perspectives"), &menu);
    if(!menu) {
        return;
    }

    std::vector<int> menuItemIds;
    const wxMenuItemList &items = menu->GetMenuItems();
    wxMenuItemList::const_iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* menuItem = *iter;
        if(menuItem->GetId() == wxID_SEPARATOR || menuItem->GetId() == XRCID("save_current_layout") || menuItem->GetId() == XRCID("manage_perspectives") || menuItem->GetId() == XRCID("restore_layout"))
            continue;
        menuItemIds.push_back(menuItem->GetId());
    }

    for(size_t i=0; i<menuItemIds.size(); i++) {
        menu->Delete(menuItemIds.at(i));
    }

    wxArrayString perspectives = ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives();
    for(size_t i=0; i<perspectives.GetCount(); i++) {
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


void clMainFrame::OnRefreshPerspectiveMenu(wxCommandEvent& e)
{
    DoUpdatePerspectiveMenu();
}

void clMainFrame::OnChangePerspectiveUI(wxUpdateUIEvent& e)
{
    wxString active   = ManagerST::Get()->GetPerspectiveManager().GetActive();
    wxString itemName = ManagerST::Get()->GetPerspectiveManager().NameFromMenuId(e.GetId());

    e.Check(active.CmpNoCase(itemName) == 0);
}

void clMainFrame::OnParserThreadReady(wxCommandEvent& e)
{
    e.Skip();
    ManagerST::Get()->SetRetagInProgress(false);

    if ( ManagerST::Get()->IsShutdownInProgress() ) {
        // we are in shutdown progress, dont do anything
        return;
    }
    
    wxUnusedVar(e);
    SetStatusMessage(wxEmptyString, 0);
    
    if ( e.GetInt() == ParseRequest::PR_SUGGEST_HIGHLIGHT_WORDS )
        // no need to trigger another UpdateColour
        return;

    LEditor *editor = GetMainBook()->GetActiveEditor();
    if (editor) {
        editor->UpdateColours();
    }
}

void clMainFrame::OnFileSaveUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    LEditor *editor = GetMainBook()->GetActiveEditor(true);
    if ( editor ) {
        event.Enable(editor->IsModified());

    } else {
        wxWindow *page = GetMainBook()->GetCurrentPage();
        if( page ) {
            event.Skip();

        } else {
            event.Enable(false);
        }
    }
}

void clMainFrame::OnActivateEditor(wxCommandEvent& e)
{
    LEditor* editor = dynamic_cast<LEditor*>(e.GetEventObject());
    if (editor) editor->SetActive();
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
    if ( e.IsDropDownClicked() ) {
        wxMenu* menu = BookmarkManager::Get().CreateBookmarksSubmenu(NULL); // Despite the name, this provides almost all the bookmark menuitems
        if (!menu) {
            e.Skip();
            return;
        }
        menu->Append(XRCID("removeall_bookmarks"), _("Remove All Bookmarks")); // This missing one

        wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(e.GetEventObject());
        if ( auibar ) {
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
    if ( e.IsDropDownClicked() ) {
        wxMenu menu;
        DoCreateBuildDropDownMenu(&menu);

        wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(e.GetEventObject());
        if ( auibar ) {
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

    wxMenu *buildDropDownMenu = new wxMenu;
    DoCreateBuildDropDownMenu(buildDropDownMenu);
    if ( GetToolBar() &&
         GetToolBar()->FindById(XRCID("build_active_project")) ) {
         GetToolBar()->SetDropdownMenu(XRCID("build_active_project"), buildDropDownMenu);
    }
}

void clMainFrame::DoCreateBuildDropDownMenu(wxMenu* menu)
{
    menu->Append(XRCID("build_active_project_only"), wxT("Project Only » Build"));
    menu->Append(XRCID("clean_active_project_only"), wxT("Project Only » Clean"));

    // build the menu and show it
    BuildConfigPtr bldcfg = WorkspaceST::Get()->GetProjBuildConf( WorkspaceST::Get()->GetActiveProjectName(), "" );
    if ( bldcfg && bldcfg->IsCustomBuild() ) {

        // Update teh custom targets
        CustomTargetsMgr::Get().SetTargets( WorkspaceST::Get()->GetActiveProjectName(), bldcfg->GetCustomTargets() );

        if ( !CustomTargetsMgr::Get().GetTargets().empty() ) {
            menu->AppendSeparator();
        }

        const CustomTargetsMgr::Map_t& targets = CustomTargetsMgr::Get().GetTargets();
        CustomTargetsMgr::Map_t::const_iterator iter = targets.begin();
        for( ; iter != targets.end(); ++iter ) {
            int winid = iter->first; // contains the menu ID
            menu->Append(winid,
                         iter->second.first);
        }

    } else {
        wxLogMessage("Could not locate build configuration for project: " + WorkspaceST::Get()->GetActiveProjectName() );

    }
}

void clMainFrame::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    CustomTargetsMgr::Get().Clear();

    // Reset the menu
    wxMenu* buildDropDownMenu = new wxMenu;
    if ( GetToolBar() && GetToolBar()->FindById(XRCID("build_active_project")) ) {
        GetToolBar()->SetDropdownMenu(XRCID("build_active_project"), buildDropDownMenu);
    }
}

void clMainFrame::OnIncrementalSearchUI(wxUpdateUIEvent& event)
{
    CHECK_SHUTDOWN();
    event.Enable( m_mainBook->GetCurrentPage() != NULL );
}

void clMainFrame::OnBuildProjectOnly(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_CMD_BUILD_PROJECT_ONLY);
    EventNotifier::Get()->AddPendingEvent( e );
}

void clMainFrame::OnCleanProjectOnly(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_CMD_CLEAN_PROJECT_ONLY);
    EventNotifier::Get()->AddPendingEvent( e );
}

void clMainFrame::OnFileSaveAllUI(wxUpdateUIEvent& event)
{
    bool hasModifiedEditor = false;
    std::vector<LEditor*> editors;
    GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i=0; i<editors.size(); ++i) {
        if( editors.at(i)->IsModified() ) {
            hasModifiedEditor = true;
            break;
        }
    }
    event.Enable( hasModifiedEditor );
}

void clMainFrame::OnShowDebuggerWindow(wxCommandEvent& e)
{
    // load the debugger configuration
    clConfig conf("debugger-view.conf");
    DebuggerPaneConfig item;
    conf.ReadItem( &item );

    bool show = e.IsChecked();
    if( e.GetId() == XRCID("debugger_win_locals") )
        item.ShowDebuggerWindow(DebuggerPaneConfig::Locals, show);

    if( e.GetId() == XRCID("debugger_win_watches"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Watches, show);

    if( e.GetId() == XRCID("debugger_win_output"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Output, show);

    if( e.GetId() == XRCID("debugger_win_threads"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Threads, show);

    if( e.GetId() == XRCID("debugger_win_callstack"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Callstack, show);

    if( e.GetId() == XRCID("debugger_win_memory"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Memory, show);

    if( e.GetId() == XRCID("debugger_win_breakpoints"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Breakpoints, show);

    if( e.GetId() == XRCID("debugger_win_asciiview"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::AsciiViewer, show);

    if( e.GetId() == XRCID("debugger_win_disassemble"))
        item.ShowDebuggerWindow(DebuggerPaneConfig::Disassemble, show);

    conf.WriteItem( &item );
    // Reload the perspective
    ManagerST::Get()->GetPerspectiveManager().LoadPerspective();
}

void clMainFrame::OnShowDebuggerWindowUI(wxUpdateUIEvent& e)
{
    // load the debugger configuration
    //clConfig conf("debugger-view.conf");
    DebuggerPaneConfig item;
    //conf.ReadItem( &item );

    DebuggerPaneConfig::eDebuggerWindows winid = DebuggerPaneConfig::None;

    if( e.GetId() == XRCID("debugger_win_locals") )
        winid = DebuggerPaneConfig::Locals;

    if( e.GetId() == XRCID("debugger_win_watches"))
        winid = DebuggerPaneConfig::Watches;

    if( e.GetId() == XRCID("debugger_win_output"))
        winid = DebuggerPaneConfig::Output;

    if( e.GetId() == XRCID("debugger_win_threads"))
        winid = DebuggerPaneConfig::Threads;

    if( e.GetId() == XRCID("debugger_win_callstack"))
        winid = DebuggerPaneConfig::Callstack;

    if( e.GetId() == XRCID("debugger_win_memory"))
        winid = DebuggerPaneConfig::Memory;

    if( e.GetId() == XRCID("debugger_win_breakpoints"))
        winid = DebuggerPaneConfig::Breakpoints;

    if( e.GetId() == XRCID("debugger_win_asciiview"))
        winid = DebuggerPaneConfig::AsciiViewer;

    if ( winid != DebuggerPaneConfig::None ) {
        e.Check( item.IsDebuggerWindowShown(winid) );
    }
}
void clMainFrame::OnRefactoringCacheStatus(wxCommandEvent& e)
{
    e.Skip();
    if ( e.GetInt() == 0 ) {
        // start
        wxLogMessage( wxString() << "Initializing refactoring database for workspace: " << WorkspaceST::Get()->GetName() );
    } else {
        wxLogMessage( wxString() << "Initializing refactoring database for workspace: " << WorkspaceST::Get()->GetName() << "... done" );
    }
}

void clMainFrame::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
//    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
//    wxColour fgColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
//    SetBackgroundColour( bgColour );
//    SetForegroundColour( fgColour );
//    Refresh();
}

void clMainFrame::OnChangeActiveBookmarkType(wxCommandEvent& e)
{
    LEditor* editor = GetMainBook()->GetActiveEditor();
    if (editor) {
        editor->OnChangeActiveBookmarkType(e);
    }
}

void clMainFrame::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    SetFrameTitle( GetMainBook()->GetActiveEditor() );
}

void clMainFrame::OnDetachEditor(wxCommandEvent& e)
{
    GetMainBook()->DetachActiveEditor();
}

void clMainFrame::OnDetachEditorUI(wxUpdateUIEvent& e)
{
    e.Enable( GetMainBook()->GetActiveEditor() != NULL );
}

