#include "clDockerWorkspace.h"
#include "clDockerWorkspaceView.h"
#include "codelite_events.h"
#include "event_notifier.h"

clDockerWorkspaceView::clDockerWorkspaceView(wxWindow* parent)
    : clTreeCtrlPanel(parent)
{
    SetOptions(0);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clDockerWorkspaceView::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clDockerWorkspaceView::OnWorkspaceClosed, this);
}

clDockerWorkspaceView::~clDockerWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clDockerWorkspaceView::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clDockerWorkspaceView::OnWorkspaceClosed, this);
}

void clDockerWorkspaceView::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

void clDockerWorkspaceView::OnWorkspaceOpened(wxCommandEvent& event)
{
    event.Skip();
    if(clDockerWorkspace::Get()->IsOpen()) {
        wxFileName workspaceFile(event.GetString());
        AddFolder(workspaceFile.GetPath());
    }
}
