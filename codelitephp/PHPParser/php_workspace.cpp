#include "php_workspace.h"
#include "php_storage.h"
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

#ifndef __WXMSW__
#include <errno.h>
#endif

PHPWorkspace* PHPWorkspace::ms_instance = 0;

PHPWorkspace::PHPWorkspace() {}

PHPWorkspace::~PHPWorkspace() { m_workspaceFile.Clear(); }

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

bool PHPWorkspace::Close(bool saveBeforeClose)
{
    if(saveBeforeClose && IsOpen()) {
        // Save it
        Save();
    }

    m_projects.clear();
    m_workspaceFile.Clear();

    PHPEvent phpEvent(wxEVT_PHP_WORKSPACE_CLOSED);
    EventNotifier::Get()->AddPendingEvent(phpEvent);

    // Notify that workspace has been opened to the plugins
    SendCmdEvent(wxEVT_WORKSPACE_CLOSED);
    return true;
}

bool PHPWorkspace::Open(const wxString& filename, bool createIfMissing)
{
    // Close the currently opened workspace
    Close();
    m_workspaceFile = filename;

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

    JSONRoot root(m_workspaceFile);
    FromJSON(root.toElement());

    PHPStorage::Instance()->OpenWorksace(tagsfile.GetFullPath());

    PHPEvent phpEvent(wxEVT_PHP_WORKSPACE_LOADED);
    phpEvent.SetFileName(m_workspaceFile.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(phpEvent);

    // Notify that workspace has been opened to the plugins
    if(!PHPCodeCompletion::Instance()->IsUnitTestsMode()) {
        wxCommandEvent evtOpen(wxEVT_WORKSPACE_LOADED);
        evtOpen.SetString(m_workspaceFile.GetFullPath());
        EventNotifier::Get()->ProcessEvent(evtOpen);
    }
    return true;
}

bool PHPWorkspace::IsOpen() const { return m_workspaceFile.IsOk() && m_workspaceFile.FileExists(); }

bool PHPWorkspace::AddFile(const wxString& project, const wxString& folder, const wxString& file)
{
    // locate the project
    PHPProject::Ptr_t p = GetProject(project);
    CHECK_PTR_RET_FALSE(p);

    PHPFolder::Ptr_t pFolder = p->Folder(folder);
    CHECK_PTR_RET_FALSE(pFolder);

    if(pFolder->AddFile(file)) {
        p->Save();

        // Notify about the file added
        PHPEvent evt(wxEVT_PHP_FILES_ADDED);
        wxArrayString files_added;
        files_added.Add(file);
        evt.SetFileList(files_added);
        EventNotifier::Get()->AddPendingEvent(evt);
    }

    return true;
}

void PHPWorkspace::AddFiles(const wxString& project,
                            const wxString& folder,
                            const wxArrayString& files,
                            wxArrayString& filesAdded)
{
    PHPProject::Ptr_t proj = GetProject(project);
    CHECK_PTR_RET(proj);

    PHPFolder::Ptr_t pFolder = proj->Folder(folder);
    CHECK_PTR_RET(pFolder);

    for(size_t i = 0; i < files.GetCount(); ++i) {
        if(pFolder->AddFile(files.Item(i))) {
            filesAdded.Add(files.Item(i));
        }
    }
    proj->Save();

    PHPEvent evt(wxEVT_PHP_FILES_ADDED);
    evt.SetFileList(filesAdded);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void PHPWorkspace::CreateFolder(const wxString& project, const wxString& folder)
{
    PHPProject::Ptr_t proj = GetProject(project);
    CHECK_PTR_RET(proj);

    proj->AddFolder(folder);
    proj->Save();
}

void PHPWorkspace::CreateProject(const wxString& project, bool active)
{
    if(HasProject(project)) return;

    // Create an empty project and initialize it with the global settings
    PHPProject::Ptr_t proj(new PHPProject(project));
    wxFileName projectFile = m_workspaceFile;
    projectFile.SetName(project);
    projectFile.SetExt("phprj");
    proj->Create(projectFile);
    proj->GetSettings().MergeWithGlobalSettings();

    m_projects.insert(std::make_pair(project, proj));
    if(active) {
        SetProjectActive(project);
    }
    Save();
    proj->Save();
}

bool PHPWorkspace::IsFolderExists(const wxString& project, const wxString& folder)
{
    return HasProject(project) && GetProject(project)->Folder(folder);
}

bool PHPWorkspace::IsProjectExists(const wxString& project) { return HasProject(project); }

const PHPProject::Map_t& PHPWorkspace::GetProjects() const { return m_projects; }

void PHPWorkspace::DeleteProject(const wxString& project)
{
    PHPProject::Ptr_t p = GetProject(project);
    CHECK_PTR_RET(p);

    wxArrayString filesRemoved;
    p->GetFiles(filesRemoved);
    m_projects.erase(project);
    Save(); // save the workspace
    DoNotifyFilesRemoved(filesRemoved);
}

void PHPWorkspace::SetProjectActive(const wxString& project)
{
    PHPProject::Map_t::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        iter->second->SetIsActive(iter->first == project);
    }
}

void PHPWorkspace::GetWorkspaceFiles(wxStringSet_t& workspaceFiles) const
{
    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        wxArrayString files;
        iter->second->GetFiles(files);
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

void PHPWorkspace::GetFileProjectFiles(const wxString& filename, wxArrayString& files)
{
    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        wxArrayString projFiles;
        iter->second->GetFiles(projFiles);
        if(projFiles.Index(filename) != wxNOT_FOUND) {
            files.swap(projFiles);
            return;
        }
    }
}

bool PHPWorkspace::DelFile(const wxString& project, const wxString& folder, const wxString& filename)
{
    PHPProject::Ptr_t proj = GetProject(project);
    CHECK_PTR_RET_FALSE(proj);

    PHPFolder::Ptr_t pFolder = proj->Folder(folder);
    CHECK_PTR_RET_FALSE(pFolder);

    bool res = pFolder->RemoveFile(filename);
    if(res) {
        proj->Save();
        wxArrayString filesRemoved;
        filesRemoved.Add(filename);
        DoNotifyFilesRemoved(filesRemoved);
    }
    return res;
}

bool PHPWorkspace::RunProject(bool debugging, const wxString& projectName, const wxString& xdebugSessionName)
{
    wxString projectToRun = projectName;
    if(projectToRun.IsEmpty()) {
        projectToRun = GetActiveProjectName();
    }

    PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(projectToRun);
    CHECK_PTR_RET_FALSE(proj);
    wxFileName indexFile(proj->GetSettings().GetIndexFile());
    if(!indexFile.IsOk() || !indexFile.Exists()) {
        // No index file is set
        wxString message;
        message << _("Failed to execute project:\n") << _("Index file is not set");
        ::wxMessageBox(message, wxT("CodeLite"), wxOK | wxICON_ERROR | wxCENTER, wxTheApp->GetTopWindow());
        return false;
    }
    
    // Error is reported inside 'Exec'
    return m_executor.Exec(projectToRun, xdebugSessionName, debugging);
}

bool PHPWorkspace::HasProject(const wxString& projectname) const
{
    if(!IsOpen()) {
        return false;
    }
    return m_projects.count(projectname);
}

void PHPWorkspace::FromJSON(const JSONElement& e)
{
    m_projects.clear();
    if(e.hasNamedObject("projects")) {
        JSONElement projects = e.namedObject("projects");
        int count = projects.arraySize();
        for(int i = 0; i < count; ++i) {
            PHPProject::Ptr_t p(new PHPProject());
            wxString project_file = projects.arrayItem(i).toString();
            wxFileName fnProject(project_file);
            fnProject.MakeAbsolute(m_workspaceFile.GetPath());
            p->Load(fnProject);
            m_projects.insert(std::make_pair(p->GetName(), p));
        }
    }
}

JSONElement PHPWorkspace::ToJSON(JSONElement& e) const
{
    // Store the list of files
    JSONElement projectsArr = JSONElement::createArray("projects");
    e.append(projectsArr);

    PHPProject::Map_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        wxFileName projectFile = iter->second->GetFilename();
        projectFile.MakeRelativeTo(m_workspaceFile.GetPath());
        projectsArr.arrayAppend(projectFile.GetFullPath());
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
    // seriaize the workspace and store it to disk
    JSONRoot root(cJSON_Object);
    JSONElement ele = root.toElement();
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
    JSONRoot root(cJSON_Object);
    JSONElement ele = root.toElement();
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
