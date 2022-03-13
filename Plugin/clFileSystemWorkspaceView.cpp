#include "clFileSystemWorkspaceView.hpp"

#include "StringUtils.h"
#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceDlg.h"
#include "clThemedButton.h"
#include "clToolBar.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"

#include <wx/app.h>

clFileSystemWorkspaceView::clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName)
    : clTreeCtrlPanel(parent)
    , m_config("fs-workspace-config")
{
    SetConfig(&m_config);
    SetNewFileTemplate("Untitled.cpp", wxStrlen("Untitled"));
    SetViewName(viewName);

    clBitmapList* images = GetToolBar()->GetBitmaps();
    GetToolBar()->AddTool(wxID_PREFERENCES, _("Settings"), images->Add("cog"), "", wxITEM_NORMAL);
    GetToolBar()->AddTool(XRCID("fsw_refresh_current_folder"), _("Refresh"), images->Add("file_reload"), "",
                          wxITEM_NORMAL);

    GetToolBar()->Bind(wxEVT_TOOL, &clFileSystemWorkspaceView::OnSettings, this, wxID_PREFERENCES);
    GetToolBar()->AddSeparator();

    GetToolBar()->Add2StatesTool(XRCID("ID_BUILD_BUTTON"), EventNotifier::Get()->TopFrame()->GetEventHandler(),
                                 { XRCID("build_active_project"), _("Build Active Project"), images->Add("build") },
                                 { XRCID("stop_active_project_build"), _("Stop Current Build"), images->Add("stop") },
                                 wxITEM_DROPDOWN);

    GetToolBar()->Add2StatesTool(XRCID("ID_RUN_BUTTON"), EventNotifier::Get()->TopFrame()->GetEventHandler(),
                                 { XRCID("execute_no_debug"), _("Run program"), images->Add("execute") },
                                 { XRCID("stop_executed_program"), _("Stop running program"), images->Add("stop") },
                                 wxITEM_NORMAL);

    // these events are connected using the App object (to support keyboard shortcuts)
    wxTheApp->Bind(wxEVT_TOOL, &clFileSystemWorkspaceView::OnRefresh, this, XRCID("fsw_refresh_current_folder"));
    wxTheApp->Bind(wxEVT_UPDATE_UI, &clFileSystemWorkspaceView::OnRefreshUI, this, XRCID("fsw_refresh_current_folder"));

    GetToolBar()->Realize();

    m_buttonConfigs = new clThemedButton(this, wxID_ANY, wxEmptyString);
    m_buttonConfigs->SetHasDropDownMenu(true);
    m_buttonConfigs->Bind(wxEVT_BUTTON, &clFileSystemWorkspaceView::OnShowConfigsMenu, this);
    GetToolBar()->Bind(wxEVT_TOOL_DROPDOWN, &clFileSystemWorkspaceView::OnBuildActiveProjectDropdown, this,
                       XRCID("ID_BUILD_BUTTON"));
    GetSizer()->Insert(0, m_buttonConfigs, 0, wxEXPAND | wxALL, 5);

    // Hide hidden folders and files
    m_options &= ~kShowHiddenFiles;
    m_options &= ~kShowHiddenFolders;

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTED, &clFileSystemWorkspaceView::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &clFileSystemWorkspaceView::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_PROGRAM_STARTED, &clFileSystemWorkspaceView::OnProgramStarted, this);
    EventNotifier::Get()->Bind(wxEVT_PROGRAM_TERMINATED, &clFileSystemWorkspaceView::OnProgramStopped, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_DISMISSED, &clFileSystemWorkspaceView::OnFindInFilesDismissed,
                               this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_SHOWING, &clFileSystemWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clFileSystemWorkspaceView::OnThemeChanged, this);
}

clFileSystemWorkspaceView::~clFileSystemWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTED, &clFileSystemWorkspaceView::OnBuildStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_ENDED, &clFileSystemWorkspaceView::OnBuildEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_PROGRAM_STARTED, &clFileSystemWorkspaceView::OnProgramStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_PROGRAM_TERMINATED, &clFileSystemWorkspaceView::OnProgramStopped, this);
    m_buttonConfigs->Unbind(wxEVT_BUTTON, &clFileSystemWorkspaceView::OnShowConfigsMenu, this);
    GetToolBar()->Unbind(wxEVT_TOOL_DROPDOWN, &clFileSystemWorkspaceView::OnBuildActiveProjectDropdown, this,
                         XRCID("ID_BUILD_BUTTON"));
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_DLG_DISMISSED, &clFileSystemWorkspaceView::OnFindInFilesDismissed,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &clFileSystemWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clFileSystemWorkspaceView::OnThemeChanged, this);
}

void clFileSystemWorkspaceView::OnFolderDropped(clCommandEvent& event)
{
    // Add only non existent folders to the workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.size() != 1) {
        return;
    }

    clFileSystemWorkspace::Get().New(folders.Item(0));
    ::clGetManager()->GetWorkspaceView()->SelectPage(GetViewName());
}

void clFileSystemWorkspaceView::OnContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    wxMenu* menu = event.GetMenu();
    m_selectedFolders.clear();

    // these entries are added even when not fired from our workspace view, but as along as we have an opened workspace
    if(clFileSystemWorkspace::Get().IsOpen()) {
        clTreeCtrlPanel* tree = dynamic_cast<clTreeCtrlPanel*>(event.GetEventObject());
        if(tree) {
            wxArrayString selected_files;
            tree->GetSelections(m_selectedFolders, selected_files);
            wxUnusedVar(selected_files);
        }

        wxMenu* cc_menu = new wxMenu;
        if(!m_selectedFolders.empty()) {
            cc_menu->Append(XRCID("fs_add_cc_inculde"), _("Add path to code completion"), wxEmptyString, wxITEM_NORMAL);
            cc_menu->Bind(wxEVT_MENU, &clFileSystemWorkspaceView::OnAddIncludePath, this, XRCID("fs_add_cc_inculde"));
        }
        cc_menu->Append(XRCID("fs_create_compile_flags"), _("Generate compile_flags.txt file..."), wxEmptyString,
                        wxITEM_NORMAL);
        cc_menu->Bind(wxEVT_MENU, &clFileSystemWorkspaceView::OnCreateCompileFlagsFile, this,
                      XRCID("fs_create_compile_flags"));
        menu->AppendSubMenu(cc_menu, _("Code Completion"), wxEmptyString);
        menu->AppendSeparator();
        menu->Append(XRCID("fs_exclude_path"), _("Exclude this folder"), wxEmptyString, wxITEM_NORMAL);
        menu->Bind(wxEVT_MENU, &clFileSystemWorkspaceView::OnExcludePath, this, XRCID("fs_exclude_path"));
    }

    if(event.GetEventObject() == this) {
        event.Skip(false);
        menu->AppendSeparator();
        menu->Append(wxID_PREFERENCES, _("Settings..."), _("Settings"), wxITEM_NORMAL);
        menu->Bind(wxEVT_MENU, &clFileSystemWorkspaceView::OnSettings, this, wxID_PREFERENCES);
    }
}

void clFileSystemWorkspaceView::OnCloseFolder(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clFileSystemWorkspace::Get().CallAfter(&clFileSystemWorkspace::Close);
}

void clFileSystemWorkspaceView::OnSettings(wxCommandEvent& event)
{
    clFileSystemWorkspaceDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }
}

void clFileSystemWorkspaceView::UpdateConfigs(const wxArrayString& configs, const wxString& selectedConfig)
{
    m_configs = configs;
    m_buttonConfigs->SetText(selectedConfig);
}

void clFileSystemWorkspaceView::OnShowConfigsMenu(wxCommandEvent& event)
{
    wxMenu menu;
    for(const wxString& config : m_configs) {
        int menuItemid = wxXmlResource::GetXRCID(config);
        menu.Append(menuItemid, config, config, wxITEM_NORMAL);
        menu.Bind(
            wxEVT_MENU,
            [=](wxCommandEvent& menuEvent) {
                m_buttonConfigs->SetText(config);
                clFileSystemWorkspace::Get().GetSettings().SetSelectedConfig(config);
                clFileSystemWorkspace::Get().Save(true);
            },
            menuItemid);
    }
    m_buttonConfigs->ShowMenu(menu);
}

void clFileSystemWorkspaceView::OnRefresh(wxCommandEvent& event)
{
    clTreeCtrlPanel::OnRefresh(event);
    clFileSystemWorkspace::Get().FileSystemUpdated();
}

void clFileSystemWorkspaceView::OnBuildStarted(clBuildEvent& event)
{
    event.Skip();
    m_buildInProgress = true;
    clDEBUG() << "Build started";
    m_toolbar->SetButtonAction(XRCID("ID_BUILD_BUTTON"), XRCID("stop_active_project_build"));
}

void clFileSystemWorkspaceView::OnBuildEnded(clBuildEvent& event)
{
    event.Skip();
    m_buildInProgress = false;
    clDEBUG() << "Build ended";
    m_toolbar->SetButtonAction(XRCID("ID_BUILD_BUTTON"), XRCID("build_active_project"));
}

void clFileSystemWorkspaceView::OnProgramStarted(clExecuteEvent& event)
{
    event.Skip();
    m_runInProgress = true;
    clDEBUG() << "Run started";
    m_toolbar->SetButtonAction(XRCID("ID_RUN_BUTTON"), XRCID("stop_executed_program"));
}

void clFileSystemWorkspaceView::OnProgramStopped(clExecuteEvent& event)
{
    event.Skip();
    m_runInProgress = false;
    clDEBUG() << "Run ended";
    m_toolbar->SetButtonAction(XRCID("ID_RUN_BUTTON"), XRCID("execute_no_debug"));
}

void clFileSystemWorkspaceView::OnBuildActiveProjectDropdown(wxCommandEvent& event)
{
    clDEBUG() << "OnBuildActiveProjectDropdown called";
    wxUnusedVar(event);
    // we dont allow showing the dropdown during build process
    if(m_buildInProgress) {
        return;
    }
    clGetManager()->ShowBuildMenu(m_toolbar, XRCID("ID_BUILD_BUTTON"));
}

void clFileSystemWorkspaceView::OnFindInFilesDismissed(clFindInFilesEvent& event)
{
    event.Skip();
    if(clFileSystemWorkspace::Get().IsOpen()) {
        clConfig::Get().Write("FindInFiles/FS/Mask", event.GetFileMask());
        clConfig::Get().Write("FindInFiles/FS/LookIn", event.GetPaths());
    }
}

void clFileSystemWorkspaceView::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    event.Skip();
    if(clFileSystemWorkspace::Get().IsOpen()) {
        // Load the C++ workspace values from the configuration
        event.SetFileMask(clConfig::Get().Read("FindInFiles/FS/Mask",
                                               wxString("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc;"
                                                        "*.xml;*.json;*.sql;*.txt;*.plist;CMakeLists.txt;*.rc;*.iss")));
        event.SetPaths(clConfig::Get().Read("FindInFiles/FS/LookIn", wxString("<Entire Workspace>")));
    }
}

void clFileSystemWorkspaceView::OnAddIncludePath(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // get list of selected folders in the UI
    wxArrayString configs = clFileSystemWorkspace::Get().GetSettings().GetConfigs();
    for(const wxString& config : configs) {
        auto config_ptr = clFileSystemWorkspace::Get().GetSettings().GetConfig(config);
        DoAddIncludePathsToConfig(config_ptr, m_selectedFolders);
    }
    clFileSystemWorkspace::Get().Save(true);
}

void clFileSystemWorkspaceView::DoAddIncludePathsToConfig(clFileSystemWorkspaceConfig::Ptr_t config,
                                                          const wxArrayString& paths)
{
    // build a map with list of folders already included
    // for this configuration
    const auto& files = config->GetCompileFlags();
    wxStringSet_t map;
    for(auto file : files) {
        if(file.StartsWith("-I")) {
            file.Remove(0, 2);
        }
        map.insert(file);
    }

    wxArrayString pathsToAdd;
    for(const auto& selected_path : paths) {
        // only add folders that do not already exist for this configuration
        if(map.count(selected_path) == 0) {
            pathsToAdd.Add("-I" + selected_path);
        }
    }

    // Add this to all the configurations
    wxArrayString compile_flags_arr = config->GetCompileFlags();
    compile_flags_arr.insert(compile_flags_arr.end(), pathsToAdd.begin(), pathsToAdd.end());
    config->SetCompileFlags(compile_flags_arr);
}

void clFileSystemWorkspaceView::OnCreateCompileFlagsFile(wxCommandEvent& event)
{
    // proxy to the workspace method
    wxUnusedVar(event);
    clFileSystemWorkspace::Get().CreateCompileFlagsFile();
}

void clFileSystemWorkspaceView::OnExcludePath(wxCommandEvent& event)
{
    // add the selected folders to the exclude path
    // get list of selected folders in the UI
    wxArrayString configs = clFileSystemWorkspace::Get().GetSettings().GetConfigs();
    for(const wxString& config : configs) {
        auto config_ptr = clFileSystemWorkspace::Get().GetSettings().GetConfig(config);

        // get the list of exclude paths and convert them into a SET to avoid duplications
        // next, we add each selected folder to the exclude list
        wxStringSet_t S;
        wxArrayString excludePathsArr = StringUtils::BuildArgv(config_ptr->GetExecludePaths());
        S.reserve(excludePathsArr.size());
        S.insert(excludePathsArr.begin(), excludePathsArr.end());
        for(wxString folder : m_selectedFolders) {
            // Make it relative to the workspace
            wxFileName fn(folder, "dummy");
            fn.MakeRelativeTo(clFileSystemWorkspace::Get().GetFileName().GetPath());
            folder = fn.GetPath();
            if(S.count(folder) == 0) {
                S.insert(folder);
                excludePathsArr.Add(folder);
            }
        }
        config_ptr->SetExcludePaths(::wxJoin(excludePathsArr, ';'));
    }
    clFileSystemWorkspace::Get().Save(true);
}

void clFileSystemWorkspaceView::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    Refresh();
}

void clFileSystemWorkspaceView::OnRefreshUI(wxUpdateUIEvent& event)
{
    event.Enable(clFileSystemWorkspace::Get().IsOpen());
}
