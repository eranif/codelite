//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpp_symbol_tree.cpp
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
#include "cl_editor.h"
#include "globals.h"
#include "pluginmanager.h"
#include "precompiled_header.h"
#include "stringsearcher.h"

#include "bitmap_loader.h"
#include "cpp_symbol_tree.h"
#include "frame.h"
#include "manager.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_DYNAMIC_CLASS(CppSymbolTree, SymbolTree)

const wxEventType wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED = wxNewEventType();

CppSymbolTree::CppSymbolTree() {}

CppSymbolTree::CppSymbolTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : SymbolTree(parent, id, pos, size, style)
{
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &CppSymbolTree::OnItemActivated, this);
    MSWSetNativeTheme(this);
}

bool CppSymbolTree::DoItemActivated(wxTreeItemId item, wxEvent& event, bool notify)
{
    //-----------------------------------------------------
    // Each tree items, keeps a private user data that
    // holds the key for searching the its corresponding
    // node in the m_tree data structure
    //-----------------------------------------------------
    if(!item.IsOk()) { return false; }

    MyTreeItemData* itemData = static_cast<MyTreeItemData*>(GetItemData(item));
    if(!itemData) {
        event.Skip();
        return false;
    }

    wxString filename = itemData->GetFileName();
    wxString project = ManagerST::Get()->GetProjectNameByFile(filename);
    wxString pattern = itemData->GetPattern();
    int lineno = itemData->GetLine();

    // Open the file and set the cursor to line number
    if(clMainFrame::Get()->GetMainBook()->OpenFile(filename, project, lineno - 1)) {
        // get the editor, and search for the pattern in the file
        clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if(editor) { FindAndSelect(editor, pattern, GetItemText(item)); }
    }

    // post an event that an item was activated
    if(notify) {
        wxCommandEvent e(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED);
        e.SetEventObject(this);
        wxPostEvent(GetEventHandler(), e);
    }
    return true;
}

void CppSymbolTree::OnItemActivated(wxTreeEvent& event) { DoItemActivated(event.GetItem(), event, true); }

void CppSymbolTree::FindAndSelect(clEditor* editor, wxString& pattern, const wxString& name)
{
    editor->FindAndSelectV(pattern, name);
}

void CppSymbolTree::ItemActivated()
{
    wxCommandEvent dummy;
    DoItemActivated(GetFocusedItem(), dummy, true);
}
