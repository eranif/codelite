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
#include <thread>
#include "clFileSystemEvent.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "parse_thread.h"
#include "compiler_command_line_parser.h"
#include <wxStringHash.h>
#include <wx/tokenzr.h>
#include "shell_command.h"
#include "processreaderthread.h"
#include "clFilesCollector.h"
#include <wx/msgdlg.h>
#include "environmentconfig.h"
#include "macromanager.h"
#include "fileutils.h"
#include <wx/xrc/xmlres.h>
#include "NewFileSystemWorkspaceDialog.h"
#include "asyncprocess.h"
#include "CompileCommandsGenerator.h"
#include "clSFTPEvent.h"

#define CHECK_ACTIVE_CONFIG() \
    if(!GetSettings().GetSelectedConfig()) { return; }

#define CHECK_EVENT(e)     \
    {                      \
        if(!IsOpen()) {    \
            e.Skip();      \
            return;        \
        }                  \
        event.Skip(false); \
    }

wxDEFINE_EVENT(wxEVT_FS_SCAN_COMPLETED, clFileSystemEvent);
clFileSystemWorkspace::clFileSystemWorkspace(bool dummy)
    : m_dummy(dummy)
{
    SetWorkspaceType("File System Workspace");
    if(!dummy) {
        EventNotifier::Get()->Bind(wxEVT_CMD_CLOSE_WORKSPACE, &clFileSystemWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_OPEN_WORKSPACE, &clFileSystemWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clFileSystemWorkspace::OnNewWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clFileSystemWorkspace::OnAllEditorsClosed, this);
        EventNotifier::Get()->Bind(wxEVT_FS_SCAN_COMPLETED, &clFileSystemWorkspace::OnScanCompleted, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_RETAG_WORKSPACE, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_RETAG_WORKSPACE_FULL, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Bind(wxEVT_SAVE_SESSION_NEEDED, &clFileSystemWorkspace::OnSaveSession, this);
        Bind(wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE, &clFileSystemWorkspace::OnParseThreadScanIncludeCompleted, this);
        EventNotifier::Get()->Bind(wxEVT_SOURCE_CONTROL_PULLED, &clFileSystemWorkspace::OnSourceControlPulled, this);

        // Build events
        EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &clFileSystemWorkspace::OnBuildStarting, this);
        EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &clFileSystemWorkspace::OnStopBuild, this);
        EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clFileSystemWorkspace::OnIsBuildInProgress, this);
        EventNotifier::Get()->Bind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING, &clFileSystemWorkspace::OnCustomTargetMenu,
                                   this);

        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clFileSystemWorkspace::OnBuildProcessTerminated, this);
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clFileSystemWorkspace::OnBuildProcessOutput, this);
        // Exec events
        EventNotifier::Get()->Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clFileSystemWorkspace::OnExecute, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_IS_PROGRAM_RUNNING, &clFileSystemWorkspace::OnIsProgramRunning, this);
        EventNotifier::Get()->Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clFileSystemWorkspace::OnStopExecute, this);
        // Debug events
        EventNotifier::Get()->Bind(wxEVT_QUICK_DEBUG_DLG_SHOWING, &clFileSystemWorkspace::OnQuickDebugDlgShowing, this);
        EventNotifier::Get()->Bind(wxEVT_QUICK_DEBUG_DLG_DISMISSED_OK, &clFileSystemWorkspace::OnQuickDebugDlgDismissed,
                                   this);

        EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clFileSystemWorkspace::OnFileSaved, this);
    }
}

clFileSystemWorkspace::~clFileSystemWorkspace()
{
    if(!m_dummy) {
        EventNotifier::Get()->Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &clFileSystemWorkspace::OnCloseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_OPEN_WORKSPACE, &clFileSystemWorkspace::OnOpenWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clFileSystemWorkspace::OnNewWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clFileSystemWorkspace::OnAllEditorsClosed, this);
        EventNotifier::Get()->Unbind(wxEVT_FS_SCAN_COMPLETED, &clFileSystemWorkspace::OnScanCompleted, this);
        EventNotifier::Get()->Unbind(wxEVT_SAVE_SESSION_NEEDED, &clFileSystemWorkspace::OnSaveSession, this);

        // parsing event
        EventNotifier::Get()->Unbind(wxEVT_CMD_RETAG_WORKSPACE, &clFileSystemWorkspace::OnParseWorkspace, this);
        EventNotifier::Get()->Unbind(wxEVT_CMD_RETAG_WORKSPACE_FULL, &clFileSystemWorkspace::OnParseWorkspace, this);
        Unbind(wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE, &clFileSystemWorkspace::OnParseThreadScanIncludeCompleted, this);
        EventNotifier::Get()->Unbind(wxEVT_SOURCE_CONTROL_PULLED, &clFileSystemWorkspace::OnSourceControlPulled, this);

        // Build events
        EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &clFileSystemWorkspace::OnBuildStarting, this);
        EventNotifier::Get()->Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clFileSystemWorkspace::OnIsBuildInProgress, this);
        EventNotifier::Get()->Unbind(wxEVT_STOP_BUILD, &clFileSystemWorkspace::OnStopBuild, this);
        EventNotifier::Get()->Unbind(wxEVT_BUILD_CUSTOM_TARGETS_MENU_SHOWING,
                                     &clFileSystemWorkspace::OnCustomTargetMenu, this);

        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clFileSystemWorkspace::OnBuildProcessTerminated, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clFileSystemWorkspace::OnBuildProcessOutput, this);

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
    }
}

wxString clFileSystemWorkspace::GetActiveProjectName() const { return ""; }

wxFileName clFileSystemWorkspace::GetFileName() const { return m_filename; }

wxString clFileSystemWorkspace::GetFilesMask() const
{
    clFileSystemWorkspaceConfig::Ptr_t conf = m_settings.GetSelectedConfig();
    if(conf) { return conf->GetFileExtensions(); }
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
    files.Alloc(m_files.size());
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
    if(force) { m_files.clear(); }
    // sanity
    if(!m_files.empty()) { m_files.clear(); }
    std::thread thr(
        [=](const wxString& rootFolder) {
            clFilesScanner fs;
            std::vector<wxString> files;
            wxStringSet_t excludeFolders = { ".git", ".svn", ".codelite" };
            fs.Scan(rootFolder, files, GetFilesMask(), "", excludeFolders);

            clFileSystemEvent event(wxEVT_FS_SCAN_COMPLETED);
            wxArrayString arrfiles;
            arrfiles.Alloc(files.size());
            for(const wxString& f : files) {
                arrfiles.Add(f);
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
        DoBuild(event.GetKind());
    }
}

void clFileSystemWorkspace::OnBuildEnded(clBuildEvent& event) { event.Skip(); }

void clFileSystemWorkspace::OnOpenWorkspace(clCommandEvent& event)
{
    event.Skip();
    wxFileName workspaceFile(event.GetFileName());

    // Test that this is our workspace
    if(m_settings.IsOk(workspaceFile) && Load(workspaceFile)) {
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
    bool loadOk = m_settings.Load(m_filename);
    if(loadOk && m_settings.GetName().empty()) { m_settings.SetName(m_filename.GetName()); }
    return loadOk;
}

void clFileSystemWorkspace::Save(bool parse)
{
    if(!m_filename.IsOk()) { return; }
    m_settings.Save(m_filename);

    clCommandEvent eventFileSave(wxEVT_FILE_SAVED);
    eventFileSave.SetFileName(m_filename.GetFullPath());
    eventFileSave.SetString(m_filename.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventFileSave);

    GetView()->UpdateConfigs(GetSettings().GetConfigs(), GetConfig() ? GetConfig()->GetName() : wxString());
    // trigger a file scan
    if(parse) { CacheFiles(); }
}

void clFileSystemWorkspace::RestoreSession()
{
    if(IsOpen()) { clGetManager()->LoadWorkspaceSession(m_filename); }
}

void clFileSystemWorkspace::DoOpen()
{
    // Create the symbols db file
    wxFileName fnFolder(GetFileName());
    fnFolder.SetExt("db");
    fnFolder.AppendDir(".codelite");
    fnFolder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Init the view
    GetView()->Clear();
    GetView()->AddFolder(GetFileName().GetPath());

    // Notify CodeLite that this workspace is opened
    clGetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    clWorkspaceManager::Get().SetWorkspace(this);

    // Notify that the a new workspace is loaded
    wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
    event.SetString(GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(event);

    // Update the build configurations button
    GetView()->UpdateConfigs(GetSettings().GetConfigs(), GetConfig() ? GetConfig()->GetName() : wxString());

    // and finally, request codelite to keep this workspace in the recently opened workspace list
    clGetManager()->AddWorkspaceToRecentlyUsedList(m_filename);

    TagsManagerST::Get()->CloseDatabase();
    TagsManagerST::Get()->OpenDatabase(fnFolder.GetFullPath());

    // Update the parser paths with the active configuration
    UpdateParserPaths();

    // Cache the source files from the workspace directories
    CacheFiles();

    // Load the workspace session (if any)
    CallAfter(&clFileSystemWorkspace::RestoreSession);
    m_isLoaded = true;
}

void clFileSystemWorkspace::DoClose()
{
    if(!m_isLoaded) return;

    // Store the session
    clGetManager()->StoreWorkspaceSession(m_filename);

    // avoid any file re-cache, we are closing
    Save(false);
    DoClear();

    // Clear the UI
    GetView()->Clear();

    // Restore the parser search paths
    const wxArrayString& paths = TagsManagerST::Get()->GetCtagsOptions().GetParserSearchPaths();
    ParseThreadST::Get()->SetSearchPaths(paths, {});
    clDEBUG() << "Parser paths are now set to:" << paths;

    // Notify workspace closed event
    wxCommandEvent event(wxEVT_WORKSPACE_CLOSED);
    EventNotifier::Get()->ProcessEvent(event);

    // Notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

    // Free the database
    TagsManagerST::Get()->CloseDatabase();

    m_isLoaded = false;
    m_showWelcomePage = true;

    wxDELETE(m_execProcess);
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

void clFileSystemWorkspace::New(const wxString& folder) { DoCreate("", folder, true); }

void clFileSystemWorkspace::OnScanCompleted(clFileSystemEvent& event)
{
    clDEBUG() << "FSW: CacheFiles completed. Found" << event.GetPaths().size() << "files";
    m_files.clear();
    m_files.reserve(event.GetPaths().size());
    for(const wxString& filename : event.GetPaths()) {
        m_files.push_back(filename);
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
    if(m_files.empty()) { return; }

    // in the case of re-tagging the entire workspace and full re-tagging is enabled
    // it is faster to drop the tables instead of deleting
    if(fullParse) { TagsManagerST::Get()->GetDatabase()->RecreateDatabase(); }

    UpdateParserPaths();

    // Create a parsing request
    ParseRequest* parsingRequest = new ParseRequest(EventNotifier::Get()->TopFrame());
    parsingRequest->_workspaceFiles.reserve(m_files.size());
    // use a deep copy to endure thread safety
    for(const wxFileName& fn : m_files) {
        // filter any non valid coding file
        parsingRequest->_workspaceFiles.push_back(fn.GetFullPath().ToAscii().data());
    }

    parsingRequest->setType(ParseRequest::PR_PARSEINCLUDES);
    parsingRequest->setDbFile(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().GetFullPath().c_str());
    parsingRequest->_evtHandler = this;
    parsingRequest->_quickRetag = !fullParse;
    ParseThreadST::Get()->Add(parsingRequest);
    clGetManager()->SetStatusMessage(_("Scanning for files to parse..."));
}

void clFileSystemWorkspace::OnParseThreadScanIncludeCompleted(wxCommandEvent& event)
{
    clGetManager()->SetStatusMessage(_("Parsing..."));

    wxBusyCursor busyCursor;
    std::set<wxString>* fileSet = (std::set<wxString>*)event.GetClientData();

    // add to this set the workspace files to create a unique list of
    // files
    for(const wxFileName& fn : m_files) {
        fileSet->insert(fn.GetFullPath());
    }

    // recreate the list in the form of vector (the API requirs vector)
    std::vector<wxFileName> vFiles;
    vFiles.reserve(fileSet->size());
    vFiles.insert(vFiles.end(), fileSet->begin(), fileSet->end());

    // -----------------------------------------------
    // tag them
    // -----------------------------------------------
    TagsManagerST::Get()->RetagFiles(vFiles, event.GetInt() ? TagsManager::Retag_Quick : TagsManager::Retag_Full);
    wxDELETE(fileSet);
}

void clFileSystemWorkspace::UpdateParserPaths()
{
    if(!GetConfig()) { return; }

    // Apply the environment (incase there are backtick, we spawn a helper process)
    const clEnvList_t envlist = GetEnvList();
    clEnvironment env(&envlist);

    // Update the parser paths
    wxString compile_flags_txt;
    wxArrayString uniquePaths = GetConfig()->GetSearchPaths(GetFileName(), compile_flags_txt);

    // Expand any macros
    for(wxString& path : uniquePaths) {
        path = MacroManager::Instance()->Expand(path, nullptr, "", "");
    }

    if(!compile_flags_txt.empty()) {
        compile_flags_txt = MacroManager::Instance()->Expand(compile_flags_txt, nullptr, "", "");
        wxFileName fnCompileFlags(GetFileName());
        fnCompileFlags.SetFullName("compile_flags.txt");
        FileUtils::WriteFileContent(fnCompileFlags, compile_flags_txt, wxConvUTF8);

        // Fire JSON Generated event
        clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
        EventNotifier::Get()->QueueEvent(eventCompileCommandsGenerated.Clone());
        clDEBUG() << "File:" << fnCompileFlags << "generated";
    }

    ParseThreadST::Get()->SetSearchPaths(uniquePaths, {});
    clDEBUG() << "[" << GetConfig()->GetName() << "]"
              << "Parser paths are now set to:" << uniquePaths;
}

void clFileSystemWorkspace::Close() { DoClose(); }

wxString clFileSystemWorkspace::CompileFlagsAsString(const wxArrayString& arr) const
{
    wxString s;
    for(const wxString& l : arr) {
        if(!l.IsEmpty()) { s << l << "\n"; }
    }
    return s.Trim();
}

wxString clFileSystemWorkspace::GetTargetCommand(const wxString& target) const
{
    if(!GetConfig()) { return wxEmptyString; }
    const wxStringMap_t& M = m_settings.GetSelectedConfig()->GetBuildTargets();
    if(M.count(target)) {
        wxString cmd = M.find(target)->second;
        if(!GetConfig()->IsRemoteEnabled()) { ::WrapInShell(cmd); }
        return cmd;
    }
    return wxEmptyString;
}

void clFileSystemWorkspace::OnBuildProcessTerminated(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        wxDELETE(m_buildProcess);
        DoPrintBuildMessage(event.GetOutput());

        clCommandEvent e(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);

    } else {
        wxDELETE(m_execProcess);
    }
}

void clFileSystemWorkspace::OnBuildProcessOutput(clProcessEvent& event)
{
    if(event.GetProcess() == m_buildProcess) {
        DoPrintBuildMessage(event.GetOutput());

    } else if(event.GetProcess() == m_execProcess) {
        clGetManager()->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
    }
}

void clFileSystemWorkspace::DoPrintBuildMessage(const wxString& message)
{
    clCommandEvent e(wxEVT_SHELL_COMMAND_ADDLINE);
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
    if(m_initialized) { return; }
    m_view = new clFileSystemWorkspaceView(clGetManager()->GetWorkspaceView()->GetBook(), GetWorkspaceType());
    clGetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());
}

void clFileSystemWorkspace::OnExecute(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();

    if(m_execProcess) { return; }

    clFileSystemWorkspaceConfig::Ptr_t conf = GetConfig();
    wxString exe = conf->GetExecutable();
    wxString args = conf->GetArgs();

    // Expand variables
    exe = MacroManager::Instance()->Expand(exe, nullptr, wxEmptyString);
    args = MacroManager::Instance()->Expand(args, nullptr, wxEmptyString);
    ::WrapInShell(exe);

    // Execute the executable
    wxString command;
    command << exe;
    if(!args.empty()) { command << " " << args; }

    clEnvList_t envList = GetEnvList();
    m_execProcess = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessCreateWithHiddenConsole,
                                         GetFileName().GetPath(), &envList);
}

clEnvList_t clFileSystemWorkspace::GetEnvList()
{
    clEnvList_t envList;
    if(!GetConfig()) { return envList; }
    wxString envstr;
    EvnVarList env = EnvironmentConfig::Instance()->GetSettings();
    EnvMap envMap = env.GetVariables(env.GetActiveSet(), false, "", "");

    // Add the global variables
    envstr += envMap.String();
    envstr += "\n";
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
    event.SetAnswer(m_execProcess != nullptr);
}

void clFileSystemWorkspace::OnStopExecute(clExecuteEvent& event)
{
    CHECK_EVENT(event);
    if(m_execProcess) { m_execProcess->Terminate(); }
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
    if(m_buildTargetMenuIdToName.count(event.GetId()) == 0) { return; }
    const wxString& target = m_buildTargetMenuIdToName.find(event.GetId())->second;
    if(GetConfig()->GetBuildTargets().count(target) == 0) { return; }
    DoBuild(target);
    m_buildTargetMenuIdToName.clear();
}

void clFileSystemWorkspace::OnCustomTargetMenu(clContextMenuEvent& event)
{
    CHECK_EVENT(event);
    CHECK_ACTIVE_CONFIG();
    wxMenu* menu = event.GetMenu();
    wxArrayString arrTargets;
    const wxStringMap_t& targets = GetConfig()->GetBuildTargets();
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
        ::wxMessageBox(_("Dont know how to run '") + target + "'", "CodeLite", wxICON_WARNING | wxCENTER);
        return;
    }

    if(GetConfig()->IsRemoteEnabled()) {
        // Launch a remote build process
        if(m_remoteBuilder && m_remoteBuilder->IsRunning()) { return; }
        m_remoteBuilder.reset(new clRemoteBuilder());
        m_remoteBuilder->Build(GetConfig()->GetRemoteAccount(), cmd, GetConfig()->GetRemoteFolder());
    } else {
        if(m_buildProcess) { return; }
        // Replace all workspace macros from the command
        cmd = MacroManager::Instance()->Expand(cmd, nullptr, wxEmptyString);

        // Build the environment to use
        clEnvList_t envList = GetEnvList();

        // Start the process with the environemt
        m_buildProcess = ::CreateAsyncProcess(this, cmd, IProcessCreateDefault, GetFileName().GetPath(), &envList);
        if(!m_buildProcess) {
            clCommandEvent e(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
            EventNotifier::Get()->AddPendingEvent(e);
        } else {
            clCommandEvent e(wxEVT_SHELL_COMMAND_STARTED);
            EventNotifier::Get()->AddPendingEvent(e);
        }
    }
}

void clFileSystemWorkspace::OnNewWorkspace(clCommandEvent& event)
{
    event.Skip();
    if(event.GetString() == GetWorkspaceType()) {
        event.Skip(false);
        // Prompt the user for folder and name
        NewFileSystemWorkspaceDialog dlg(EventNotifier::Get()->TopFrame());
        if(dlg.ShowModal() == wxID_OK) { DoCreate(dlg.GetWorkspaceName(), dlg.GetWorkspacePath(), false); }
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
    if(m_isLoaded && (GetFileName() == fn)) { return; }

    // Call close here, it does nothing if a workspace is not opened
    DoClose();
    DoClear();

    if(!name.IsEmpty()) {
        fn.SetName(name);
    } else if(fn.GetFullName().IsEmpty()) {
        wxString name = ::clGetTextFromUser(_("Workspace Name"), _("Name"), fn.GetDirs().Last());
        if(name.IsEmpty()) { return; }
        fn.SetName(name);
    }

    fn.SetExt("workspace");
    SetName(fn.GetName());

    // Creates an empty workspace file
    m_filename = fn;
    if(!fn.FileExists()) { Save(false); }

    // and load it
    if(Load(m_filename)) {
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
        const wxString& account = GetConfig()->GetRemoteAccount();
        const wxString& remotePath = GetConfig()->GetRemoteFolder();

        wxString remoteFilePath;

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
