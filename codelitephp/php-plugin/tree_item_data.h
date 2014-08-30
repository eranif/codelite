#ifndef PHPVIEWMODEL_H
#define PHPVIEWMODEL_H

#include <map>
#include <wx/treebase.h>

class ItemData : public wxTreeItemData
{
public:
    enum kKIND {
        Kind_Unknown = -1,
        Kind_Workspace,
        Kind_Project,
        Kind_Folder,
        Kind_File
    };

protected:
    kKIND        kind;
    bool         active;     // make sense only if kind == Kind_Project
    wxString     projectName;// make sense only if kind == Kind_Project
    wxString     file;       // make sense only if kind == Kind_File
    wxString     folderPath; // make sense only if kind == Kind_Folder
    wxString     folderName; // make sense only if kind == Kind_Folder

public:
    ItemData(kKIND k)
        : kind(k)
        , active(false)
    {}

    void SetFolderName(const wxString& folderName) {
        this->folderName = folderName;
    }
    virtual ~ItemData() {}

    void SetProjectName(const wxString& projectName) {
        this->projectName = projectName;
    }
    const wxString& GetProjectName() const {
        return projectName;
    }
    void SetActive(bool active) {
        this->active = active;
    }
    void SetFile(const wxString& file) {
        this->file = file;
    }
    void SetFolderPath(const wxString& folderPath) {
        this->folderPath = folderPath;
        if ( this->folderPath.EndsWith("/") ) {
            this->folderPath.RemoveLast();
        }
    }

    void SetKind(kKIND kind) {
        this->kind = kind;
    }
    bool IsActive() const {
        return active;
    }
    const wxString& GetFile() const {
        return file;
    }
    const wxString& GetFolderPath() const {
        return folderPath;
    }

    const wxString& GetFolderName() const {
        return folderName;
    }

    kKIND GetKind() const {
        return kind;
    }

    bool IsProject() const {
        return kind == Kind_Project;
    }

    bool IsFolder() const {
        return kind == Kind_Folder;
    }

    bool IsFile() const {
        return kind == Kind_File;
    }

    bool IsWorkspace() const {
        return kind == Kind_Workspace;
    }
};

#endif // PHPVIEWMODEL_H
