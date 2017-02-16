//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBLocalsView.h
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

#ifndef LLDBLOCALSVIEW_H
#define LLDBLOCALSVIEW_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBVariable.h"
#include "cl_treelistctrl.h"
#include <map>

class LLDBPlugin;
class LLDBLocalsView : public LLDBLocalsViewBase
{
    typedef std::map<int, wxTreeItemId> IntItemMap_t;

    LLDBPlugin* m_plugin;
    clTreeListCtrl* m_treeList;
    LLDBLocalsView::IntItemMap_t m_pendingExpandItems;
    wxStringSet_t m_expandedItems;
    std::map<wxString, wxTreeItemId> m_pathToItem;

private:
    void DoAddVariableToView(const LLDBVariable::Vect_t& variables, wxTreeItemId parent);
    LLDBVariableClientData* GetItemData(const wxTreeItemId& id);
    void Cleanup();
    void GetWatchesFromSelections(wxArrayTreeItemIds& items);
    wxString GetItemPath(const wxTreeItemId& item);

protected:
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
    virtual void OnNewWatch(wxCommandEvent& event);
    // events coming from LLDB
    void OnLLDBStarted(LLDBEvent& event);
    void OnLLDBExited(LLDBEvent& event);
    void OnLLDBLocalsUpdated(LLDBEvent& event);
    void OnLLDBRunning(LLDBEvent& event);
    void OnLLDBVariableExpanded(LLDBEvent& event);

    // UI events
    void OnItemExpanding(wxTreeEvent& event);
    void OnLocalsContextMenu(wxContextMenuEvent& event);

public:
    LLDBLocalsView(wxWindow* parent, LLDBPlugin* plugin);
    virtual ~LLDBLocalsView();
};
#endif // LLDBLOCALSVIEW_H
