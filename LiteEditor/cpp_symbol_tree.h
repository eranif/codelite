//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpp_symbol_tree.h
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
#ifndef LITEEDITOR_CPP_SYMBOL_TREE_H
#define LITEEDITOR_CPP_SYMBOL_TREE_H

#include "symbol_tree.h"
#include "stack"

extern const wxEventType wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED;

/// This class represents the GUI tree for the C++ symbols
class CppSymbolTree : public SymbolTree
{
    std::stack<wxTreeItemId> m_itemsStack;

public:
    CppSymbolTree();

    /// Nothing special here, just call our parent constructor
    CppSymbolTree(wxWindow* parent,
                  const wxWindowID id,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS);

    /// destructor
    virtual ~CppSymbolTree(){};

protected:
    virtual void OnItemActivated(wxTreeEvent& event);
    bool DoItemActivated(wxTreeItemId item, wxEvent& event, bool notify);
    void FindAndSelect(clEditor* editor, wxString& pattern, const wxString& name);

    DECLARE_DYNAMIC_CLASS(CppSymbolTree)
};

#endif // LITEEDITOR_CPP_SYMBOL_TREE_H
