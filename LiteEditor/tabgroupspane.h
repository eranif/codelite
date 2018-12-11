//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2010 by Eran Ifrah
// file name            : tabgroupspane.h
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

#ifndef TABGROUPSPANE_H
#define TABGROUPSPANE_H

#include "wx/panel.h"
#include <wx/treectrl.h>
#include <vector>
#include <utility>
#include "theme_handler_helper.h"

class clThemedTreeCtrl;
enum tabgrouptype { TGT_group, TGT_item };

class TabGrpTreeItemData : public wxTreeItemData
{
public:
    TabGrpTreeItemData() {}
    TabGrpTreeItemData(const wxString& fp, enum tabgrouptype type = TGT_item)
        : m_filepath(fp)
        , m_type(type)
    {
    }

    wxString GetFilepath() const { return m_filepath; }
    tabgrouptype GetType() const { return m_type; }

protected:
    wxString m_filepath;
    tabgrouptype m_type; // Is this item a tabgroup or a contained tab?
};

class TabgroupsPane : public wxPanel
{
    enum tabgroupmenuitems {
        TGM_ID_Add = 10000,
        TGM_ID_Paste,
        TGM_ID_Delete,
        TGM_ID_Duplicate,
        TGM_ID_CopyItem,
        TGM_ID_CutItem,
        TGM_ID_RemoveItem
    };
    ThemeHandlerHelper* m_themeHelper;

public:
    TabgroupsPane() { m_node = NULL; }
    TabgroupsPane(wxWindow* parent, const wxString& caption);
    ~TabgroupsPane();
    void DisplayTabgroups(bool isGlobal = false);
    bool AddNewTabgroupToTree(bool isGlobal, const wxString& newfilepath, wxTreeItemId selection = wxTreeItemId());
    void FileDropped(const wxString& filename);
    /*!
     * \brief Returns the 'root' item for either Global or Workspace tabgroups
     */
    wxTreeItemId GetRootItemForTabgroup(bool global);
    
protected:
    void AddFile(const wxString& filename);
    
    void AddTreeItem(bool isGlobal, 
                     const wxString& tabgroupname,
                     const wxArrayString& tabfilepaths,
                     const wxTreeItemId insertafter = wxTreeItemId());
    void AddTabgroupItem();
    void PasteTabgroupItem(wxTreeItemId itemtopaste = wxTreeItemId());
    void DeleteTabgroup();
    void DuplicateTabgroup();
    void CopyTabgroupItem(wxTreeItemId itemtocopy = wxTreeItemId());
    void DeleteTabgroupItem(bool DoCut = false, wxTreeItemId itemtocopy = wxTreeItemId());
    int DoGetIconIndex(const wxString& filename);

    void OnContextMenu(wxCommandEvent& event);
    // The next 4 methods deal with keypresses, not the context menu
    void OnCopy(wxCommandEvent& WXUNUSED(event)) { CopyTabgroupItem(); }
    void OnPaste(wxCommandEvent& WXUNUSED(event)) { PasteTabgroupItem(); }
    void OnCut(wxCommandEvent& WXUNUSED(event)) { DeleteTabgroupItem(true); }
    void OnDelete(wxCommandEvent& WXUNUSED(event));

    void OnItemActivated(wxTreeEvent& event);
    void OnItemRtClick(wxTreeEvent& event);
    void OnBeginLabelEdit(wxTreeEvent& event);
    void OnEndLabelEdit(wxTreeEvent& event);

    void OnBeginDrag(wxTreeEvent& event);
    void OnEndDrag(wxTreeEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnInitDone(wxCommandEvent& e);
    clThemedTreeCtrl* m_tree;
    /*!
     * \brief Stores the dragged item during DnD
     */
    wxTreeItemId m_draggedItem;
    /*!
     * \brief Used as a temporary store for a copyied/cut item's data
     */
    wxXmlNode* m_node;
    /*!
     * \brief Used as a temporary store for the filepath of a copied/cut item
     */
    wxString m_copieditem_filepath;
};

#endif // TABGROUPSPANE_H
