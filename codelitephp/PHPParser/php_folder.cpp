#include "php_folder.h"
#include <globals.h>
#include "php_event.h"
#include <plugin.h>
#include <event_notifier.h>
#include <algorithm>
#include <wx/busyinfo.h>
#include <wx/utils.h>
#include "php_strings.h"
#include "clFileSystemEvent.h"

JSONElement PHPFolder::ToJSON() const
{
    JSONElement json = JSONElement::createObject("");
    json.addProperty("m_name", m_name);
    json.addProperty("m_files", m_files);

    JSONElement arr = JSONElement::createArray("children");
    json.append(arr);

    PHPFolder::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        arr.arrayAppend((*iter)->ToJSON());
    }
    return json;
}

void PHPFolder::FromJSON(const JSONElement& element)
{
    m_children.clear();
    m_name = element.namedObject("m_name").toString();
    m_files = element.namedObject("m_files").toArrayString();

    JSONElement children = element.namedObject("children");
    int size = children.arraySize();
    for(int i = 0; i < size; ++i) {
        PHPFolder::Ptr_t child(new PHPFolder());
        child->FromJSON(children.arrayItem(i));
        child->SetParent(this);
        m_children.push_back(child);
    }
}

bool PHPFolder::RenameFile(const wxString& old_filename, const wxString& new_filename)
{
    wxFileName fnOld(old_filename);
    wxFileName fnNew(new_filename);
    fnNew.SetPath(fnOld.GetPath());

    int where = m_files.Index(fnOld.GetFullName());
    if(where == wxNOT_FOUND) {
        return false;
    }

    // a file with this name already exists
    if(fnNew.Exists()) {
        return false;
    }

    m_files.RemoveAt(where);
    m_files.Add(fnNew.GetFullName());
    m_files.Sort();

    // Step: 2
    // Notify the plugins, maybe they want to override the
    // default behavior (e.g. Subversion plugin)
    clFileSystemEvent e(wxEVT_FILE_RENAMED);
    e.SetPath(fnOld.GetFullPath());
    e.SetNewpath(fnNew.GetFullPath());
    
    if(!EventNotifier::Get()->ProcessEvent(e)) {
        wxRenameFile(fnOld.GetFullPath(), fnNew.GetFullPath());
    }
    
    PHPEvent eventFileRenamed(wxEVT_PHP_FILE_RENAMED);
    eventFileRenamed.SetOldFilename(fnOld.GetFullPath());
    eventFileRenamed.SetFileName(fnNew.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventFileRenamed);
    return true;
}

wxString PHPFolder::GetPathRelativeToProject(bool unixPath) const
{
    // go up until we find the project
    wxArrayString folders;
    folders.Add(GetName());

    PHPFolder* parent = GetParent();
    while(parent) {
        folders.Insert(parent->GetName(), 0);
        parent = parent->GetParent();
    }

    wxString sep = unixPath ? wxString("/") : wxString(wxFILE_SEP_PATH);
    wxString relativePath;
    for(size_t i = 0; i < folders.GetCount(); ++i) {
        relativePath << folders.Item(i) << sep;
    }

    if(relativePath.EndsWith(sep)) relativePath.RemoveLast();

    return relativePath;
}

PHPFolder::Ptr_t PHPFolder::AddFolder(const wxString& name)
{
    PHPFolder::List_t::iterator iter =
        std::find_if(m_children.begin(), m_children.end(), PHPFolder::CompareByName(name));
    if(iter != m_children.end()) return (*iter);

    PHPFolder::Ptr_t folder(new PHPFolder(name));
    folder->SetParent(this);
    m_children.push_back(folder);
    return folder;
}

void PHPFolder::GetFiles(wxArrayString& files, const wxString& projectDir, size_t flags) const
{
    // use native path
    wxString basepath = GetPathRelativeToProject(false);
    for(size_t i = 0; i < m_files.GetCount(); ++i) {
        wxString filename;
        if(!projectDir.IsEmpty()) {
            filename << projectDir << wxFILE_SEP_PATH;
        }
        filename << basepath << wxFILE_SEP_PATH << m_files.Item(i);
        wxFileName fn(filename);
        fn.Normalize(wxPATH_NORM_TILDE | wxPATH_NORM_DOTS);
        filename = fn.GetFullPath();

        if(flags & kGetUnixSlashes) {
            filename.Replace("\\", "/");
        }
        files.Add(filename);
    }

    if(flags & kGetFilesRecursive) {
        PHPFolder::List_t::const_iterator iter = m_children.begin();
        for(; iter != m_children.end(); ++iter) {
            (*iter)->GetFiles(files, projectDir, flags);
        }
    }
}

void PHPFolder::GetFolders(PHPFolder::Map_t& folders) const
{
    PHPFolder::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        folders.insert(std::make_pair((*iter)->GetPathRelativeToProject(), (*iter)));
        (*iter)->GetFolders(folders);
    }
}

PHPFolder::Ptr_t PHPFolder::Folder(const wxString& name) const
{
    PHPFolder::List_t::const_iterator iter =
        std::find_if(m_children.begin(), m_children.end(), PHPFolder::CompareByName(name));
    if(iter != m_children.end()) return (*iter);
    return PHPFolder::Ptr_t(NULL);
}

void PHPFolder::DoNotifyFilesRemoved(const wxArrayString& files)
{
    if(!files.IsEmpty()) {
        EventNotifier::Get()->PostFileRemovedEvent(files);
    }
}

void PHPFolder::RemoveFilesRecursively(const wxString& projectPath, bool notify)
{
    wxArrayString files;
    GetFiles(files, projectPath);
    if(notify) {
        DoNotifyFilesRemoved(files);
    }
    m_children.clear();
    m_files.clear();
}

void PHPFolder::DeleteChildFolder(const wxString& name, const wxString& projectPath, bool notify)
{
    PHPFolder::Ptr_t folder = Folder(name);
    CHECK_PTR_RET(folder);

    wxArrayString files;
    folder->GetFiles(files, projectPath);

    // iter is valid (otherwise, folder would have been NULL)
    PHPFolder::List_t::iterator iter =
        std::find_if(m_children.begin(), m_children.end(), PHPFolder::CompareByName(name));
    m_children.erase(iter);
    
    if(notify) {
        DoNotifyFilesRemoved(files);
    }
}

bool PHPFolder::AddFile(const wxString& filename)
{
    wxFileName fn(filename);
    if(m_files.Index(fn.GetFullName()) == wxNOT_FOUND) {
        m_files.Add(fn.GetFullName());
        m_files.Sort();
        return true;
    }
    return false;
}

bool PHPFolder::RemoveFile(const wxString& filename)
{
    wxFileName fn(filename);
    int where = m_files.Index(fn.GetFullName());
    if(where != wxNOT_FOUND) {
        m_files.RemoveAt(where);
        return true;
    }
    return false;
}

wxString PHPFolder::GetFullPath(const wxString& projectPath, bool includeDots) const
{
    wxString relPath = GetPathRelativeToProject(false);
    wxString path;
    path << projectPath << wxFILE_SEP_PATH << relPath;
    wxFileName fnPath(path, wxEmptyString);
    if(!includeDots) {
        fnPath.Normalize(wxPATH_NORM_DOTS);
    }
    return fnPath.GetPath();
}

void PHPFolder::CreaetOnFileSystem(const wxString& projectPath)
{
    wxString path = GetFullPath(projectPath);
    wxFileName fn(path, "");
    fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
}

wxString PHPFolder::GetFolderPathFromFileFullPath(const wxString& filename, const wxString& projectPath)
{
    wxFileName fn(filename);
    wxString folderPath = fn.GetPath();
    if(!folderPath.StartsWith(projectPath)) {
        return wxEmptyString;
    }

    folderPath.Replace(projectPath, "");
    if(!folderPath.StartsWith(".")) {
        folderPath.Prepend(".");
    }
    folderPath.Replace("\\", "/");
    return folderPath;
}

void PHPFolder::RemoveFiles(const wxArrayString& files,
                            const wxString& projectPath,
                            wxArrayString& filesRemoved,
                            bool removeFromFileSystem)
{
    for(size_t i = 0; i < files.GetCount(); ++i) {
        if(RemoveFile(files.Item(i))) {
            wxFileName fn(projectPath, files.Item(i));
            filesRemoved.Add(fn.GetFullPath());
            
            // delete it from the file system?
            if(removeFromFileSystem) {
                wxLogNull noLog;
                ::wxRemoveFile(fn.GetFullPath());
            }
        }
    }
}
