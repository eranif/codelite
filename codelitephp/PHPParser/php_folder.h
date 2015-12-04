//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_folder.h
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

#ifndef PHP_FOLDER_H
#define PHP_FOLDER_H

#include <wx/string.h>
#include <map>
#include <vector>
#include <set>
#include <json_node.h>
#include <list>
#include <wx/sharedptr.h>
#include "smart_ptr.h"

class PHPFolder
{
public:
    typedef SmartPtr<PHPFolder> Ptr_t;
    typedef std::list<PHPFolder::Ptr_t> List_t;
    typedef std::map<wxString, PHPFolder::Ptr_t> Map_t;

    enum {
        kGetFilesRecursive = 0x00000001, // Return files for this folder and its children
        kGetUnixSlashes = 0x00000002,    // Use unix slashes
    };

protected:
    wxArrayString m_files;
    wxString m_name;
    PHPFolder::List_t m_children;
    PHPFolder* m_parent;

protected:
    void DoNotifyFilesRemoved(const wxArrayString& files);

public:
    class CompareByName
    {
        wxString m_name;

    public:
        CompareByName(const wxString& name)
            : m_name(name)
        {
        }
        bool operator()(const PHPFolder::Ptr_t other) const { return other->GetName() == m_name; }
    };

public:
    PHPFolder(const wxString& name)
        : m_name(name)
        , m_parent(NULL)
    {
    }
    PHPFolder()
        : m_parent(NULL)
    {
    } // invalid folder
    ~PHPFolder() {}

    /**
     * @brief return the folder path for a filename
     */
    static wxString GetFolderPathFromFileFullPath(const wxString& filename, const wxString& projectPath);

    JSONElement ToJSON() const;
    void FromJSON(const JSONElement& element);

    /**
     * @brief add child folder to this folder
     */
    PHPFolder::Ptr_t AddFolder(const wxString& name);

    bool IsOk() const { return !m_name.IsEmpty(); }

    /**
     * @brief return the file system path relative to the project file
     * @param unixPath use / instead of backslash
     */
    wxString GetPathRelativeToProject(bool unixPath = true) const;

    /**
     * @brief return the folder full path on the file system
     * @param projectPath
     */
    wxString GetFullPath(const wxString& projectPath, bool includeDots = true) const;

    /**
     * @brief create the folder on the file system
     * @param projectPath
     */
    void CreaetOnFileSystem(const wxString& projectPath);

    PHPFolder* GetParent() const { return m_parent; }

    void SetChildren(const PHPFolder::List_t& children) { this->m_children = children; }

    void SetParent(PHPFolder* parent) { this->m_parent = parent; }

    const PHPFolder::List_t& GetChildren() const { return m_children; }

    bool AddFile(const wxString& filename);
    bool RemoveFile(const wxString& filename);
    /**
     * @brief delete list of files.
     * Note that the files should be relative to the
     * @param files list of files. Relative to the folder
     * @param projectPath the project path
     * @param filesRemoved [output] the list of files that were actually removed. The list of files removed are returned
     * in their __full__ path
     */
    void RemoveFiles(const wxArrayString& files,
                     const wxString& projectPath,
                     wxArrayString& filesRemoved,
                     bool removeFromFileSystem = false);

    /**
     * @brief Rename file. old_filename _must_ be in full path
     * @param old_filename
     * @param new_filename new name only ( no need to for path )
     * @return
     */
    bool RenameFile(const wxString& old_filename, const wxString& new_filename);

    bool HasFile(const wxString& filename) const { return m_files.Index(filename) != wxNOT_FOUND; }
    void SetFiles(const wxArrayString& files) { this->m_files = files; }
    void SetName(const wxString& name) { this->m_name = name; }
    /**
     * @brief return this folder files. The list of files is returned relative to the project
     * @param files [ouptut]
     * @param projectDir [input] the folder path of the project file
     * @param flags
     */
    void GetFiles(wxArrayString& files, const wxString& projectDir, size_t flags = kGetFilesRecursive) const;

    const wxString& GetName() const { return m_name; }

    /**
     * @brief return child folder with name (name _must_ be a single name without children)
     * @param name
     * @return the folder or NULL
     */
    PHPFolder::Ptr_t Folder(const wxString& name) const;

    /**
     * @brief return map of folders (recursive). The key in the returned map is the full path of the folder relative to
     * the
     * parent project
     * @param folders [output]
     */
    void GetFolders(PHPFolder::Map_t& folders) const;

    /**
     * @brief delete children folder
     * @name the folder name. Must be a direct child of this folder
     */
    void DeleteChildFolder(const wxString& name, const wxString& projectPath, bool notify);

    /**
     * @brief delete the current folder files and all its children
     * notify about the file removal once done
     */
    void RemoveFilesRecursively(const wxString& projectPath, bool notify);
};

#endif // PHP_FOLDER_H
