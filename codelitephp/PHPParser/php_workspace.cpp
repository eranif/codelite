#include "php_workspace.h"
#include "php_strings.h"
#include <dirtraverser.h>
#include <plugin.h>
#include <globals.h>
#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <event_notifier.h>
#include <macros.h>
#include "php_event.h"
#include <cl_command_event.h>
#include <wx/msgdlg.h>
#include "php_code_completion.h"
#include "php_strings.h"
#include <wx/msgdlg.h>
#include "php_parser_thread.h"
#include <wx/progdlg.h>
#include "cl_command_event.h"
#include "php_strings.h"
#include "php_configuration_data.h"
#include "clWorkspaceManager.h"
#include <algorithm>
#include "file_logger.h"

#ifndef __WXMSW__
#include <errno.h>
#endif

PHPWorkspace* PHPWorkspace::ms_instance = 0;

wxDEFINE_EVENT(wxEVT_PHP_WORKSPACE_FILES_SYNC_START, clCommandEvent);
wxDEFINE_EVENT(wxEVT_PHP_WORKSPACE_FILES_SYNC_END, clCommandEvent);

PHPWorkspace::PHPWorkspace()
    : m_manager(NULL)
    , m_projectSyncOwner(NULL)
{
    SetWorkspaceType(PHPStrings::PHP_WORKSPACE_VIEW_LABEL);
    Bind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPWorkspace::OnProjectSyncEnd, this);
}

PHPWorkspace::~PHPWorkspace()
{
    m_workspaceFile.Clear();
    Unbind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPWorkspace::OnProjectSyncEnd, this);
}

PHPWorkspace* PHPWorkspace::Get()
{
    if(ms_instance == 0) {
        ms_instance = new PHPWorkspace();
    }
    return ms_instance;
}

void PHPWorkspace::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

bool PHPWorkspace::Close(bool saveBeforeClose, bool saveSession)
{
    SendCmdEvent(wxEVT_WORKSPACE_CLOSING);

    if(IsOpen()) {
        if(m_manager && saveSession) {
            m_manager->StoreWorkspaceSession(m_workspaceFile);
        }
        if(saveBeforeClose) {
            // Save it
            Save();
        }
    }

    m_projects.clear();
    m_workspaceFile.Clear();

    // Close the code completion lookup table
    PHPCodeCompletion::Instance()->Close();
    PHPParserThread::Clear();

    PHPEvent phpEvent(wxEVT_PHP_WORKSPACE_CLOSED);
    EventNotifier::Get()->AddPendingEvent(phpEvent);

    // Notify that workspace has been opened to the plugins
    SendCmdEvent(wxEVT_WORKSPACE_CLOSED);
    return true;
}

bool PHPWorkspace::Open(const wxString& filename, wxEvtHandler* view, bool createIfMissing)
{
    // Close the currently opened workspace
    if(IsOpen()) {
        Close(true, true);
    }

    m_workspaceFile = filename;
    wxFileName fnNewWspFile = m_workspaceFile;
    fnNewWspFile.SetExt("workspace");

    if(!fnNewWspFile.Exists()) {
        wxLogNull nolog;
        if(::wxCopyFile(m_workspaceFile.GetFullPath(), fnNewWspFile.GetFullPath())) {
            m_workspaceFile = fnNewWspFile;
        }
    }

    // Ensure that the workspace file is renamed to .workspace
    {
        // Create the private folder if needed
        wxFileName fn(m_workspaceFile);
        fn.AppendDir(".codelite");
        wxLogNull nolog;
        ::wxMkdir(fn.GetPath());
    }

    if(!m_workspaceFile.FileExists()) {
        if(createIfMissing) {
            if(!Create(filename)) {
                return false;
            }
        } else {
            m_workspaceFile.Clear();
            return false;
        }
    }

    // point the tags storage to the correct db file
    wxFileName tagsfile(filename);
    tagsfile.SetExt(wxT("phptags"));

    // set the working directory to the workspace path
    ::wxSetWorkingDirectory(m_workspaceFile.GetPath());

    JSON root(m_workspaceFile);
    FromJSON(root.toElement());

    // We open the symbols database manually here and _not_ via an event
    // since the parser thread might open it first and leave us with a lock
    PHPCodeCompletion::Instance()->Open(m_workspaceFile);

    // Notify internally that the workspace is loaded
    PHPEvent phpEvent(wxEVT_PHP_WORKSPACE_LOADED);
    phpEvent.SetFileName(m_workspaceFile.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(phpEvent);

    // Notify that the a new workspace is loaded
    // This time send the standard codelite event
    // this is important so other plugins such as Svn, Git
    // want to adjust their paths according to the new workspace
    {
        wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
        event.SetString(GetFilename().GetFullPath());
        EventNotifier::Get()->AddPendingEvent(event);
    }

    // wxBusyInfo busy(_("Scanning for workspace files..."), EventNotifier::Get()->TopFrame());
    // wxYieldIfNeeded();
    wxBusyCursor bc;
    SyncWithFileSystemAsync(view);

    // Perform a quick re-parse of the workspace
    ParseWorkspace(false);

    // set this workspace as the active one
    clWorkspaceManager::Get().SetWorkspace(this);

    // and finally, request codelite to keep this workspace in the recently opened workspace list
    clGetManager()->AddWorkspaceToRecentlyUsedList(GetFilename());

    CallAfter(&PHPWorkspace::RestoreWorkspaceSession);
    // Change the workspace extension
    return true;
}

bool PHPWorkspace::IsOpen() const { return m_workspaceFile.IsOk() && m_workspaceFile.FileExists(); }

void PHPWorkspace::CreateProject(const PHPProject::CreateData& createData)
{
    wxString projectName;
    wxFileName fnProjectFileName(createData.path, "");
    projectName << createData.name << ".phprj";
    fnProjectFileName.SetFullName(projectName);

    if(HasProject(projectName)) return;

    // Ensure that the path to the file exists
    fnProjectFileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Create an empty project and initialize it with the global settings
    PHPProject::Ptr_t proj(new PHPProject());
    // Setup the file path + name
    proj->Create(fnProjectFileName, createData.name);
    proj->GetSettings().MergeWithGlobalSettings();
    if(!createData.phpExe.IsEmpty() && wxFileName::Exists(createData.phpExe)) {
        proj->GetSettings().SetPhpExe(createData.phpExe);
    }
    proj->GetSettings().SetRunAs(createData.projectType);
    proj->GetSettings().SetCcIncludePath(createData.ccPaths);
    m_projects.insert(std::make_pair(proj->GetName(), proj));
    if(m_projects.size() == 1) {
        SetProjectActive(proj->GetName());
    }
    Save();
    proj->Save();

    // Retag the workspace (there could be new files that were added to the workspace)
    ParseWorkspace(false);
}

bool PHPWorkspace::IsProjectExists(const wxString& project) { return HasProject(project); }

const PHPProject::Map_t& PHPWorkspace::GetProjects() const { return m_projects; }

void PHPWorkspace::DeleteProject(const wxString& project)
{
    PHPProject::Ptr_t p = GetProject(project);
    CHECK_PTR_RET(p);
    m_projects.erase(project);
    if(p->IsActive() && !m_projects.empty()) {
        // we are removing the active project, select a new project to be the active
        PHPProject::Ptr_t newActiveProject = m_projects.begin()->second;
        newActiveProject->SetIsActive(true);
        newActiveProject->Save();
    }
    Save(); // save the workspace
}

void PHPWorkspace::SetProjectActive(const wxString& project)
{
    PHPProject::Map_t::iterator iter = m_projects.begin();
    PHPProject::Ptr_t activeProject;
    for(; iter != m_projects.end(); ++iter) {
        if(iter->first == project) {
            activeProject = iter->second;
        }
        bool newState = (iter->first == project);
        if(iter->second->IsActive() != newState) {
            iter->second->SetIsActive(newState);
            // Save the change to the file system
            iter->second->Save();
        }
    }

    // notify about this change
    if(activeProject) {
        clProjectSettingsEvent evt(wxEVT_ACTIVE_PROJECT_CHANGED);
        evt.SetProjectName(project);
        evt.SetFileName(activeProject->GetFilename().GetFullPath());
        EventNotifier::Get()->AddPendingEvent(evt);
    }
}

void PHPWorkspace::GetWorkspaceFiles(wxStringSet_t& workspaceFiles, wxProgressDialog* progress) const
{
    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        const wxArrayString& files = iter->second->GetFiles(progress);
        workspaceFiles.insert(files.begin(), files.end());
    }
}

void PHPWorkspace::GetWorkspaceFiles(wxStringSet_t& workspaceFiles, const wxString& filter) const
{
    wxStringSet_t all_files;
    GetWorkspaceFiles(all_files);
    if(filter.IsEmpty()) {
        workspaceFiles.swap(all_files);

    } else {
        wxStringSet_t::iterator iter = all_files.begin();
        for(; iter != all_files.end(); ++iter) {
            wxFileName fn(*iter);
            if(fn.GetName().Contains(filter)) {
                workspaceFiles.insert(*iter);
            }
        }
    }
}

void PHPWorkspace::DoNotifyFilesRemoved(const wxArrayString& files)
{
    if(!files.IsEmpty()) {

        wxBusyInfo info(_("Updating workspace..."));
        wxYieldIfNeeded();
        EventNotifier::Get()->PostFileRemovedEvent(files);
    }
}

wxString PHPWorkspace::GetActiveProjectName() const
{
    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        if(iter->second->IsActive()) {
            return iter->second->GetName();
        }
    }
    return "";
}

void PHPWorkspace::DelFile(const wxString& project, const wxString& filename)
{
    PHPProject::Ptr_t proj = GetProject(project);
    CHECK_PTR_RET(proj);

    wxArrayString files;
    files.Add(filename);
    proj->FilesDeleted(files, true);
}

bool PHPWorkspace::RunProject(bool debugging,
                              const wxString& urlOrFilePath,
                              const wxString& projectName,
                              const wxString& xdebugSessionName)
{
    wxString projectToRun = projectName;
    if(projectToRun.IsEmpty()) {
        projectToRun = GetActiveProjectName();
    }

    PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(projectToRun);
    CHECK_PTR_RET_FALSE(proj);
    // Error is reported inside 'Exec'
    return m_executor.Exec(projectToRun, urlOrFilePath, xdebugSessionName, debugging);
}

bool PHPWorkspace::HasProject(const wxString& projectname) const
{
    if(!IsOpen()) {
        return false;
    }
    return m_projects.count(projectname);
}

void PHPWorkspace::FromJSON(const JSONItem& e)
{
    m_projects.clear();
    if(e.hasNamedObject("projects")) {
        PHPProject::Ptr_t firstProject;
        JSONItem projects = e.namedObject("projects");
        int count = projects.arraySize();
        for(int i = 0; i < count; ++i) {
            PHPProject::Ptr_t p(new PHPProject());
            wxString project_file = projects.arrayItem(i).toString();
            wxFileName fnProject(project_file);
            fnProject.MakeAbsolute(m_workspaceFile.GetPath());
            p->Load(fnProject);
            m_projects.insert(std::make_pair(p->GetName(), p));
            if(!firstProject) {
                firstProject = p;
            }
        }

        PHPProject::Ptr_t activeProject = GetActiveProject();
        if(!activeProject && firstProject) {
            // No active project found, mark the first project as active
            activeProject = firstProject;
            SetProjectActive(firstProject->GetName());
        }

        if(activeProject) {
            // Notify about active project been set
            clProjectSettingsEvent evt(wxEVT_ACTIVE_PROJECT_CHANGED);
            evt.SetProjectName(activeProject->GetName());
            evt.SetFileName(activeProject->GetFilename().GetFullPath());
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    }
}

#define PHP_WORKSPACE_VERSION wxString("1.0")
#define PHP_WORKSPACE_IDE wxString("CodeLite")

JSONItem PHPWorkspace::ToJSON(JSONItem& e) const
{
    JSONItem metadata = JSONItem::createObject("metadata");
    e.append(metadata);

    metadata.addProperty("version", PHP_WORKSPACE_VERSION);
    metadata.addProperty("ide", PHP_WORKSPACE_IDE);
    metadata.addProperty("type", wxString("php"));

    // Store the list of files
    JSONItem projectsArr = JSONItem::createArray("projects");
    e.append(projectsArr);

    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        wxFileName projectFile = iter->second->GetFilename();
        projectFile.MakeRelativeTo(m_workspaceFile.GetPath());
        projectsArr.arrayAppend(projectFile.GetFullPath(wxPATH_UNIX));
    }

    return e;
}

PHPProject::Ptr_t PHPWorkspace::GetActiveProject() const { return GetProject(GetActiveProjectName()); }

PHPProject::Ptr_t PHPWorkspace::GetProject(const wxString& project) const
{
    if(!HasProject(project)) {
        return PHPProject::Ptr_t(NULL);
    }
    return m_projects.find(project)->second;
}

void PHPWorkspace::Save()
{
    if(!IsOpen()) {
        return;
    }
    // serialize the workspace and store it to disk
    JSON root(cJSON_Object);
    JSONItem ele = root.toElement();
    ToJSON(ele);
    root.save(m_workspaceFile);
}

bool PHPWorkspace::Create(const wxString& filename)
{
    {
        wxFileName fn(filename);
        fn.AppendDir(".codelite");
        wxLogNull nolog;
        ::wxMkdir(fn.GetPath());
    }

    wxFileName fn(filename);

    if(fn.FileExists()) {
        return true;
    }

    // create it
    JSON root(cJSON_Object);
    JSONItem ele = root.toElement();
    ToJSON(ele);
    root.save(fn);
    return true;
}

wxString PHPWorkspace::GetPrivateFolder() const
{
    wxFileName fn(GetFilename());
    fn.AppendDir(".codelite");
    return fn.GetPath();
}

void PHPWorkspace::Rename(const wxString& newname)
{
    wxFileName new_path(GetFilename());
    new_path.SetName(newname);

    if(!::wxRenameFile(m_workspaceFile.GetFullPath(), new_path.GetFullPath())) {
        wxString msg;
        msg << _("Failed to rename workspace file:\n'") << m_workspaceFile.GetFullName() << _("'\nto:\n'")
            << new_path.GetFullName() << "'\n" << strerror(errno);
        ::wxMessageBox(msg, "CodeLite", wxOK | wxCENTER | wxICON_ERROR, EventNotifier::Get()->TopFrame());
        return;
    }

    PHPEvent evt(wxEVT_PHP_WORKSPACE_RENAMED);
    evt.SetOldFilename(m_workspaceFile.GetFullPath());
    evt.SetFileName(new_path.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(evt);

    // and now rename the actual file
    m_workspaceFile = new_path;

    // trigger a workspace parsing
    wxCommandEvent evtRetag(wxEVT_CMD_RETAG_WORKSPACE_FULL);
    EventNotifier::Get()->AddPendingEvent(evtRetag);
}

void PHPWorkspace::DoPromptWorkspaceModifiedDialog()
{
    wxMessageDialog dlg(FRAME,
                        _("Workspace file modified externally. Would you like to reload the workspace?"),
                        "CodeLite",
                        wxYES_NO | wxCENTER);
    dlg.SetYesNoLabels(_("Reload Workspace"), _("Ignore"));

    int answer = dlg.ShowModal();
    if(answer == wxID_YES) {
        wxCommandEvent evtReload(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
        FRAME->GetEventHandler()->AddPendingEvent(evtReload);
    }
}

void PHPWorkspace::GetWorkspaceFiles(wxArrayString& workspaceFiles, wxProgressDialog* progress) const
{
    wxStringSet_t files;
    GetWorkspaceFiles(files, progress);
    workspaceFiles.clear();
    wxStringSet_t::const_iterator iter = files.begin();
    for(; iter != files.end(); ++iter) {
        workspaceFiles.Add(*iter);
    }
}

void PHPWorkspace::ParseWorkspace(bool full)
{
    // Request for parsing
    if(full) {
        // a full parsing is needed, stop the paser thread
        // close the database, delete it and recreate it
        // then, restart the parser thread
        PHPParserThread::Clear();
        PHPParserThread::Release(); // Stop and wait the thread terminates

        // Close the CC manager
        PHPCodeCompletion::Instance()->Close();

        // Delete the file
        wxFileName fnDatabaseFile(m_workspaceFile.GetPath(), "phpsymbols.db");
        fnDatabaseFile.AppendDir(".codelite");

        wxLogNull noLog;
        bool bRemoved = clRemoveFile(fnDatabaseFile.GetFullPath());
        wxUnusedVar(bRemoved);

        // Start the managers again
        PHPParserThread::Instance()->Start();
        PHPCodeCompletion::Instance()->Open(m_workspaceFile);
    }

    PHPParserThreadRequest* req = new PHPParserThreadRequest(PHPParserThreadRequest::kParseWorkspaceFilesQuick);
    req->workspaceFile = GetFilename().GetFullPath();
    GetWorkspaceFiles(req->files);

    // Append the current project CC include paths
    PHPProject::Ptr_t pProject = GetActiveProject();
    if(pProject) {
        PHPProjectSettingsData& settings = pProject->GetSettings();
        req->frameworksPaths = settings.GetCCIncludePathAsArray();
    }
    PHPParserThread::Instance()->Add(req);
}

TerminalEmulator* PHPWorkspace::GetTerminalEmulator() { return m_executor.GetTerminalEmulator(); }

void PHPWorkspace::RestoreWorkspaceSession()
{
    if(m_manager && IsOpen()) {
        m_manager->LoadWorkspaceSession(m_workspaceFile);
    }
}

PHPProject::Ptr_t PHPWorkspace::GetProjectForFile(const wxFileName& filename) const
{
    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        if(iter->second->HasFile(filename)) return iter->second;
    }
    return PHPProject::Ptr_t(NULL);
}

bool PHPWorkspace::AddProject(const wxFileName& projectFile, wxString& errmsg)
{
    if(!CanCreateProjectAtPath(projectFile, true)) {
        return false;
    }

    PHPProject::Ptr_t proj(new PHPProject());
    proj->Load(projectFile);

    if(proj->IsOk()) {
        if(HasProject(proj->GetName())) {
            errmsg = _("A project with similar name already exists in the workspace");
            return false;
        }
        // Keep the active project name _before_ we add the new project
        wxString activeProjectName = GetActiveProjectName();

        proj->GetSettings().MergeWithGlobalSettings();
        m_projects.insert(std::make_pair(proj->GetName(), proj));

        if(m_projects.size() == 1) {
            // if we have a single project in the workspace, make it the active
            SetProjectActive(proj->GetName());
        } else {
            // Restore the active project name. This also removes the "Active project" flag (if any)
            // from the newly added project
            SetProjectActive(activeProjectName);
        }

        Save();
        proj->Save();

        // Retag the workspace (there could be new files that were added to the workspace)
        ParseWorkspace(false);
        return true;
    }
    return false;
}

bool PHPWorkspace::CanCreateProjectAtPath(const wxFileName& projectFileName, bool prompt) const
{
    wxString newpath = projectFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
    const PHPProject::Map_t& projects = GetProjects();
    PHPProject::Map_t::const_iterator iter = projects.begin();
    for(; iter != projects.end(); ++iter) {
        if(newpath.StartsWith(iter->second->GetFilename().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME))) {
            // The new path is a sub folder of a project
            if(prompt) {
                wxString message;
                message << _("Unable to create a project at the selected path\n") << _("Path '") << newpath
                        << _("' is already part of project '") << iter->second->GetName() << "'";
                ::wxMessageBox(message, "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
            }
            return false;

        } else if(iter->second->GetFilename().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).StartsWith(newpath)) {
            // The new project is a parent of an existing project
            if(prompt) {
                wxString message;
                message << _("Unable to create a project at the selected path\n") << _("Project '")
                        << iter->second->GetName() << _("' is located under this path");
                ::wxMessageBox(message, "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
            }
            return false;
        }
    }
    return true;
}

bool PHPWorkspace::IsBuildSupported() const { return false; }
bool PHPWorkspace::IsProjectSupported() const { return true; }

wxString PHPWorkspace::GetFilesMask() const
{
    // set the default find in files mask
    PHPConfigurationData conf;
    return conf.Load().GetFindInFilesMask();
}

wxString PHPWorkspace::GetProjectFromFile(const wxFileName& filename) const
{
    PHPProject::Map_t::const_iterator iter =
        std::find_if(m_projects.begin(), m_projects.end(), [&](const PHPProject::Map_t::value_type& v) {
            wxString path = filename.GetPath();
            return path.StartsWith(v.second->GetFilename().GetPath());
        });

    if(iter != m_projects.end()) {
        return iter->second->GetName();
    }
    return wxEmptyString;
}

void PHPWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    PHPProject::Ptr_t p = GetProject(projectName.IsEmpty() ? GetActiveProjectName() : projectName);
    CHECK_PTR_RET(p);

    p->GetFilesArray(files);
}

void PHPWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    std::for_each(m_projects.begin(), m_projects.end(), [&](const PHPProject::Map_t::value_type& v) {
        v.second->GetFilesArray(files);
    });
}

void PHPWorkspace::SyncWithFileSystemAsync(wxEvtHandler* owner)
{
    m_inSyncProjects.clear();
    m_projectSyncOwner = owner;

    if(owner) {
        clCommandEvent event(wxEVT_PHP_WORKSPACE_FILES_SYNC_START);
        owner->AddPendingEvent(event);
    }
    
    if(!m_projects.empty()) {
        PHPProject::Map_t::const_iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            m_inSyncProjects.insert(iter->first);
            iter->second->SyncWithFileSystemAsync(this);
        }
    } else {
        if(owner) {
            // Fire sync-ended event
            clCommandEvent endEvent(wxEVT_PHP_WORKSPACE_FILES_SYNC_END);
            owner->AddPendingEvent(endEvent);
        }
    }
}

void PHPWorkspace::OnProjectSyncEnd(clCommandEvent& event)
{
    const wxString& name = event.GetString();
    if(m_inSyncProjects.count(name) == 0) {
        clWARNING() << "PHPWorkspace::OnProjectSyncEnd: unable to find project '" << name << "' in the workspace..."
                    << clEndl;
        return;
    }

    clDEBUG() << "PHPWorkspace::OnProjectSyncEnd: project" << name << "completed sync" << clEndl;
    m_inSyncProjects.erase(name);

    // Load the project
    PHPProject::Ptr_t pProj = GetProject(name);
    CHECK_PTR_RET(pProj);

    // Update the project files
    pProj->SetFiles(event.GetStrings());

    if(m_inSyncProjects.empty()) {
        clDEBUG() << "PHPWorkspace::OnProjectSyncEnd: all projects completed sync" << clEndl;
        if(m_projectSyncOwner) {
            clCommandEvent endEvent(wxEVT_PHP_WORKSPACE_FILES_SYNC_END);
            m_projectSyncOwner->AddPendingEvent(endEvent);
        }
    }
}
wxFileName PHPWorkspace::GetProjectFileName(const wxString& projectName) const
{
    PHPProject::Ptr_t p = GetProject(projectName);
    if(!p) {
        return wxFileName();
    }
    return p->GetFilename();
}

wxArrayString PHPWorkspace::GetWorkspaceProjects() const
{
    wxArrayString projectArr;
    PHPProject::Map_t projects = GetProjects();
    std::for_each(projects.begin(), projects.end(), [&](PHPProject::Map_t::value_type p) {
        projectArr.Add(p.second->GetName());
    });
    return projectArr;
}

