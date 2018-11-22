//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : simpletablebase.h
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

#ifndef __simpletablebase__
#define __simpletablebase__

#include "cl_treelistctrl.h"
#include "debugger.h"
#include "debuggerobserver.h"
#include "debuggersettingsbasedlg.h"
#include "manager.h"
#include <map>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>

///////////////////////////////////////////////////////////////////////////

class DbgTreeItemData : public wxTreeItemData
{
public:
    wxString _gdbId;
    size_t _kind;
    bool _isFake;
    wxString _retValueGdbValue;

public:
    enum {
        Locals = 0x00000001,
        FuncArgs = 0x00000002,
        VariableObject = 0x00000004,
        Watch = 0x00000010,
        FuncRetValue = 0x00000020
    };

public:
    DbgTreeItemData()
        : _kind(Locals)
        , _isFake(false)
    {
    }

    DbgTreeItemData(const wxString& gdbId)
        : _gdbId(gdbId)
        , _isFake(false)
    {
    }

    virtual ~DbgTreeItemData() {}
};

///////////////////////////////////////////////////////////////////////////////
/// Class SimpleTableBase
///////////////////////////////////////////////////////////////////////////////
class DebuggerTreeListCtrlBase : public LocalsTableBase
{
private:
    bool m_withButtons;

protected:
    virtual void OnSortItems(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
    virtual void OnDeleteWatch(wxCommandEvent& event);
    virtual void OnDeleteWatchUI(wxUpdateUIEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);
    virtual void OnItemRightClick(wxTreeEvent& event);
    virtual void OnListEditLabelBegin(wxTreeEvent& event);
    virtual void OnListEditLabelEnd(wxTreeEvent& event);
    virtual void OnListKeyDown(wxTreeEvent& event);
    virtual void OnNewWatch(wxCommandEvent& event);
    virtual void OnNewWatchUI(wxUpdateUIEvent& event);
    virtual void OnRefresh(wxCommandEvent& event);

    std::map<wxString, wxTreeItemId> m_gdbIdToTreeId;
    std::map<wxString, wxTreeItemId> m_listChildItemId;
    std::map<wxString, wxTreeItemId> m_createVarItemId;
    DbgStackInfo m_curStackInfo;

protected:
    int m_DBG_USERR;
    int m_QUERY_NUM_CHILDS;
    int m_LIST_CHILDS;

public:
    DebuggerTreeListCtrlBase(wxWindow* parent, wxWindowID id = wxID_ANY, bool withButtonsPane = true,
                             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300),
                             long style = wxTAB_TRAVERSAL);
    ~DebuggerTreeListCtrlBase();

    //////////////////////////////////////////////
    // Common to both Locals / Watches
    //////////////////////////////////////////////
    virtual IDebugger* DoGetDebugger();
    virtual void DoResetItemColour(const wxTreeItemId& item, size_t itemKind);
    virtual void OnEvaluateVariableObj(const DebuggerEventData& event);
    virtual void OnCreateVariableObjError(const DebuggerEventData& event);
    virtual void DoRefreshItemRecursively(IDebugger* dbgr, const wxTreeItemId& item, wxArrayString& itemsToRefresh);
    virtual void Clear();
    virtual void DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item, bool forceCreate);
    virtual wxString DoGetGdbId(const wxTreeItemId& item);
    virtual wxTreeItemId DoFindItemByGdbId(const wxString& gdbId);
    virtual void DoDeleteWatch(const wxTreeItemId& item);
    virtual wxTreeItemId DoFindItemByExpression(const wxString& expr);
    virtual void ResetTableColors();
    virtual wxString GetItemPath(const wxTreeItemId& item);
};

#endif //__simpletablebase__
