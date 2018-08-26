#include "FilesCollector.h"
#include "file_logger.h"
#include "php_project.h"
#include "php_workspace.h"
#include <cl_command_event.h>
#include <event_notifier.h>
#include <macros.h>
#include <plugin.h>
#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#include "clFileSystemEvent.h"

wxDEFINE_EVENT(wxEVT_PHP_PROJECT_FILES_SYNC_START, clCommandEvent);
wxDEFINE_EVENT(wxEVT_PHP_PROJECT_FILES_SYNC_END, clCommandEvent);

class PHPProjectSyncThread : public wxThread
{
public:
    struct Data {
        wxString excludes;
        wxString includes;
        wxString directory;
        wxString projectName;
    };

protected:
    wxEvtHandler* m_owner;
    PHPProjectSyncThread::Data m_projectData;

public:
    PHPProjectSyncThread(wxEvtHandler* owner, const PHPProjectSyncThread::Data& data)
        : wxThread(wxTHREAD_DETACHED)
        , m_owner(owner)
        , m_projectData(data)
    {
    }

    virtual ~PHPProjectSyncThread() {}
    void* Entry()
    {
        clDEBUG() << "Scanning files for project:" << m_projectData.projectName << "..." << clEndl;

        // Report the start event
        {
            clCommandEvent event(wxEVT_PHP_PROJECT_FILES_SYNC_START);
            event.SetString(m_projectData.projectName);
            m_owner->AddPendingEvent(event);
        }

        // Scan and collect the files
        clCommandEvent event(wxEVT_PHP_PROJECT_FILES_SYNC_END);
        FilesCollector collector(event.GetStrings(), m_projectData.includes, m_projectData.excludes, NULL);
        collector.Collect(m_projectData.directory);

        // Notify about sync completed
        event.SetString(m_projectData.projectName);
        m_owner->AddPendingEvent(event);

        clDEBUG() << "Scanning files for project:" << m_projectData.projectName << "... is completed" << clEndl;
        return NULL;
    }

    void Start()
    {
        Create();
        Run();
    }
};

PHPProject::PHPProject()
    : m_isActive(false)
    , m_importFileSpec(
          "*.php;*.php5;*.inc;*.phtml;*.js;*.html;*.css;*.scss;*.less;*.json;*.xml;*.ini;*.md;*.txt;*.text;."
          "htaccess;*.ctp")
    , m_excludeFolders(".git;.svn;.codelite;.clang")
{
    Bind(wxEVT_PHP_PROJECT_FILES_SYNC_START, &PHPProject::OnFileScanStart, this);
    Bind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPProject::OnFileScanEnd, this);
}

PHPProject::~PHPProject()
{
    Unbind(wxEVT_PHP_PROJECT_FILES_SYNC_START, &PHPProject::OnFileScanStart, this);
    Unbind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPProject::OnFileScanEnd, this);
}

void PHPProject::FromJSON(const JSONElement& element)
{
    m_importFileSpec = element.namedObject("m_importFileSpec").toString(m_importFileSpec);
    m_excludeFolders = element.namedObject("m_excludeFolders").toString(m_excludeFolders);
    m_name = element.namedObject("m_name").toString();
    m_isActive = element.namedObject("m_isActive").toBool();
    m_settings.FromJSON(element.namedObject("settings"));
}

void PHPProject::ToJSON(JSONElement& pro) const
{
    pro.addProperty("m_name", m_name);
    pro.addProperty("m_isActive", m_isActive);
    pro.addProperty("m_importFileSpec", m_importFileSpec);
    pro.addProperty("m_excludeFolders", m_excludeFolders);
    pro.append(m_settings.ToJSON());
}

wxArrayString& PHPProject::GetFiles(wxProgressDialog* progress)
{
    wxUnusedVar(progress);
    return m_files;
}

void PHPProject::GetFilesArray(wxArrayString& files) const
{
    files.Alloc(m_files.size());
    files.insert(files.end(), m_files.begin(), m_files.end());
}

void PHPProject::Create(const wxFileName& filename, const wxString& name)
{
    m_filename = filename;
    m_name = name;
    Save();
}

void PHPProject::Load(const wxFileName& filename)
{
    m_filename = filename;
    JSONRoot root(m_filename);
    FromJSON(root.toElement());
}

void PHPProject::Save()
{
    JSONRoot root(cJSON_Object);
    JSONElement pro = root.toElement();
    ToJSON(pro);
    root.save(m_filename);
}

void PHPProject::FolderDeleted(const wxString& name, bool notify)
{
    // Normalize the folder name by using wxFileName
    wxFileName folder(name, "");
    wxString folderRemoved = folder.GetPath();

    wxArrayString updatedArray;
    wxArrayString deletedFiles;

    updatedArray.Alloc(m_files.size());
    deletedFiles.Alloc(m_files.size());
    for(size_t i = 0; i < m_files.GetCount(); ++i) {
        if(!m_files.Item(i).StartsWith(name)) {
            updatedArray.Add(m_files.Item(i));
        } else {
            deletedFiles.Add(m_files.Item(i));
        }
    }

    // Free extra memory
    updatedArray.Shrink();
    deletedFiles.Shrink();

    // Update the list
    m_files.swap(updatedArray);
    m_files.Sort();
    if(notify) {
        clCommandEvent event(wxEVT_PROJ_FILE_REMOVED);
        event.SetStrings(deletedFiles);
        EventNotifier::Get()->AddPendingEvent(event);
    }
}

void PHPProject::FileRenamed(const wxString& oldname, const wxString& newname, bool notify)
{
    int where = m_files.Index(oldname);
    if(where != wxNOT_FOUND) { m_files.Item(where) = newname; }

    if(notify && (where != wxNOT_FOUND)) {
        {
            wxArrayString arr;
            arr.Add(oldname);
            clCommandEvent event(wxEVT_PROJ_FILE_REMOVED);
            event.SetStrings(arr);
            EventNotifier::Get()->AddPendingEvent(event);
        }
        {
            wxArrayString arr;
            arr.Add(newname);
            clCommandEvent event(wxEVT_PROJ_FILE_ADDED);
            event.SetStrings(arr);
            EventNotifier::Get()->AddPendingEvent(event);
        }

        // And finally notify about rename-event
        clFileSystemEvent renameEvent(wxEVT_FILE_RENAMED);
        renameEvent.SetPath(oldname);
        renameEvent.SetNewpath(newname);
        EventNotifier::Get()->AddPendingEvent(renameEvent);
    }
}

void PHPProject::SynchWithFileSystem()
{
    m_files.Clear();
    // Call GetFiles so the m_files will get populated again
    GetFiles(NULL);
}

void PHPProject::FilesDeleted(const wxArrayString& files, bool notify)
{
    if(files.IsEmpty()) return;

    // Normalize the folder name by using wxFileName
    for(size_t i = 0; i < files.GetCount(); ++i) {
        int where = m_files.Index(files.Item(i));
        if(where != wxNOT_FOUND) { m_files.RemoveAt(where); }
    }

    if(notify) {
        clCommandEvent event(wxEVT_PROJ_FILE_REMOVED);
        event.SetStrings(files);
        EventNotifier::Get()->AddPendingEvent(event);
    }
}

bool PHPProject::HasFile(const wxFileName& filename) const
{
    return filename.GetFullPath().StartsWith(GetFilename().GetPath());
}

void PHPProject::FileAdded(const wxString& filename, bool notify)
{
    if(m_files.Index(filename) == wxNOT_FOUND) {
        m_files.Add(filename);
        m_files.Sort();
    }

    if(notify) {
        clCommandEvent event(wxEVT_PROJ_FILE_ADDED);
        wxArrayString files;
        files.Add(filename);
        event.SetStrings(files);
        EventNotifier::Get()->AddPendingEvent(event);
    }
}

void PHPProject::FolderAdded(const wxString& folderpath)
{
    wxFileName fakeFile(folderpath, FOLDER_MARKER);
    if(m_files.Index(fakeFile.GetFullPath()) == wxNOT_FOUND) {
        m_files.Add(fakeFile.GetFullPath());
        m_files.Sort();
    }
}

void PHPProject::SyncWithFileSystemAsync(wxEvtHandler* owner)
{
    // Build the info needed for the thread
    PHPProjectSyncThread::Data data;
    data.directory = GetFilename().GetPath();
    data.projectName = GetName();
    data.excludes = m_excludeFolders;
    data.includes = m_importFileSpec;

    // Start the background thread, a detached one which deletes itself at the end
    PHPProjectSyncThread* thr = new PHPProjectSyncThread((owner ? owner : this), data);
    thr->Start();
}

void PHPProject::OnFileScanStart(clCommandEvent& event) {}

void PHPProject::OnFileScanEnd(clCommandEvent& event) { m_files.swap(event.GetStrings()); }

void PHPProject::SetFiles(const wxArrayString& files) { m_files = files; }
