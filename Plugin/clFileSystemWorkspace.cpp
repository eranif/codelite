#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceView.hpp"
#include "clFilesCollector.h"
#include "JSON.h"
#include "globals.h"
#include "imanager.h"
#include "clWorkspaceView.h"
#include "clWorkspaceManager.h"
#include "event_notifier.h"
#include "codelite_events.h"

#define WSP_FILE_NAME "CodeLiteFS.workspace"

clFileSystemWorkspace::clFileSystemWorkspace(bool dummy)
    : m_dummy(dummy)
{
    SetWorkspaceType("File System Workspace");
    if(!dummy) {
        m_view = new clFileSystemWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook(), GetWorkspaceType());
        clGetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());

        EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &clFileSystemWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_OPEN_WORKSPACE, &clFileSystemWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clFileSystemWorkspace::OnAllEditorsClosed, this);
    }
}

clFileSystemWorkspace::~clFileSystemWorkspace()
{
    if(!m_dummy) {
        EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &clFileSystemWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_OPEN_WORKSPACE, &clFileSystemWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clFileSystemWorkspace::OnAllEditorsClosed, this);
    }
}

wxString clFileSystemWorkspace::GetActiveProjectName() const { return ""; }

wxFileName clFileSystemWorkspace::GetFileName() const { return m_filename; }

wxString clFileSystemWorkspace::GetFilesMask() const { return "*.cpp;*.c;*.txt;*.json;*.hpp;*.cc;*.cxx;*.xml"; }

wxFileName clFileSystemWorkspace::GetProjectFileName(const wxString& projectName) const
{
    wxUnusedVar(projectName);
    return wxFileName();
}

void clFileSystemWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const {}

wxString clFileSystemWorkspace::GetProjectFromFile(const wxFileName& filename) const { return ""; }

void clFileSystemWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    for(const wxString& file : m_files) {
        files.Add(file);
    }
}

wxArrayString clFileSystemWorkspace::GetWorkspaceProjects() const { return {}; }

bool clFileSystemWorkspace::IsBuildSupported() const { return true; }

bool clFileSystemWorkspace::IsProjectSupported() const { return false; }

void clFileSystemWorkspace::CacheFiles()
{
    if(!m_files.empty()) { m_files.clear(); }
    clFilesScanner fs;
    wxStringSet_t excludeFolders = { ".git", ".svn" };
    fs.Scan(GetFileName().GetPath(), m_files, GetFilesMask(), "", excludeFolders);
}

void clFileSystemWorkspace::OnBuildStarting(clBuildEvent& event) { event.Skip(); }

void clFileSystemWorkspace::OnBuildEnded(clBuildEvent& event) { event.Skip(); }

void clFileSystemWorkspace::OnOpenWorkspace(clCommandEvent& event)
{
    event.Skip();
    wxFileName workspaceFile(event.GetFileName());

    // Test that this is our workspace
    if(Load(workspaceFile)) {
        event.Skip(false);
        DoOpen();

    } else {
        m_filename.Clear();
    }
}

void clFileSystemWorkspace::OnCloseWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(m_isLoaded) {
        event.Skip(false);
        DoClose();
    }
}

bool clFileSystemWorkspace::Load(const wxFileName& file)
{
    if(m_isLoaded) { return true; }
    m_filename = file;
    JSON root(m_filename);
    if(!root.isOk()) { return false; }
    JSONItem item = root.toElement();
    wxString wt = item.namedObject("workspace_type").toString();
    if(wt != GetWorkspaceType()) { return false; }
    m_flags = item.namedObject("flags").toSize_t(m_flags);
    JSONItem arrTargets = item.namedObject("targets");
    int nCount = arrTargets.arraySize();
    for(int i = 0; i < nCount; ++i) {
        JSONItem p = arrTargets.arrayItem(i);
        if(p.arraySize() == 2) {
            wxString targetName = p.arrayItem(0).toString();
            wxString command = p.arrayItem(1).toString();
            m_buildTargets.insert({ targetName, command });
        }
    }
    // Make sure we have the default build target (Build, Clean, Rebuild)
    if(m_buildTargets.count("Build") == 0) { m_buildTargets.insert({ "Build", "" }); }
    if(m_buildTargets.count("Clean") == 0) { m_buildTargets.insert({ "Clean", "" }); }
    if(m_buildTargets.count("Rebuild") == 0) { m_buildTargets.insert({ "Rebuild", "" }); }
    return true;
}

void clFileSystemWorkspace::Save()
{
    if(m_isLoaded) { return; }
    if(!m_filename.IsOk()) { return; }

    JSON root(cJSON_Object);
    JSONItem item = root.toElement();
    item.addProperty("workspace_type", GetWorkspaceType());
    item.addProperty("flags", m_flags);
    JSONItem arrTargets = JSONItem::createArray("targets");
    item.append(arrTargets);

    for(const auto& vt : m_buildTargets) {
        JSONItem target = JSONItem::createArray();
        target.arrayAppend(vt.first);
        target.arrayAppend(vt.second);
        arrTargets.arrayAppend(target);
    }
    root.save(GetFileName());
}

void clFileSystemWorkspace::RestoreSession()
{
    // Restore any session
}

void clFileSystemWorkspace::DoOpen()
{ // Init the view
    GetView()->Clear();
    GetView()->AddFolder(GetFileName().GetPath());

    // Notify CodeLite that this workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // Notify that the a new workspace is loaded
    wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
    event.SetString(GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(event);

    // and finally, request codelite to keep this workspace in the recently opened workspace list
    clGetManager()->AddWorkspaceToRecentlyUsedList(m_filename);

    // Load the workspace session (if any)
    CallAfter(&clFileSystemWorkspace::RestoreSession);
    m_isLoaded = true;
}

void clFileSystemWorkspace::DoClose()
{
    if(!m_isLoaded) return;

    // Store the session
    clGetManager()->StoreWorkspaceSession(m_filename);

    Save();
    DoClear();

    // Clear the UI
    GetView()->Clear();

    // Notify workspace closed event
    wxCommandEvent event(wxEVT_WORKSPACE_CLOSED);
    EventNotifier::Get()->ProcessEvent(event);

    // Notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

    m_isLoaded = false;
    m_showWelcomePage = true;
}

void clFileSystemWorkspace::DoClear()
{
    m_filename.Clear();
    m_buildTargets.clear();
    m_flags = 0;
}

void clFileSystemWorkspace::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    if(m_showWelcomePage) {
        m_showWelcomePage = false;
        // Show the 'Welcome Page'
        wxFrame* frame = EventNotifier::Get()->TopFrame();
        wxCommandEvent eventShowWelcomePage(wxEVT_MENU, XRCID("view_welcome_page"));
        eventShowWelcomePage.SetEventObject(frame);
        frame->GetEventHandler()->AddPendingEvent(eventShowWelcomePage);
    }
}

clFileSystemWorkspace& clFileSystemWorkspace::Get()
{
    static clFileSystemWorkspace wsp(false);
    return wsp;
}

void clFileSystemWorkspace::New(const wxString& folder)
{
    // Create an empty workspace
    wxFileName fn(folder, WSP_FILE_NAME);

    // If an workspace is opened and it is the same one as this, dont do nothing
    if(m_isLoaded && (GetFileName() == fn)) { return; }

    // Call close here, it does nothing if a workspace is not opened
    DoClose();
    DoClear();

    if(!fn.FileExists()) {
        // Creates an empty workspace file
        m_filename = fn;
        Save();
    }

    // and load it
    if(Load(m_filename)) { DoOpen(); }
}
