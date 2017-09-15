//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : localstable.h
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

#ifndef __localstable__
#define __localstable__

#include "simpletablebase.h"
#include <wx/arrstr.h>
#include "debuggerobserver.h"
#include "debuggersettings.h"
#include "cl_command_event.h"

#define LIST_LOCALS_CHILDS 600
#define QUERY_LOCALS_CHILDS 601
#define QUERY_LOCALS_CHILDS_FAKE_NODE 602

class LocalsTable : public DebuggerTreeListCtrlBase
{
protected:
    DebuggerPreDefinedTypes m_preDefTypes;
    bool m_resolveLocals;
    bool m_arrayAsCharPtr;
    bool m_sortAsc;
    bool m_defaultHexDisplay;
    

protected:
    void DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved,
                                         size_t flags,
                                         std::map<wxString, wxString>& oldValues);
    void DoUpdateLocals(const LocalVariables& locals, size_t kind);

    // Events
    void OnItemExpanding(wxTreeEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnRefreshUI(wxUpdateUIEvent& event);
    void OnItemRightClick(wxTreeEvent& event);
    void OnEditValue(wxCommandEvent& event);
    void OnEditValueUI(wxUpdateUIEvent& event);
    void OnStackSelected(clCommandEvent& event);
    void OnSortItems(wxCommandEvent& event);

public:
    LocalsTable(wxWindow* parent);
    virtual ~LocalsTable();

public:
    /**
     * @brief callback to IDebugger::CreateVariableObject
     * @param event
     */
    void OnCreateVariableObj(const DebuggerEventData& event);
    /**
     * @brief callback to IDebugger::ListChildren
     */
    void OnListChildren(const DebuggerEventData& event);
    /**
     * @brief called to IDEbugger::UpdateVariableObject
     */
    void OnVariableObjUpdate(const DebuggerEventData& event);

    void UpdateLocals(const LocalVariables& locals);
    void UpdateFrameInfo();

    void UpdateFuncArgs(const LocalVariables& args);
    void UpdateFuncReturnValue(const wxString& retValueGdbId);
    void Initialize();

    DECLARE_EVENT_TABLE()
};
#endif // __localstable__
