//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_project.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef PHP_PROJECT_H
#define PHP_PROJECT_H

#include <map>
#include <vector>
#include <set>
#include "php_project_settings_data.h"
#include <wx/sharedptr.h>
#include <wx/progdlg.h>
#include "cl_command_event.h"
#include <wx/event.h>

class PHPProject : public wxEvtHandler
{
    wxString m_name;
    bool m_isActive;
    PHPProjectSettingsData m_settings;
    wxFileName m_filename;
    wxString m_importFileSpec;
    wxArrayString m_files;
    wxString m_excludeFolders;

public:
    typedef wxSharedPtr<PHPProject> Ptr_t;
    typedef std::map<wxString, PHPProject::Ptr_t> Map_t;

    /**
     * @class CreateData
     */
    struct CreateData {
        wxString path;
        wxString name;
        wxString phpExe;
        int projectType;
        bool importFilesUnderPath;
        wxString ccPaths;

        CreateData()
            : projectType(0)
            , importFilesUnderPath(false)
        {
        }
    };

private:
    void OnFileScanStart(clCommandEvent& event);
    void OnFileScanEnd(clCommandEvent& event);

public:
    PHPProject();
    virtual ~PHPProject();

    void Create(const wxFileName& filename, const wxString& name);
    void Load(const wxFileName& filename);
    void Save();

    void SetExcludeFolders(const wxString& excludeFolders) { this->m_excludeFolders = excludeFolders; }
    const wxString& GetExcludeFolders() const { return m_excludeFolders; }
    const wxString& GetImportFileSpec() const { return m_importFileSpec; }
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    const wxFileName& GetFilename() const { return m_filename; }
    void SetSettings(const PHPProjectSettingsData& settings) { this->m_settings = settings; }
    const PHPProjectSettingsData& GetSettings() const { return m_settings; }
    PHPProjectSettingsData& GetSettings() { return m_settings; }

    void ToJSON(JSONItem& element) const;
    void FromJSON(const JSONItem& element);

    bool IsOk() const { return !m_name.IsEmpty(); }
    bool IsActive() const { return m_isActive; }
    void SetIsActive(bool isActive) { this->m_isActive = isActive; }
    /**
     * @brief return a list of all project files (fullpath)
     */
    wxArrayString& GetFiles(wxProgressDialog* progress);
    
    /**
     * @brief set the project files
     */
    void SetFiles(const wxArrayString& files);
    
    /**
     * @brief return a list of all project files (fullpath)
     */
    void GetFilesArray(wxArrayString& files) const;

    /**
     * @brief check if filename is part of this project
     * @param filename
     */
    bool HasFile(const wxFileName& filename) const;

    /**
     * @brief folder was deleted from the file system. Update the cached files
     * @param name folder path
     * @param notify when set to true, fire wxEVT_PROJ_FILE_REMOVED event (useful if you wish to update
     * other plugins such as git or subversion)
     */
    void FolderDeleted(const wxString& name, bool notify);

    void SetImportFileSpec(const wxString& importFileSpec) { this->m_importFileSpec = importFileSpec; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }

    /**
     * @brief file was renamed on the file system, update the cache and notify if needed
     */
    void FileRenamed(const wxString& oldname, const wxString& newname, bool notify);

    /**
     * @brief file was added on the file syste, update the cache and notify
     */
    void FileAdded(const wxString& filename, bool notify);

    /**
     * @brief file was added on the file system update the cache
     */
    void FolderAdded(const wxString& folderpath);

    /**
     * @brief files were deleted, remove them from the files cache and notify
     */
    void FilesDeleted(const wxArrayString& files, bool notify);

    /**
     * @brief synch the project folders with the file system
     */
    void SynchWithFileSystem();

    /**
     * @brief sync the projec with the file system. But do this in a background thread
     * Once this function is done, it fires an event wxEVT_PHP_PROJECT_FILES_SYNCED
     * @owner owner the class that will receive the start/end events. If no provided (i.e. NULL is passed) 
     * the current project object will receive them
     */
    void SyncWithFileSystemAsync(wxEvtHandler* owner = NULL);
};

wxDECLARE_EVENT(wxEVT_PHP_PROJECT_FILES_SYNC_START, clCommandEvent);
wxDECLARE_EVENT(wxEVT_PHP_PROJECT_FILES_SYNC_END, clCommandEvent);

#endif
