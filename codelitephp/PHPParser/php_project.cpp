#include "php_project.h"
#include <macros.h>
#include "php_workspace.h"
#include <wx/tokenzr.h>
#include <wx/busyinfo.h>
#include <wx/utils.h>
#include <event_notifier.h>
#include <cl_command_event.h>
#include <plugin.h>
#include <macros.h>
#include <wx/msgdlg.h>
#include "FilesCollector.h"

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
    if(m_files.IsEmpty()) {
        FilesCollector traverser(m_importFileSpec, m_excludeFolders, progress);
        wxDir dir(GetFilename().GetPath());
        dir.Traverse(traverser);
        m_files.swap(traverser.GetFilesAndFolders());
        m_files.Sort();
    }
    return m_files;
}

void PHPProject::GetFilesArray(wxArrayString& files) const
{
    if(!m_files.IsEmpty()) {
        files.insert(files.end(), m_files.begin(), m_files.end());
        return;
    }

    FilesCollector traverser(m_importFileSpec, m_excludeFolders, NULL);
    wxDir dir(GetFilename().GetPath());
    dir.Traverse(traverser);
    wxArrayString& collectedFiles = traverser.GetFilesAndFolders();
    files.insert(files.end(), collectedFiles.begin(), collectedFiles.end());
    files.Sort();
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
    for(size_t i = 0; i < m_files.GetCount(); ++i) {
        if(!m_files.Item(i).StartsWith(name)) {
            updatedArray.Add(m_files.Item(i));
        } else {
            deletedFiles.Add(m_files.Item(i));
        }
    }

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
    if(where != wxNOT_FOUND) {
        m_files.Item(where) = newname;
    }

    if(notify && where != wxNOT_FOUND) {
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
        if(where != wxNOT_FOUND) {
            m_files.RemoveAt(where);
        }
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
