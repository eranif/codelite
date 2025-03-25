//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clFileViwerTreeCtrl.h
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

#ifndef CLFILEVIWERTREECTRL_H
#define CLFILEVIWERTREECTRL_H

#include "clThemedTreeCtrl.h"
#include "codelite_exports.h"

#include <memory>
#include <wx/filename.h>
#include <wx/treectrl.h>

/**
 * @class clTreeNodeIndex
 * @brief a helper class for fast searching a folder in the tree view
 */
class WXDLLIMPEXP_SDK clTreeNodeIndex
{
    std::unordered_map<wxString, wxTreeItemId> m_children;

public:
    clTreeNodeIndex() {}
    virtual ~clTreeNodeIndex() {}

    wxTreeItemId Find(const wxString& path);
    void Add(const wxString& path, const wxTreeItemId& item);
    void Delete(const wxString& name);

    /**
     * @brief clear the index content
     */
    void Clear() { m_children.clear(); }
};

// Item data class
class WXDLLIMPEXP_SDK clTreeCtrlData : public wxTreeItemData
{
public:
    enum eKind {
        kDummy = -1,
        kRoot,   // the hidden root folder
        kFile,   // a file
        kFolder, // a folder
    };
    eKind m_kind;
    // If file, contains the fullpath otherwise, contains the path
    wxString m_path;
    wxString m_name;
    std::unique_ptr<clTreeNodeIndex> m_index;

public:
    clTreeCtrlData(eKind kind)
        : m_kind(kind)
    {
        if(IsFolder()) {
            m_index = std::make_unique<clTreeNodeIndex>();
        }
    }
    virtual ~clTreeCtrlData() = default;

    clTreeNodeIndex* GetIndex() { return m_index.get(); }

    void SetKind(const eKind& kind) { this->m_kind = kind; }
    const eKind& GetKind() const { return m_kind; }

    /**
     * @brief set the path and update the display name
     */
    void SetPath(const wxString& path)
    {
        this->m_path = path;
        if(IsFolder()) {
            wxFileName fn(m_path, "");
            if(fn.GetDirCount()) {
                m_name = fn.GetDirs().Last();
            } else {
                m_name = m_path;
            }
        } else if(IsFile()) {
            wxFileName fn(m_path);
            m_name = fn.GetFullName();
        } else {
            m_name = wxEmptyString;
        }
    }

    /**
     * @brief return the item's path. if it is a folder, return the folder path. If it is a file
     * return the file's fullpath
     */
    const wxString& GetPath() const { return m_path; }

    /**
     * @brief return the display name
     */
    const wxString& GetName() const { return m_name; }

    // Aliases
    bool IsFolder() const { return m_kind == kFolder; }
    bool IsFile() const { return m_kind == kFile; }
    bool IsDummy() const { return m_kind == kDummy; }
};

class WXDLLIMPEXP_SDK clFileViewerTreeCtrl : public clThemedTreeCtrl
{
public:
    clFileViewerTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxTR_DEFAULT_STYLE | wxTR_MULTIPLE | wxTR_HIDE_ROOT | wxBORDER_STATIC);
    virtual ~clFileViewerTreeCtrl();
};

#endif // CLFILEVIWERTREECTRL_H
