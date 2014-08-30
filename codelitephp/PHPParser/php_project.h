#ifndef PHP_PROJECT_H
#define PHP_PROJECT_H

#include <map>
#include <vector>
#include <set>
#include "php_folder.h"
#include "php_project_settings_data.h"

class PHPProject
{
    wxString               m_name;
    bool                   m_isActive;
    PHPFolder::Map_t       m_folders;
    PHPProjectSettingsData m_settings;
    wxFileName             m_filename;
    
public:
    typedef wxSharedPtr<PHPProject> Ptr_t;
    typedef std::map<wxString, PHPProject::Ptr_t> Map_t;
    
public:
    PHPProject() : m_isActive(false) {}
    PHPProject(const wxString &name) : m_name( name ), m_isActive(false) {}
    ~PHPProject() {}
    
    void Create(const wxFileName& filename);
    void Load(const wxFileName& filename);
    void Save();
    
    void SetFilename(const wxFileName& filename) {
        this->m_filename = filename;
    }
    const wxFileName& GetFilename() const {
        return m_filename;
    }
    void SetSettings(const PHPProjectSettingsData& settings) {
        this->m_settings = settings;
    }
    const PHPProjectSettingsData& GetSettings() const {
        return m_settings;
    }
    PHPProjectSettingsData& GetSettings() {
        return m_settings;
    }

    void ToJSON(JSONElement &element) const;
    void FromJSON(const JSONElement &element);

    bool IsOk() const {
        return !m_name.IsEmpty();
    }
    bool IsActive() const {
        return m_isActive;
    }
    void SetIsActive(bool isActive) {
        this->m_isActive = isActive;
    }
    /**
     * @brief return a list of all project files (fullpath)
     */
    void GetFiles(wxArrayString& files) const;

    /**
     * @brief find and return a folder
     * @param name the folder name (relative to the project)
     * @return the folder object or NULL
     */
    PHPFolder::Ptr_t Folder(const wxString &name) const ;
    
    /**
     * @brief return a folder by file full path
     */
    PHPFolder::Ptr_t FolderByFileFullPath(const wxString &filename) const;
    /**
     * @brief return the parent folder of a folder
     * @param name path to the child folder
     * @return return the parent folder of a folder
     */
    PHPFolder::Ptr_t GetParentFolder(const wxString &child_folder) const;
    /**
     * @brief add folder to project
     */
    PHPFolder::Ptr_t AddFolder( const wxString &name);
    
    /**
     * @brief delete folder
     * @param name (e.g. A/B/C)
     */
    void DeleteFolder(const wxString &name);
    
    bool HasFolder(const wxString &name) const {
        return m_folders.count(name);
    }
    void SetName(const wxString& name) {
        this->m_name = name;
    }
    const PHPFolder::Map_t& GetFolders() const {
        return m_folders;
    }
    PHPFolder::Map_t& GetFolders() {
        return m_folders;
    }
    const wxString& GetName() const {
        return m_name;
    }
    
    /**
     * @brief import a directory to the project
     * @param path
     * @param filespec
     * @param recursive
     */
    void ImportDirectory(const wxString& path, const wxString &filespec, bool recursive);
    
    /**
     * @brief rename a file (the file stays on the same path, only its name is renamed)
     * @param filename
     * @param newFullName
     * @return true on success, false otherwise
     */
    bool RenameFile(const wxFileName& filename, const wxString& newFullName);
};
#endif
