//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : my_tree_view.h
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

#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <wx/treectrl.h>
#include "tree_item_data.h"

class MyTreeView : public wxTreeCtrl
{
public:
    MyTreeView() {}
    MyTreeView(wxWindow* parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
    virtual ~MyTreeView();
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
    virtual int OnCompareItems(const ItemData* a, const ItemData* b);
    void SortItem(const wxTreeItemId& item)
    {
        if(item.IsOk() && ItemHasChildren(item)) {
            SortChildren(item);
        }
    }
    wxDECLARE_DYNAMIC_CLASS(MyTreeView);
};

#endif // MYTREEVIEW_H
