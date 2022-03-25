#include "clFileSystemWorkspace.hpp"

#include "CompileCommandsGenerator.h"
#include "JSON.h"
#include "NewFileSystemWorkspaceDialog.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "build_settings_config.h"
#include "clConsoleBase.h"
#include "clFileSystemEvent.h"
#include "clFileSystemWorkspaceView.hpp"
#include "clFilesCollector.h"
#include "clKeyboardManager.h"
#include "clSFTPEvent.h"
#include "clShellHelper.hpp"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "compiler_command_line_parser.h"
#include "ctags_manager.h"
#include "debuggermanager.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macromanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include "shell_command.h"

#include <thread>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>
#include <wxStringHash.h>

#define CHECK_ACTIVE_CONFIG()                \
    if(!GetSettings().GetSelectedConfig()) { \
        return;                              \
    }

#define CHECK_EVENT(e)     \
    {                      \
        if(!IsOpen()) {    \
            e.Skip();      \
            return;        \
        }                  \
        event.Skip(false); \
    }

wxDEFINE_EVENT(wxEVT_FS_SCAN_COMPLETED, clFileSystemEvent);
wxDEFINE_EVENT(wxEVT_FS_NEW_WORKSPACE_FILE_CREATED, clFileSystemEvent);
clFileSystemWorkspace::clFileSystemWorkspace(bool dummy)
    : m_dummy(dummy)
{
    SetWorkspaceType("File System Workspace");
    if(!dummy) {
        EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &clFileSystemWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_OPEN_WORKSPACE, &clFileSystemWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_RELOAD_WORKSPACE, &clFileSystemWorkspace::OnReloadWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clFileSystemWorkspace::OnNewWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clFileSystemWorkspace::OnAllEditorsClosed, this);
        EventNotifier::Get()->Bind(wxEVT_FS_SCAN_COMPLETED, &clFileSystemWorkspace::OnScanCompleted, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_RETAG_WORKSPACE, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_RETAG_WORKSPACE_FULL, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_SAVE_SESSION_NEEDED, &clFileSystemWorkspace::OnSaveSession, this);
        EventNotifier::Get()->Bind(wxEVT_SOURCE_CONTROL_PULLED, &clFileSystemWorkspace::OnSourceControlPulled, this);

        // Build events
        EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &clFileSystemWorkspace::OnBuildStarting, this);
        EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &clFileSystemWorkspace::OnStopBuild, this);
        EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clFileSystemWorkspace::OnIsBuildInProgress, this);
        EventNotifier::Get()->Bind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING, &clFileSystemWorkspace::OnCustomTargetMenu,
                                   this);

        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clFileSystemWorkspace::OnBuildProcessTerminated, this);
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clFileSystemWorkspace::OnBuildProcessOutput, this);
        Bind(wxEVT_TERMINAL_EXIT, &clFileSystemWorkspace::OnExecProcessTerminated, this);

        // Exec events
        EventNotifier::Get()->Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clFileSystemWorkspace::OnExecute, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_IS_PROGRAM_RUNNING, &clFileSystemWorkspace::OnIsProgramRunning, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clFileSystemWorkspace::OnStopExecute, this);
        // Debug events
        EventNotifier::Get()->Bind(wxEVT_QUICK_DEBUG_DLG_SHOWING, &clFileSystemWorkspace::OnQuickDebugDlgShowing, this);
        EventNotifier::Get()->Bind(wxEVT_QUICK_DEBUG_DLG_DISMISSED_OK, &clFileSystemWorkspace::OnQuickDebugDlgDismissed,
                                   this);

        EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clFileSystemWorkspace::OnFileSaved, this);
        EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &clFileSystemWorkspace::OnDebug, this);

        EventNotifier::Get()->Bind(wxEVT_FILE_CREATED, &clFileSystemWorkspace::OnFileSystemUpdated, this);
    }
}

clFileSystemWorkspace::~clFileSystemWorkspace()
{
    if(!m_dummy) {
        EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &clFileSystemWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_OPEN_WORKSPACE, &clFileSystemWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_RELOAD_WORKSPACE, &clFileSystemWorkspace::OnReloadWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clFileSystemWorkspace::OnNewWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clFileSystemWorkspace::OnAllEditorsClosed, this);
        EventNotifier::Get()->Unbind(wxEVT_FS_SCAN_COMPLETED, &clFileSystemWorkspace::OnScanCompleted, this);
        EventNotifier::Get()->Unbind(wxEVT_SAVE_SESSION_NEEDED, &clFileSystemWorkspace::OnSaveSession, this);

        // parsing event
        EventNotifier::Get()->Unbind(wxEVT_CMD_RETAG_WORKSPACE, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_RETAG_WORKSPACE_FULL, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_SOURCE_CONTROL_PULLED, &clFileSystemWorkspace::OnSourceControlPulled, this);

        // Build events
        EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &clFileSystemWorkspace::OnBuildStarting, this);
        EventNotifier::Get()->Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clFileSystemWorkspace::OnIsBuildInProgress, this);
        EventNotifier::Get()->Unbind(wxEVT_STOP_BUILD, &clFileSystemWorkspace::OnStopBuild, this);
        EventNotifier::Get()->Unbind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING,
                                     &clFileSystemWorkspace::OnCustomTargetMenu, this);

        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clFileSystemWorkspace::OnBuildProcessTerminated, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clFileSystemWorkspace::OnBuildProcessOutput, this);
        Unbind(wxEVT_TERMINAL_EXIT, &clFileSystemWorkspace::OnExecProcessTerminated, this);

        // Exec events
        EventNotifier::Get()->Unbind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clFileSystemWorkspace::OnExecute, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_IS_PROGRAM_RUNNING, &clFileSystemWorkspace::OnIsProgramRunning, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clFileSystemWorkspace::OnStopExecute, this);

        // Debug events
        EventNotifier::Get()->Unbind(wxEVT_QUICK_DEBUG_DLG_SHOWING, &clFileSystemWorkspace::OnQuickDebugDlgShowing,
                                     this);
        EventNotifier::Get()->Unbind(wxEVT_QUICK_DEBUG_DLG_DISMISSED_OK,
                                     &clFileSystemWorkspace::OnQuickDebugDlgDismissed, this);
        EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clFileSystemWorkspace::OnFileSaved, this);
        EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &clFileSystemWorkspace::OnDebug, this);

        EventNotifier::Get()->Unbind(wxEVT_FILE_CREATED, &clFileSystemWorkspace::OnFileSystemUpdated, this);
    }
}

wxString clFileSystemWorkspace::GetActiveProjectName() const { return ""; }

wxFileName clFileSystemWorkspace::GetFileName() const { return m_filename; }

wxString clFileSystemWorkspace::GetFilesMask() const
{
    clFileSystemWorkspaceConfig::Ptr_t conf = m_settings.GetSelectedConfig();
    if(conf) {
        return conf->GetFileExtensions();
    }
    return wxEmptyString;
}

wxFileName clFileSystemWorkspace::GetProjectFileName(const wxString& projectName) const
{
    wxUnusedVar(projectName);
    return wxFileName();
}

void clFileSystemWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const {}

wxString clFileSystemWorkspace::GetProjectFromFile(const wxFileName& filename) const { return ""; }

void clFileSystemWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    files.clear();
    files.Alloc(m_files.GetSize());
    for(const wxFileName& file : m_files) {
        files.Add(file.GetFullPath());
    }
}

wxArrayString clFileSystemWorkspace::GetWorkspaceProjects() const { return {}; }

bool clFileSystemWorkspace::IsBuildSupported() const { return true; }

bool clFileSystemWorkspace::IsProjectSupported() const { return false; }

void clFileSystemWorkspace::CacheFiles(bool force)
{
    // should we force a rescan of the files?
    if(force) {
        m_files.Clear();
    }
    // sanity
    if(!m_files.IsEmpty()) {
        m_files.Clear();
    }
    std::thread thr(
        [=](const wxString& rootFolder) {
            clFilesScanner fs;
            std::vector<wxString> files;
            wxStringSet_t excludeFolders = { ".git", ".svn", ".codelite", ".ctagsd" };

            wxString excludePaths = GetExcludeFolders();
            wxArrayString paths = StringUtils::BuildArgv(excludePaths);
            if(!paths.IsEmpty()) {
                for(wxString& excludePath : paths) {
                    excludePath.Trim().Trim(false);
                    if(excludePath.EndsWith("/") || excludePath.EndsWith("\\")) {
                        excludePath.RemoveLast();
                    }
                    if(excludePath.IsEmpty()) {
                        continue;
                    }

                    wxFileName fnpath(excludePath, "");
                    excludeFolders.insert(fnpath.GetPath());
                }
            }
            fs.Scan(rootFolder, files, GetFilesMask(), "", excludeFolders);
            clFileSystemEvent event(wxEVT_FS_SCAN_COMPLETED);
            wxArrayString arrfiles;
            arrfiles.Alloc(files.size());
            for(const wxFileName& f : files) {
                arrfiles.Add(f.GetFullPath());
            }
            event.SetPaths(arrfiles);
            EventNotifier::Get()->QueueEvent(event.Clone());
        },
        GetFileName().GetPath());
    thr.detach();
}

void clFileSystemWorkspace::OnBuildStarting(clBuildEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        // before we start the build, save all modified files
        clGetManager()->SaveAll(false);
        DoBuild(event.GetKind());
    }
}

void clFileSystemWorkspace::OnBuildEnded(clBuildEvent& event) { event.Skip(); }

void clFileSystemWorkspace::OnOpenWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(OpenWorkspace(event.GetFileName())) {
        event.Skip(false);
    } else {
        m_filename.Clear();
    }
}

void clFileSystemWorkspace::OnCloseWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(CloseWorkspace()) {
        event.Skip(false);
    }
}

bool clFileSystemWorkspace::Load(const wxFileName& file)
{
    if(m_isLoaded) {
        return true;
    }
    m_filename = file;
    bool loadOk = m_settings.Load(m_filename);
    if(loadOk && m_settings.GetName().empty()) {
        m_settings.SetName(m_filename.GetName());
    }
    return loadOk;
}

void clFileSystemWorkspace::Save(bool parse)
{
    if(!m_filename.IsOk()) {
        return;
    }
    m_settings.Save(m_filename);

    // Fire 'saved' event
    clCommandEvent eventFileSave(wxEVT_FILE_SAVED);
    eventFileSave.SetFileName(m_filename.GetFullPath());
    eventFileSave.SetString(m_filename.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventFileSave);

    GetView()->SetExcludeFilePatterns(
        GetSettings().GetSelectedConfig() ? GetSettings().GetSelectedConfig()->GetExcludeFilesPattern() : "");
    GetView()->UpdateConfigs(GetSettings().GetConfigs(), GetConfig() ? GetConfig()->GetName() : wxString());
    // trigger a file scan
    if(parse) {
        CacheFiles();
    }
}

void clFileSystemWorkspace::RestoreSession()
{
    if(IsOpen()) {
        clGetManager()->LoadWorkspaceSession(m_filename);
    }
}

void clFileSystemWorkspace::DoOpen()
{
    // Close any opened workspace
    auto frame = EventNotifier::Get()->TopFrame();
    wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventCloseWsp.SetEventObject(frame);
    frame->GetEventHandler()->ProcessEvent(eventCloseWsp);

    // set the working directory to the workspace view
    ::wxSetWorkingDirectory(GetFileName().GetPath());

    // Create the symbols db file
    wxFileName fnFolder(GetFileName());
    fnFolder.SetExt("db");
    fnFolder.AppendDir(".codelite");
    fnFolder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Load the backticks cache file, this needs to be done early as we can
    // since it is used
    if(m_backtickCache) {
        m_backtickCache.reset(nullptr);
    }

    // load the new cache
    m_backtickCache.reset(new clBacktickCache(GetFileName().GetPath()));

    // Init the view
    GetView()->Clear();

    // Update the view exclude patterns
    auto selectedConf = GetSettings().GetSelectedConfig();
    if(selectedConf) {
        GetView()->SetExcludeFilePatterns(selectedConf->GetExcludeFilesPattern());
    }

    // And now load the main folder
    GetView()->AddFolder(GetFileName().GetPath());

    // Notify CodeLite that this workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // Notify that the a new workspace is loaded
    clWorkspaceEvent event(wxEVT_WORKSPACE_LOADED);
    event.SetString(GetFileName().GetFullPath());
    event.SetFileName(GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(event);

    // Update the build configurations button
    GetView()->UpdateConfigs(GetSettings().GetConfigs(), GetConfig() ? GetConfig()->GetName() : wxString());

    // and finally, request codelite to keep this workspace in the recently opened workspace list
    clGetManager()->AddWorkspaceToRecentlyUsedList(m_filename);

    // Cache the source files from the workspace directories
    CacheFiles();

    // mark the workspace as loaded before restoring the session
    m_isLoaded = true;

    // Load the workspace session (if any)
    CallAfter(&clFileSystemWorkspace::RestoreSession);
}

void clFileSystemWorkspace::DoClose()
{
    if(!m_isLoaded)
        return;

    // Store the session
    clGetManager()->StoreWorkspaceSession(m_filename);

    // avoid any file re-cache, we are closing
    Save(false);
    DoClear();

    // Clear the UI
    GetView()->Clear();

    // Notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

    // Notify workspace closed event
    clWorkspaceEvent event_closed(wxEVT_WORKSPACE_CLOSED);
    EventNotifier::Get()->ProcessEvent(event_closed);

    // Free the database
    TagsManagerST::Get()->CloseDatabase();

    m_isLoaded = false;
    m_showWelcomePage = true;

    if(m_backtickCache) {
        m_backtickCache->Save();
        m_backtickCache.reset(nullptr);
    }

    wxDELETE(m_buildProcess);
    GetView()->UpdateConfigs({}, wxString());
}

void clFileSystemWorkspace::DoClear()
{
    m_filename.Clear();
    m_settings.Clear();
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

void clFileSystemWorkspace::New(const wxString& folder, const wxString& name) { DoCreate(name, folder, true); }

void clFileSystemWorkspace::OnScanCompleted(clFileSystemEvent& event)
{
    clDEBUG() << "FSW: CacheFiles completed. Found" << event.GetPaths().size() << "files";
    m_files.Clear();
    m_files.Alloc(event.GetPaths().size());
    for(const wxString& filename : event.GetPaths()) {
        m_files.Add(filename);
    }
    clGetManager()->SetStatusMessage(_("File system scan completed"));

    // Trigger a non full reparse
    Parse(false);
}

void clFileSystemWorkspace::OnParseWorkspace(wxCommandEvent& event)
{
    if(!m_isLoaded) {
        event.Skip();
        return;
    }
    Parse(event.GetInt() == (event.GetEventType() == wxEVT_CMD_RETAG_WORKSPACE));
}

void clFileSystemWorkspace::Parse(bool fullParse)
{
    if(m_files.IsEmpty()) {
        return;
    }

    if(fullParse) {
        TagsManagerST::Get()->ParseWorkspaceFull(GetFileName().GetPath());
    } else {
        TagsManagerST::Get()->ParseWorkspaceIncremental();
    }
}

void clFileSystemWorkspace::Close() { DoClose(); }

wxString clFileSystemWorkspace::CompileFlagsAsString(const wxArrayString& arr) const
{
    wxString s;
    for(const wxString& l : arr) {
        if(!l.IsEmpty()) {
            s << l << "\n";
        }
    }
    return s.Trim();
}

wxString clFileSystemWorkspace::GetTargetCommand(const wxString& target) const
{
    if(!GetConfig()) {
        return wxEmptyString;
    }
    const auto& M = m_settings.GetSelectedConfig()->GetBuildTargets();
    if(M.count(target)) {
        wxString cmd = M.find(target)->second;
        return cmd;
    }
    return wxEmptyString;
}

void clFileSystemWorkspace::OnBuildProcessTerminated(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        wxDELETE(m_buildProcess);
        DoPrintBuildMessage(event.GetOutput());

        clBuildEvent e(wxEVT_BUILD_PROCESS_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);

        // Notify about build process ended to plugins
        clBuildEvent eventStopped(wxEVT_BUILD_ENDED);
        EventNotifier::Get()->AddPendingEvent(eventStopped);

        // remove any temporary scripts generated by our helper
        m_shell_helper.Cleanup();
    }
}

void clFileSystemWorkspace::OnBuildProcessOutput(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        DoPrintBuildMessage(event.GetOutput());
    }
}

void clFileSystemWorkspace::DoPrintBuildMessage(const wxString& message)
{
    clBuildEvent e(wxEVT_BUILD_PROCESS_ADDLINE);
    e.SetString(message);
    EventNotifier::Get()->AddPendingEvent(e);
}

void clFileSystemWorkspace::OnSaveSession(clCommandEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        clGetManager()->StoreWorkspaceSession(m_filename);
    }
}

void clFileSystemWorkspace::Initialise()
{
    if(m_initialized) {
        return;
    }
    m_view = new clFileSystemWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook(), GetWorkspaceType());
    clGetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());
    auto accel_manager = clKeyboardManager::Get();

    // register global accelerators entries
    accel_manager->AddAccelerator(_("File System Workspace"), { { "fsw_refresh_current_folder", _("Refresh") } });
}

void clFileSystemWorkspace::OnExecute(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();

    if(m_execPID != wxNOT_FOUND) {
        return;
    }

    wxString exe, args, wd;
    GetExecutable(exe, args, wd);
    clEnvList_t envList = GetEnvList();
    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    console->SetAutoTerminate(true);
    console->SetCommand(exe, args);
    console->SetWorkingDirectory(wd);
    console->SetWaitWhenDone(true);
    console->SetSink(this);
    console->SetEnvironment(envList);

    if(console->Start()) {
        m_execPID = console->GetPid();
    }

    // Notify about program execution
    clExecuteEvent startEvent(wxEVT_PROGRAM_STARTED);
    EventNotifier::Get()->AddPendingEvent(startEvent);
}

clEnvList_t clFileSystemWorkspace::GetEnvList()
{
    clEnvList_t envList;
    if(!GetConfig()) {
        return envList;
    }
    wxString envstr;
    EvnVarList env = EnvironmentConfig::Instance()->GetSettings();
    EnvMap envMap = env.GetVariables(env.GetActiveSet(), false, "", "");

    // Add the global variables
    envstr += envMap.String();
    envstr += "\n";

    // include special environment variable with the default compiler paths
    const wxString& compiler_name = GetConfig()->GetCompiler();
    if(!compiler_name.empty()) {
        auto cmp = BuildSettingsConfigST::Get()->GetCompiler(compiler_name);
        auto paths = cmp->GetDefaultIncludePaths();
        if(!paths.empty()) {
            wxString env_value;
            for(const auto& path : paths) {
                if(!env_value.empty()) {
                    env_value << ";";
                }
                env_value << path;
            }
            envstr += "CXX_INCLUDE_PATHS=" + env_value + "\n";
        }
    }

    // and finally, include the user environment variables
    envstr += GetConfig()->GetEnvironment();

    // Append the workspace environment
    envstr = MacroManager::Instance()->Expand(envstr, nullptr, wxEmptyString);
    envList = FileUtils::CreateEnvironment(envstr);

    return envList;
}

void clFileSystemWorkspace::OnIsBuildInProgress(clBuildEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();
    event.SetIsRunning(m_buildProcess != nullptr || (m_remoteBuilder && m_remoteBuilder->IsRunning()));
}

void clFileSystemWorkspace::OnIsProgramRunning(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();
    event.SetAnswer(m_execPID != wxNOT_FOUND);
}

void clFileSystemWorkspace::OnStopExecute(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    if(m_execPID != wxNOT_FOUND) {
        ::clKill(m_execPID, wxSIGTERM, true);
        m_execPID = wxNOT_FOUND;
    }
}

void clFileSystemWorkspace::OnStopBuild(clBuildEvent& event)
{
    CHECK_EVENT(event);
    if(m_buildProcess) {
        m_buildProcess->Terminate();
    } else if(m_remoteBuilder && m_remoteBuilder->IsRunning()) {
        m_remoteBuilder->Stop();
    }
}

void clFileSystemWorkspace::OnQuickDebugDlgShowing(clDebugEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();

    wxString args = MacroManager::Instance()->Expand(GetConfig()->GetArgs(), NULL, "", "");
    wxString exec = MacroManager::Instance()->Expand(GetConfig()->GetExecutable(), NULL, "", "");
    event.SetArguments(args);
    event.SetExecutableName(exec);
}

void clFileSystemWorkspace::OnQuickDebugDlgDismissed(clDebugEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspace::GetConfig() const
{
    return GetSettings().GetSelectedConfig();
}

void clFileSystemWorkspace::OnMenuCustomTarget(wxCommandEvent& event)
{
    if(m_buildTargetMenuIdToName.count(event.GetId()) == 0) {
        return;
    }
    const wxString& target = m_buildTargetMenuIdToName.find(event.GetId())->second;
    if(GetConfig()->GetBuildTargets().count(target) == 0) {
        return;
    }
    DoBuild(target);
    m_buildTargetMenuIdToName.clear();
}

void clFileSystemWorkspace::OnCustomTargetMenu(clContextMenuEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();
    wxMenu* menu = event.GetMenu();
    wxArrayString arrTargets;
    const auto& targets = GetConfig()->GetBuildTargets();
    // Copy the targets to std::map to get a sorted results
    std::map<wxString, wxString> M;
    M.insert(targets.begin(), targets.end());
    m_buildTargetMenuIdToName.clear();

    for(const auto& vt : M) {
        const wxString& name = vt.first;
        int menuId = wxXmlResource::GetXRCID(vt.first);
        menu->Append(menuId, name, name, wxITEM_NORMAL);
        menu->Bind(wxEVT_MENU, &clFileSystemWorkspace::OnMenuCustomTarget, this, menuId);
        m_buildTargetMenuIdToName.insert({ menuId, name });
    }
}

void clFileSystemWorkspace::DoBuild(const wxString& target)
{
    if(!GetConfig()) {
        ::wxMessageBox(_("You should have at least one workspace configuration.\n0 found\nOpen the project "
                         "settings and add one"),
                       "CodeLite", wxICON_WARNING | wxCENTER);
        return;
    }

    wxString cmd = GetTargetCommand(target);
    if(cmd.IsEmpty()) {
        ::wxMessageBox(_("Don't know how to run '") + target + "'", "CodeLite", wxICON_WARNING | wxCENTER);
        return;
    }

    if(m_buildProcess) {
        return;
    }

    m_shell_helper.Cleanup(); // clear any scripts generated earlier
    size_t flags = IProcessCreateDefault | IProcessCreateWithHiddenConsole | IProcessRawOutput;
    // Check that the remote development is enabled AND remote build
    if(GetConfig()->IsRemoteEnabled() && GetConfig()->IsRemoteBuild()) {
        flags |= IProcessCreateSSH;
    } else {
        flags |= IProcessWrapInShell;
    }

    // Replace all workspace macros from the command
    cmd = MacroManager::Instance()->Expand(cmd, nullptr, wxEmptyString);

    // Build the environment to use
    clEnvList_t envList = GetEnvList();

    // Start the process with the environemt
    wxString ssh_account;
    wxString wd = GetFileName().GetPath();

    // make changes if SSH is enabled here
    if(flags & IProcessCreateSSH) {
        ssh_account = GetConfig()->GetRemoteAccount();
        wd = GetConfig()->GetRemoteFolder();
    }

    // See if the command requires modifications
    if(!(flags & IProcessCreateSSH) && m_shell_helper.ProcessCommand(cmd)) {
        cmd = m_shell_helper.GetCommand();
        flags |= m_shell_helper.GetProcessCreateFlags();
    }

    m_buildProcess = ::CreateAsyncProcess(this, cmd, flags, wd, &envList, ssh_account);
    if(!m_buildProcess) {
        clBuildEvent e(wxEVT_BUILD_PROCESS_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);

    } else {

        // notify about starting build process.
        // we pass the selected compiler in the event
        clBuildEvent e(wxEVT_BUILD_PROCESS_STARTED);
        e.SetToolchain(GetConfig()->GetCompiler());
        EventNotifier::Get()->AddPendingEvent(e);

        // Notify about build process started
        clBuildEvent eventStart(wxEVT_BUILD_STARTED);
        eventStart.SetConfigurationName(GetConfig()->GetName());
        eventStart.SetProjectName(wxEmptyString);
        EventNotifier::Get()->AddPendingEvent(eventStart);
    }
}

void clFileSystemWorkspace::OnNewWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(event.GetString() == GetWorkspaceType()) {
        event.Skip(false);
        // Prompt the user for folder and name
        NewFileSystemWorkspaceDialog dlg(EventNotifier::Get()->TopFrame());
        if(dlg.ShowModal() == wxID_OK) {
            DoCreate(dlg.GetWorkspaceName(), dlg.GetWorkspacePath(), false);
        }
    }
}

void clFileSystemWorkspace::DoCreate(const wxString& name, const wxString& path, bool loadIfExists)
{
    wxFileName fn(path, "");
    if(fn.GetDirCount() == 0) {
        ::wxMessageBox(_("Unable to create a workspace on the root folder"), "CodeLite", wxICON_ERROR | wxCENTER);
        return;
    }

    if(loadIfExists) {
        // Check to see if any workspace already exists in this folder
        clFilesScanner fs;
        clFilesScanner::EntryData::Vec_t results;
        fs.ScanNoRecurse(path, results, "*.workspace");
        for(const auto& f : results) {
            if(clFileSystemWorkspaceSettings::IsOk(f.fullpath)) {
                fn = f.fullpath;
                break;
            }
        }
    }

    // If an workspace is opened and it is the same one as this, dont do nothing
    if(m_isLoaded && (GetFileName() == fn)) {
        return;
    }

    // Call close here, it does nothing if a workspace is not opened
    DoClose();
    DoClear();

    if(!name.IsEmpty()) {
        fn.SetName(name);
    } else if(fn.GetFullName().IsEmpty()) {
        wxString name = ::clGetTextFromUser(_("Workspace Name"), _("Name"), fn.GetDirs().Last());
        if(name.IsEmpty()) {
            return;
        }
        fn.SetName(name);
    }

    fn.SetExt("workspace");
    SetName(fn.GetName());

    // Creates an empty workspace file
    m_filename = fn;
    if(!fn.FileExists()) {
        Save(false);

        // let the plugins know that we just created a new workspace file and we
        // are about to load it
        clFileSystemEvent createEvent(wxEVT_FS_NEW_WORKSPACE_FILE_CREATED);
        createEvent.SetPath(m_filename.GetFullPath());
        EventNotifier::Get()->ProcessEvent(createEvent);
    }

    // and load it
    if(Load(m_filename)) {
        // Now load it
        DoOpen();
    } else {
        m_filename.Clear();
    }
}

void clFileSystemWorkspace::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    CHECK_ACTIVE_CONFIG();

    if(GetConfig()->IsRemoteEnabled()) {
        const wxString& filename = event.GetFileName();

        // There are 2 cases where we don't want to trigger remote save:
        // 1. if the file was opened by the sftp plugin, don't attempt to save it remotely
        // it will be done by the sftp plugin. These files are marked with client data
        // set with the "sftp" key
        // 2. if the file is not located under our root folder -> don't attempt to save it
        bool managedBySftp = false;
        IEditor* editor = clGetManager()->FindEditor(filename);
        if(editor && editor->GetClientData("sftp")) {
            managedBySftp = true;
        }

        wxString rootPath = GetFileName().GetPath();
        wxString filePath = wxFileName(filename).GetPath();
        bool doRemoteSave = filePath.StartsWith(rootPath) && !managedBySftp;

        if(doRemoteSave) {
            wxString remoteFilePath;
            const wxString& account = GetConfig()->GetRemoteAccount();
            const wxString& remotePath = GetConfig()->GetRemoteFolder();

            // Make the local file path relative to the workspace location
            wxFileName fnLocalFile(event.GetFileName());
            fnLocalFile.MakeRelativeTo(GetFileName().GetPath());

            remoteFilePath = fnLocalFile.GetFullPath(wxPATH_UNIX);
            remoteFilePath.Prepend(remotePath + "/");
            wxFileName fnRemoteFile(remoteFilePath);

            // Build the remote filename
            clSFTPEvent eventSave(wxEVT_SFTP_SAVE_FILE);
            eventSave.SetAccount(account);
            eventSave.SetLocalFile(filename);
            eventSave.SetRemoteFile(fnRemoteFile.GetFullPath(wxPATH_UNIX));
            EventNotifier::Get()->QueueEvent(eventSave.Clone());
        }
    }
}

void clFileSystemWorkspace::OnSourceControlPulled(clSourceControlEvent& event)
{
    event.Skip();
    clDEBUG() << "Source control '" << event.GetSourceControlName() << "' pulled.";
    clDEBUG() << "Refreshing tree + re-parsing";
    GetView()->RefreshTree();

    // Re-Cache the files and trigger a workspace parse
    CacheFiles(true);
}

void clFileSystemWorkspace::TriggerQuickParse()
{
    wxCommandEvent eventParse(wxEVT_MENU, XRCID("retag_workspace"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->QueueEvent(eventParse.Clone());
}

void clFileSystemWorkspace::FileSystemUpdated() { CacheFiles(true); }

void clFileSystemWorkspace::OnDebug(clDebugEvent& event)
{
    if(!IsOpen()) {
        event.Skip();
        return;
    }

    // This is ours to handle. Stop processing it here
    event.Skip(false);

    DebuggerMgr::Get().SetActiveDebugger(GetConfig()->GetDebugger());
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!dbgr) {
        return;
    }
    // if already running, skip this
    // the default behaviour is to "continue"
    if(dbgr->IsRunning()) {
        event.Skip();
        return;
    }

    // Update the debugger information
    DebuggerInformation dinfo = dbgr->GetDebuggerInformation();
    dinfo.breakAtWinMain = false;
    dinfo.consoleCommand = EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand();
    dbgr->SetDebuggerInformation(dinfo);
    dbgr->SetIsRemoteDebugging(false);

    // Setup the debug session

    wxString exe, args, wd;
    GetExecutable(exe, args, wd);

    const wxString& user_debugger = GetConfig()->GetDebuggerPath();

    // Start the debugger
    DebugSessionInfo session_info;
    clDebuggerBreakpoint::Vec_t bpList;
    session_info.exeName = exe;
    session_info.cwd = wd;
    session_info.init_file_content = GetConfig()->GetDebuggerCommands();
    clGetManager()->GetBreakpoints(bpList);
    session_info.bpList = bpList;

    // Start terminal (doesn't do anything under MSW)
    m_debuggerTerminal.Clear();
    m_debuggerTerminal.Launch(dbgr->GetName());
    session_info.ttyName = m_debuggerTerminal.GetTty();
    session_info.enablePrettyPrinting = dinfo.enableGDBPrettyPrinting;

    // Fire "starting" event
    clDebugEvent eventStarting(wxEVT_DEBUG_STARTING);
    eventStarting.SetClientData(&session_info);
    if(EventNotifier::Get()->ProcessEvent(eventStarting))
        return;

    // Check if any plugins provided us with new breakpoints
    if(!eventStarting.GetBreakpoints().empty()) {
        session_info.bpList.swap(eventStarting.GetBreakpoints());
    }

    // Set the gdb executable
    if(!user_debugger.empty()) {
        session_info.debuggerPath = user_debugger;
    } else {
        CompilerPtr cmp = GetCompiler();
        if(cmp && !cmp->GetTool("Debugger").empty()) {
            session_info.debuggerPath = cmp->GetTool("Debugger");
        }
    }

    // convert the envlist into map
    auto envlist = FileUtils::CreateEnvironment(GetConfig()->GetEnvironment());
    //    wxStringMap_t envmap;
    //    envmap.reserve(envlist.size());
    //    envmap.insert(envlist.begin(), envlist.end());
    dbgr->Start(session_info, &envlist);

    // Notify that debug session started
    // this will ensure that the debug layout is loaded
    clDebugEvent eventStarted(wxEVT_DEBUG_STARTED);
    eventStarted.SetClientData(&session_info);
    EventNotifier::Get()->ProcessEvent(eventStarted);
    // Now run the debuggee
    dbgr->Run(args, "");
}

void clFileSystemWorkspace::GetExecutable(wxString& exe, wxString& args, wxString& wd)
{
    exe = GetConfig()->GetExecutable();
    args = GetConfig()->GetArgs();
    wd = GetConfig()->GetWorkingDirectory().IsEmpty() ? GetFileName().GetPath() : GetConfig()->GetWorkingDirectory();

    // build the arguments
    args.Replace("\r", wxEmptyString);
    args.Replace("\n", " ");
    auto args_arr = StringUtils::BuildArgv(args);

    args.clear();
    for(auto& arg : args_arr) {
        if(!args.empty()) {
            args << " ";
        }
        arg.Trim().Trim(false);
        // wrap with quotes if required
        ::WrapWithQuotes(arg);
        args << arg;
    }

    exe = MacroManager::Instance()->Expand(exe, nullptr, wxEmptyString);
    args = MacroManager::Instance()->Expand(args, nullptr, wxEmptyString);
    wd = MacroManager::Instance()->Expand(wd, nullptr, wxEmptyString);
}

CompilerPtr clFileSystemWorkspace::GetCompiler()
{
    return BuildSettingsConfigST::Get()->GetCompiler(GetConfig()->GetCompiler());
}

void clFileSystemWorkspace::OnExecProcessTerminated(clProcessEvent& event)
{
    event.Skip();
    m_execPID = wxNOT_FOUND;

    // Notify about program stopped execution
    clExecuteEvent startEvent(wxEVT_PROGRAM_TERMINATED);
    EventNotifier::Get()->AddPendingEvent(startEvent);
}

void clFileSystemWorkspace::OnFileSystemUpdated(clFileSystemEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        const wxArrayString& paths = event.GetPaths();
        if(paths.empty()) {
            return;
        }

        for(const wxString& path : paths) {
            m_files.Add(path);
        }

        // Parse the newly added files
        Parse(false);
    }
}

void clFileSystemWorkspace::CreateCompileFlagsFile()
{
    wxBusyCursor bc;
    const wxFileName& filename = clFileSystemWorkspace::Get().GetFileName();
    auto backticks_cache = clFileSystemWorkspace::Get().GetBackticksCache();

    auto selectedConfig = m_settings.GetSelectedConfig();
    // Create a compile flags from the following info:
    // - Get list of folders from the current workspace
    // - Any command that is displayed in the text box, expand it
    // - Selected compiler paths
    auto compilerOptions = selectedConfig->GetCompilerOptions(backticks_cache);
    auto includes = selectedConfig->ExpandUserCompletionFlags(filename.GetPath(), backticks_cache, true);

    wxString compile_flags_txt;
    // Include the workspace path by default
    wxString workspacePath = filename.GetPath();
    ::WrapWithQuotes(workspacePath);

    compile_flags_txt << "-I" << workspacePath << "\n";
    for(const auto& s : includes) {
        compile_flags_txt << s << "\n";
    }

    for(const auto& s : compilerOptions) {
        compile_flags_txt << s << "\n";
    }

    if(!compile_flags_txt.empty()) {
        // replace any placeholders
        compile_flags_txt = MacroManager::Instance()->Expand(compile_flags_txt, nullptr, "", "");
        wxFileName fnCompileFlags(filename);
        fnCompileFlags.SetFullName("compile_flags.txt");
        FileUtils::WriteFileContent(fnCompileFlags, compile_flags_txt, wxConvUTF8);

        // Fire JSON Generated event
        clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
        EventNotifier::Get()->QueueEvent(eventCompileCommandsGenerated.Clone());
        clDEBUG() << "File:" << fnCompileFlags << "generated" << clEndl;

        wxString msg;
        msg << _("Successfully generated file:\n") << fnCompileFlags.GetFullPath();
        ::wxMessageBox(msg, "CodeLite");
    }
}

wxString clFileSystemWorkspace::GetExcludeFolders() const
{
    clFileSystemWorkspaceConfig::Ptr_t conf = m_settings.GetSelectedConfig();
    if(conf) {
        return conf->GetExecludePaths();
    }
    return wxEmptyString;
}

void clFileSystemWorkspace::SetProjectActive(const wxString& project)
{
    // no projects in a file system workspace
    wxUnusedVar(project);
}

bool clFileSystemWorkspace::OpenWorkspace(const wxString& filepath)
{
    // Test that this is our workspace
    if(m_settings.IsOk(filepath) && Load(filepath)) {
        DoOpen();
        return true;
    } else {
        return false;
    }
}

bool clFileSystemWorkspace::CloseWorkspace()
{
    if(!IsOpen()) {
        return false;
    }
    DoClose();
    return true;
}

void clFileSystemWorkspace::OnReloadWorkspace(clCommandEvent& event)
{
    if(!IsOpen()) {
        event.Skip();
        return;
    }

    event.Skip(false);
    wxString current_file = m_filename.GetFullPath();
    if(CloseWorkspace()) {
        OpenWorkspace(current_file);
    }
}
