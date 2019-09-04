#include "clFileSystemWorkspaceView.hpp"
#include "globals.h"
#include "clFileSystemWorkspace.hpp"
#include "clWorkspaceView.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "clFileSystemWorkspaceDlg.h"
#include "clToolBar.h"

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
    // Hide hidden folders and files
    m_options &= ~kShowHiddenFiles;
    m_options &= ~kShowHiddenFolders;

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);
}

clFileSystemWorkspaceView::~clFileSystemWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &clFileSystemWorkspaceView::OnContextMenu, this);
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
