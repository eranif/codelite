//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPOutlineTree.h
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

#ifndef PHPOUTLINETREE_H
#define PHPOUTLINETREE_H

#include "PHPEntityBase.h"
#include "clThemedTreeCtrl.h"
#include "imanager.h"
#include <wx/filename.h>

class PHPOutlineTree : public clThemedTreeCtrl
{
    wxFileName m_filename;
    IManager* m_manager;

protected:
    void BuildTree(wxTreeItemId parentTreeItem, PHPEntityBase::Ptr_t entity);
    int GetImageId(PHPEntityBase::Ptr_t entry);
    void SetEditorActive(IEditor* editor);
    wxTreeItemId DoFind(const wxString& pattern, const wxTreeItemId& parent);

public:
    PHPOutlineTree(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~PHPOutlineTree();
    void BuildTree(const wxFileName& filename);
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    const wxFileName& GetFilename() const { return m_filename; }
    void Clear();
    void SetManager(IManager* manager) { this->m_manager = manager; }
    IManager* GetManager() { return m_manager; }
    void ItemSelected(const wxTreeItemId& item, bool focusEditor);
    bool Select(const wxString& pattern);
};

#endif // PHPOUTLINETREE_H
