//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : FileExplorerTab.h
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

#ifndef FILEEXPLORERTAB_H
#define FILEEXPLORERTAB_H

#include "wxcrafter.h"
#include <wx/menu.h>
#include "imanager.h"

class FileExplorerTab : public FileExplorerBase
{
protected:
    wxMenu* m_rclickMenu;
    
public:
    FileExplorerTab(wxWindow* parent);
    virtual ~FileExplorerTab();
    wxGenericDirCtrl *Tree() const {
        return m_genericDirCtrl;
    }
    TreeItemInfo GetSelectedItemInfo();
    /**
     * @brief return the number of items selected
     */
    size_t GetSelectionCount() const;
    
    /**
     * @brief if there are any items selected, clear them
     */
    void ClearSelections();
    
    /**
     * @brief return a wxFileName that represents the current selected item
     * If multiple selections are made, return the first one
     */
    bool GetSelection(wxFileName& path);
    
protected:
    virtual void OnContextMenu(wxTreeEvent& event);
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnOpenFile(wxCommandEvent &event);
    virtual void OnOpenFileInTextEditor(wxCommandEvent &event);
    virtual void OnRefreshNode(wxCommandEvent &event);
    virtual void OnDeleteNode(wxCommandEvent &event);
    virtual void OnSearchNode(wxCommandEvent &event);
    virtual void OnTagNode(wxCommandEvent &event);
    virtual void OnKeyDown( wxTreeEvent &event );
    virtual void OnOpenShell(wxCommandEvent &event);
    virtual void OnOpenExplorer(wxCommandEvent &event);
    virtual void OnOpenWidthDefaultApp(wxCommandEvent &e);
    
    void DoOpenItem(const wxString& path);
};
#endif // FILEEXPLORERTAB_H
