//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : localsview.h
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

#ifndef LOCALSVIEW_H
#define LOCALSVIEW_H

#include "php_ui.h"
#include <macros.h>
#include "xdebugevent.h"
#include <map>

class LocalsView : public LocalsViewBase
{
    /// Set if items that are expanded in the current locals view
    wxStringSet_t m_localsExpandedItemsFullname;
    wxDataViewItemArray m_localsExpandedItems;
    std::map<wxString, wxDataViewItem> m_waitingExpand;

public:
    LocalsView(wxWindow* parent);
    virtual ~LocalsView();

protected:
    virtual void OnLocalsMenu(wxDataViewEvent& event);
    virtual void OnLocalExpanding(wxDataViewEvent& event);
    virtual void OnLocalCollapsed(wxDataViewEvent& event);
    virtual void OnLocalExpanded(wxDataViewEvent& event);
    void OnCopyValue(wxCommandEvent& event);

    wxString DoGetItemClientData(const wxDataViewItem& item) const;

    void OnLocalsUpdated(XDebugEvent& e);
    void OnXDebugSessionEnded(XDebugEvent& e);
    void OnXDebugSessionStarted(XDebugEvent& e);
    void OnProperytGet(XDebugEvent& e);
    void AppendVariablesToTree(const wxDataViewItem& parent, const XVariable::List_t& children);
};
#endif // LOCALSVIEW_H
