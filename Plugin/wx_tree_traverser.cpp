//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : wx_tree_traverser.cpp
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

#include "wx_tree_traverser.h"

wxTreeTraverser::wxTreeTraverser(wxTreeCtrl* tree)
    : m_tree(tree)
{
}

wxTreeTraverser::~wxTreeTraverser() {}

void wxTreeTraverser::DoTraverse(const wxTreeItemId& item)
{
    if(item.IsOk() == false)
        return;

    // Call the user callback
    OnItem(item);

    // Recurse the children
    if(m_tree->ItemHasChildren(item)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
        while(child.IsOk()) {
            DoTraverse(child);
            child = m_tree->GetNextChild(item, cookie);
        }
    }
}

void wxTreeTraverser::OnItem(const wxTreeItemId& item) {}

void wxTreeTraverser::Traverse(const wxTreeItemId& item) { DoTraverse(item.IsOk() ? item : m_tree->GetRootItem()); }
