#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceDlg.h"
#include "clFileSystemWorkspaceView.hpp"
#include "clThemedButton.h"
#include "clToolBar.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"

clFileSystemWorkspaceView::clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName)
    : clTreeCtrlPanel(parent)
    , m_config("fs-workspace-config")
{
    SetConfig(&m_config);
    SetNewFileTemplate("Untitled.cpp", wxStrlen("Untitled"));
    SetViewName(viewName);

    GetToolBar()->AddTool(wxID_PREFERENCES, _("Settings"), clGetManager()->GetStdIcons()->LoadBitmap("cog"), "",
                          wxITEM_NORMAL);
    GetToolBar()->Bind(wxEVT_TOOL, &clFileSystemWorkspaceView::OnSettings, this, wxID_PREFERENCES);
    GetToolBar()->AddSeparator();

    BitmapLoader* bmps = clGetManager()->GetStdIcons();

    GetToolBar()->Add2StatesTool(
        XRCID("ID_BUILD_BUTTON"), EventNotifier::Get()->TopFrame()->GetEventHandler(),
        { XRCID("build_active_project"), _("Build Active Project"), bmps->LoadBitmap("build") },
        { XRCID("stop_active_project_build"), _("Stop Current Build"), bmps->LoadBitmap("stop") }, wxITEM_DROPDOWN);

    GetToolBar()->Add2StatesTool(
        XRCID("ID_RUN_BUTTON"), EventNotifier::Get()->TopFrame()->GetEventHandler(),
        { XRCID("execute_no_debug"), _("Run program"), bmps->LoadBitmap("execute") },
        { XRCID("stop_executed_program"), _("Stop running program"), bmps->LoadBitmap("stop") }, wxITEM_NORMAL);

    GetToolBar()->Realize();

    m_buttonConfigs = new clThemedButton(this, wxID_ANY, wxEmptyString);
    m_buttonConfigs->SetHasDropDownMenu(true);
    m_buttonConfigs->Bind(wxEVT_BUTTON, &clFileSystemWorkspaceView::OnShowConfigsMenu, this);
    GetToolBar()->Bind(wxEVT_TOOL_DROPDOWN, &clFileSystemWorkspaceView::OnBuildActiveProjectDropdown, this,
                       XRCID("ID_BUILD_BUTTON"));
    GetSizer()->Insert(0, m_buttonConfigs, 0, wxEXPAND);

    // Hide hidden folders and files
    m_options &= ~kShowHiddenFiles;
    m_options &= ~kShowHiddenFolders;

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);

    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTED, &clFileSystemWorkspaceView::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &clFileSystemWorkspaceView::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_PROGRAM_STARTED, &clFileSystemWorkspaceView::OnProgramStarted, this);
    EventNotifier::Get()->Bind(wxEVT_PROGRAM_TERMINATED, &clFileSystemWorkspaceView::OnProgramStopped, this);
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
    if(event.GetEventObject() == this) {
        event.Skip(false);
        wxMenu* menu = event.GetMenu();
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
    dlg.ShowModal();
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
        menu.Bind(wxEVT_MENU,
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
