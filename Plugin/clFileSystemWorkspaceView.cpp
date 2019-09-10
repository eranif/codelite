#include "clFileSystemWorkspaceView.hpp"
#include "globals.h"
#include "clFileSystemWorkspace.hpp"
#include "clWorkspaceView.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "clFileSystemWorkspaceDlg.h"
#include "clToolBar.h"
#include "clThemedButton.h"

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
    GetToolBar()->AddTool(XRCID("execute_no_debug"), _("Run Active Project"), bmps->LoadBitmap("execute"),
                          _("Run Active Project"));
    GetToolBar()->AddTool(XRCID("build_active_project"), _("Build Active Project"), bmps->LoadBitmap("build"),
                          _("Build Active Project"), wxITEM_DROPDOWN);
    GetToolBar()->AddTool(XRCID("stop_active_project_build"), _("Stop Current Build"), bmps->LoadBitmap("stop"),
                          _("Stop Current Build"));
    GetToolBar()->Realize();

    m_buttonConfigs = new clThemedButton(this, wxID_ANY, wxEmptyString);
    m_buttonConfigs->SetHasDropDownMenu(true);
    m_buttonConfigs->Bind(wxEVT_BUTTON, &clFileSystemWorkspaceView::OnShowConfigsMenu, this);
    GetSizer()->Insert(0, m_buttonConfigs, 0, wxEXPAND);

    // Hide hidden folders and files
    m_options &= ~kShowHiddenFiles;
    m_options &= ~kShowHiddenFolders;

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);
}

clFileSystemWorkspaceView::~clFileSystemWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);
    m_buttonConfigs->Unbind(wxEVT_BUTTON, &clFileSystemWorkspaceView::OnShowConfigsMenu, this);
}

void clFileSystemWorkspaceView::OnFolderDropped(clCommandEvent& event)
{
    // Add only non existent folders to the workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.size() != 1) { return; }

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
        menu.Bind(
            wxEVT_MENU,
            [=](wxCommandEvent& menuEvent) {
                m_buttonConfigs->SetText(config);
                clFileSystemWorkspace::Get().GetSettings().SetSelectedConfig(config);
            },
            menuItemid);
    }
    m_buttonConfigs->ShowMenu(menu);
}
