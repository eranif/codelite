//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : simpletable.h
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
#ifndef __simpletable__
#define __simpletable__

#include "cl_command_event.h"
#include "debuggerobserver.h"
#include "simpletablebase.h"

#define QUERY_NUM_CHILDREN 500
#define LIST_WATCH_CHILDREN 501

class IDebugger;
class WatchesTable;

class WatchDropTarget : public wxTextDropTarget
{
    WatchesTable* m_parent;

public:
    explicit WatchDropTarget(WatchesTable* parent)
        : m_parent(parent)
    {
    }
    ~WatchDropTarget() override = default;
    bool OnDropText(wxCoord x, wxCoord y, const wxString& text) override;
};

/** Implementing WatchesTable */
class WatchesTable : public DebuggerTreeListCtrlBase
{
    wxMenu* m_rclickMenu;

protected:
    // Handlers for WatchesTable events.
    void OnListKeyDown(wxTreeEvent& event) override;
    void OnItemRightClick(wxTreeEvent& event) override;
    void OnItemExpanding(wxTreeEvent& event) override;
    void OnListEditLabelBegin(wxTreeEvent& event) override;
    void OnListEditLabelEnd(wxTreeEvent& event) override;

    // Menu events
    void OnNewWatch(wxCommandEvent& event) override;
    void OnNewWatchUI(wxUpdateUIEvent& event) override;
    void OnDeleteWatch(wxCommandEvent& event) override;
    void OnDeleteWatchUI(wxUpdateUIEvent& event) override;
    void OnDeleteAll(wxCommandEvent& event);
    void OnDeleteAllUI(wxUpdateUIEvent& event);
    void OnMenuEditExpr(wxCommandEvent& event);
    void OnMenuEditExprUI(wxUpdateUIEvent& event);
    void OnMenuCopyValue(wxCommandEvent& event);
    void OnMenuCopyBoth(wxCommandEvent& event);
    void OnMenuDisplayFormat(wxCommandEvent& event);
    void OnNewWatch_Internal(wxCommandEvent& event);

protected:
    void InitTable();
    void DoUpdateExpression(const wxTreeItemId& item, const wxString& newExpr);

public:
    virtual void OnTypeResolveError(clCommandEvent& event);
    void OnRefreshUI(wxUpdateUIEvent& event) override;
    void OnRefresh(wxCommandEvent& event) override;
    /**
     * @brief a callback called from the global manager when the debugger
     * responded for our CreateVariableObject call
     * @param event
     */
    void OnCreateVariableObject(const DebuggerEventData& event);

    /**
     * @brief a type resolving is completed, we can now add the watch
     */
    void OnTypeResolved(const DebuggerEventData& event);
    /**
     * @brief a callback called from the global manager when the debugger
     * responded for our ListChildren call
     * @param event
     */
    void OnListChildren(const DebuggerEventData& event);
    void OnUpdateVariableObject(const DebuggerEventData& event);

    void UpdateVariableObjects();

public:
    /** Constructor */
    explicit WatchesTable(wxWindow* parent);
    ~WatchesTable() override;

    void Clear() override;

    void AddExpression(const wxString& expr);
    wxArrayString GetExpressions();
    void RefreshValues(bool repositionEditor = true);
};

#endif // __simpletable__
