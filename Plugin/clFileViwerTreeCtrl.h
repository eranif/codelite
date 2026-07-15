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

#include "codelite_exports.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <wx/dataview.h>
#include <wx/filename.h>

/**
 * @class clTreeNodeIndex
 * @brief a helper class for fast searching a folder in the tree view
 */
class WXDLLIMPEXP_SDK clTreeNodeIndex
{
    std::unordered_map<wxString, wxDataViewItem> m_children;

public:
    clTreeNodeIndex() = default;
    virtual ~clTreeNodeIndex() = default;

    wxDataViewItem Find(const wxString& path);
    void Add(const wxString& path, const wxDataViewItem& item);
    void Delete(const wxString& name);

    /**
     * @brief clear the index content
     */
    void Clear() { m_children.clear(); }
};

// Item data class
class WXDLLIMPEXP_SDK clTreeCtrlData : public wxClientData
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
        if (IsFolder()) {
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
        if (IsFolder()) {
            wxFileName fn(m_path, "");
            if (fn.GetDirCount()) {
                m_name = fn.GetDirs().Last();
            } else {
                m_name = m_path;
            }
        } else if (IsFile()) {
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
    bool IsRoot() const { return m_kind == kRoot; }
};

class WXDLLIMPEXP_SDK clFileViewerTreeCtrl : public wxDataViewTreeCtrl
{
    // Client data for the (invisible) root item, which has no wxDataViewItem of its own.
    // Holds the index used for locating top level folders.
    clTreeCtrlData m_rootData{ clTreeCtrlData::kRoot };

    bool ShouldComeBefore(clTreeCtrlData* a, clTreeCtrlData* b) const;
    wxDataViewItem InsertSorted(const wxDataViewItem& parent,
                                const wxString& text,
                                int icon,
                                int expandedIcon,
                                bool isContainer,
                                wxClientData* data);
    void OnLeftDown(wxMouseEvent& event);

public:
    clFileViewerTreeCtrl(wxWindow* parent,
                         wxWindowID id = wxID_ANY,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxDV_MULTIPLE | wxDV_ROW_LINES | wxDV_NO_HEADER | wxBORDER_STATIC);
    virtual ~clFileViewerTreeCtrl() = default;

    /**
     * @brief the (invisible) root item. Top level folders are direct children of this item
     */
    wxDataViewItem GetRootItem() const { return wxDataViewItem(); }

    /**
     * @brief return the client data associated with an item. If item is the (invisible) root item,
     * return the synthetic root data instead (used for the top-level folders index)
     */
    clTreeCtrlData* GetItemData(const wxDataViewItem& item) const
    {
        return item.IsOk() ? static_cast<clTreeCtrlData*>(wxDataViewTreeCtrl::GetItemData(item))
                           : const_cast<clTreeCtrlData*>(&m_rootData);
    }

    bool ItemHasChildren(const wxDataViewItem& item) const { return GetChildCount(item) > 0; }

    /**
     * @brief associate bitmap vector with this tree, used as the source for icon indices
     */
    void SetBitmaps(std::vector<wxBitmap>* bitmaps);

    /**
     * @brief append a new file item, keeping the folders/hidden/case-insensitive sort order
     */
    wxDataViewItem InsertFileSorted(const wxDataViewItem& parent, const wxString& text, int icon, wxClientData* data)
    {
        return InsertSorted(parent, text, icon, wxWithImages::NO_IMAGE, false, data);
    }

    /**
     * @brief append a new folder item, keeping the folders/hidden/case-insensitive sort order
     */
    wxDataViewItem InsertFolderSorted(
        const wxDataViewItem& parent, const wxString& text, int icon, int expandedIcon, wxClientData* data)
    {
        return InsertSorted(parent, text, icon, expandedIcon, true, data);
    }
};

#endif // CLFILEVIWERTREECTRL_H
