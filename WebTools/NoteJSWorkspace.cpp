#include "NoteJSWorkspace.h"
#include "NodeJSWorkspaceConfiguration.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "NodeJSWorkspaceView.h"
#include "globals.h"
#include "imanager.h"
#include "clWorkspaceView.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/dirdlg.h>
#include "ctags_manager.h"
#include "clWorkspaceManager.h"
#include <wx/msgdlg.h>

NodeJSWorkspace* NodeJSWorkspace::ms_workspace = NULL;

NodeJSWorkspace::NodeJSWorkspace(bool dummy)
{
    m_dummy = true;
    SetWorkspaceType("Node.js");
}

NodeJSWorkspace::NodeJSWorkspace()
    : m_clangOldFlag(false)
{
    SetWorkspaceType("Node.js");
    m_view = new NodeJSWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook(), GetWorkspaceType());
    clGetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());

    EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &NodeJSWorkspace::OnCloseWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &NodeJSWorkspace::OnNewWorkspace, this);
}

NodeJSWorkspace::~NodeJSWorkspace()
{
    if(!m_dummy) {
        EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &NodeJSWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &NodeJSWorkspace::OnNewWorkspace, this);
    }
}

bool NodeJSWorkspace::IsBuildSupported() const { return false; }
bool NodeJSWorkspace::IsProjectSupported() const { return false; }

void NodeJSWorkspace::Free()
{
    if(ms_workspace) {
        delete ms_workspace;
    }
    ms_workspace = NULL;
}

NodeJSWorkspace* NodeJSWorkspace::Get()
{
    if(!ms_workspace) {
        ms_workspace = new NodeJSWorkspace();
    }
    return ms_workspace;
}

bool NodeJSWorkspace::IsOpen() const { return m_filename.IsOk() && m_filename.Exists(); }

bool NodeJSWorkspace::Create(const wxFileName& filename)
{
    if(IsOpen()) return false;
    if(filename.Exists()) return false;
    DoClear();
    m_filename = filename;

    // By default add the workspace path
    m_folders.Add(m_filename.GetPath());
    Save();

    // We dont load the workspace
    DoClear();
    return true;
}

bool NodeJSWorkspace::Open(const wxFileName& filename)
{
    if(IsOpen()) return false;
    m_filename = filename;
    NodeJSWorkspaceConfiguration conf;
    conf.Load(m_filename);
    m_folders = conf.GetFolders();
    DoOpen(m_filename);
    return true;
}

void NodeJSWorkspace::Close()
{
    if(!IsOpen()) return;
    Save();
    DoClear();

    // disable clang for NodeJS
    clGetManager()->EnableClangCodeCompletion(m_clangOldFlag);

    // Clear the UI
    GetView()->Clear();

    // Notify workspace closed event
    wxCommandEvent event(wxEVT_WORKSPACE_CLOSED);
    EventNotifier::Get()->ProcessEvent(event);

    // notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);
}

void NodeJSWorkspace::DoClear()
{
    m_filename.Clear();
    m_folders.Clear();
}

void NodeJSWorkspace::Save()
{
    NodeJSWorkspaceConfiguration conf;
    conf.SetFolders(m_folders);
    conf.Save(m_filename);
}

void NodeJSWorkspace::OnCloseWorkspace(clCommandEvent& e)
{
    e.Skip();
    if(IsOpen()) {
        e.Skip(false);
        Close();
    }
}

void NodeJSWorkspace::OnNewWorkspace(wxCommandEvent& e)
{
    e.Skip();
    if(e.GetString() == GetWorkspaceType()) {
        e.Skip(false);
        // Create a new NodeJS workspace
        wxString path = ::wxDirSelector(_("Select a folder"), "", wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON);
        if(path.IsEmpty()) return;

        wxFileName workspaceFile(path, "");
        if(!workspaceFile.GetDirCount()) {
            ::wxMessageBox(
                _("Can not create workspace in the root folder"), _("New Workspace"), wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        workspaceFile.SetName(workspaceFile.GetDirs().Last());
        workspaceFile.SetExt("workspace");
        if(!Create(workspaceFile)) {
            ::wxMessageBox(_("Failed to create workspace\nWorkspace already exists"),
                           _("New Workspace"),
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        DoOpen(workspaceFile);
    }
}

void NodeJSWorkspace::DoOpen(const wxFileName& filename)
{
    Open(filename);

    const wxArrayString& folders = GetFolders();
    for(size_t i = 0; i < folders.size(); ++i) {
        GetView()->AddFolder(folders.Item(i));
    }

    // Notify codelite that NodeJS workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // Keep the old clang state before we disable it
    const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
    m_clangOldFlag = (options.GetClangOptions() & CC_CLANG_ENABLED);

    clGetManager()->EnableClangCodeCompletion(false);

    // Notify that the a new workspace is loaded
    wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
    event.SetString(filename.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(event);
}
