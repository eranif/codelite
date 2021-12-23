//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : wx_tree_traverser.h
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

#ifndef WXTREETRAVERSER_H
#define WXTREETRAVERSER_H

#include "codelite_exports.h"
#include "precompiled_header.h"

class WXDLLIMPEXP_SDK wxTreeTraverser
{
protected:
    wxTreeCtrl* m_tree;

protected:
    void DoTraverse(const wxTreeItemId& item);

public:
    wxTreeTraverser(wxTreeCtrl* tree);
    virtual ~wxTreeTraverser();

    /**
     * @brief traverse the tree starting with given 'item'
     * If item is invalid, the traverse will start from the root item
     * @param item
     */
    virtual void Traverse(const wxTreeItemId& item = wxTreeItemId());

    /**
     * @brief this method is called by the traverser for every item in the tree
     * By default this method does nothing. Override it to do something meaningfull
     * @param item current item
     */
    virtual void OnItem(const wxTreeItemId& item);
};

#endif // WXTREETRAVERSER_H
