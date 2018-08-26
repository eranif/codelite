#include "DockerfileSettingsDlg.h"
#include "clDockerSettings.h"
#include "clDockerWorkspace.h"
#include "clDockerWorkspaceSettings.h"
#include "clDockerWorkspaceView.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include <wx/menu.h>

clDockerWorkspaceView::clDockerWorkspaceView(wxWindow* parent)
    : clTreeCtrlPanel(parent)
    , m_config("docker-view.conf")
{
    // Set the view options
    SetConfig(&m_config);
    clDockerSettings settings;
    size_t options = 0;
    settings.Load();
    if(settings.IsLinkEditor()) { options |= kLinkToEditor; }
    SetOptions(options);
    
    SetViewName("Docker");
    SetNewFileTemplate("Untitled", wxStrlen("Untitled"));

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clDockerWorkspaceView::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clDockerWorkspaceView::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &clDockerWorkspaceView::OnFileContextMenu, this);
}

clDockerWorkspaceView::~clDockerWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clDockerWorkspaceView::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clDockerWorkspaceView::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &clDockerWorkspaceView::OnFileContextMenu, this);
}

void clDockerWorkspaceView::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();

    // Store the view settings
    clDockerSettings settings;
    settings.Load();
    settings.SetLinkEditor(m_options & kLinkToEditor);
    settings.Save();
}

void clDockerWorkspaceView::OnWorkspaceOpened(wxCommandEvent& event)
{
    event.Skip();
    if(clDockerWorkspace::Get()->IsOpen()) {
        wxFileName workspaceFile(event.GetString());
        AddFolder(workspaceFile.GetPath());
    }
}

void clDockerWorkspaceView::OnFileContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    if(event.GetEventObject() != this) { return; }
    const wxArrayString& contextMenuFiles = event.GetStrings();
    if(contextMenuFiles.size() != 1) { return; }

    if(wxFileName(contextMenuFiles.Item(0)).GetFullName() != "Dockerfile") { return; }

    event.GetMenu()->PrependSeparator();
    event.GetMenu()->Prepend(XRCID("run_dockerfile"), _("Run..."));
    event.GetMenu()->Prepend(XRCID("build_dockerfile"), _("Build..."));
    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(XRCID("ID_DOCKERFILE_SETTINGS"), _("Settings..."));
    event.GetMenu()->Bind(wxEVT_MENU,
                          [&](wxCommandEvent& evt) {
                              // Context menu requested for a "Dockerfile"
                              clDockerfile info;
                              clDockerWorkspaceSettings& settings = clDockerWorkspace::Get()->GetSettings();

                              wxArrayString folders, files;
                              GetSelections(folders, files);
                              if(files.size() != 1) { return; }
                              if(wxFileName(files.Item(0)).GetFullName() != "Dockerfile") { return; }

                              settings.GetFileInfo(files.Item(0), info);
                              info.SetPath(files.Item(0));
                              DockerfileSettingsDlg dlg(EventNotifier::Get()->TopFrame(), info);
                              if(dlg.ShowModal() != wxID_OK) { return; }
                              settings.SetFileInfo(files.Item(0), info);
                              settings.Save(clDockerWorkspace::Get()->GetFileName());
                          },
                          XRCID("ID_DOCKERFILE_SETTINGS"));
    event.GetMenu()->Bind(
        wxEVT_MENU, [&](wxCommandEvent& evt) { clDockerWorkspace::Get()->BuildDockerfile(contextMenuFiles.Item(0)); },
        XRCID("build_dockerfile"));
    event.GetMenu()->Bind(
        wxEVT_MENU, [&](wxCommandEvent& evt) { clDockerWorkspace::Get()->RunDockerfile(contextMenuFiles.Item(0)); },
        XRCID("run_dockerfile"));
}
