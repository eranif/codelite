//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : simpletable.cpp
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
#include "simpletable.h"
#include <wx/textdlg.h> //wxGetTextFromUser
#include "manager.h"
#include <wx/regex.h>
#include <wx/xrc/xmlres.h>
#include "debuggermanager.h"
#include "globals.h"
#include "debugger.h"
#include "simpletablebase.h"
#include "event_notifier.h"
#include <wx/utils.h>

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

WatchesTable::WatchesTable(wxWindow* parent)
    : DebuggerTreeListCtrlBase(parent)
{
    InitTable();
    // Only sort top level items, don't sort their children
    m_listTable->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
    
    m_DBG_USERR = DBG_USERR_WATCHTABLE;
    m_QUERY_NUM_CHILDS = QUERY_NUM_CHILDS;
    m_LIST_CHILDS = LIST_WATCH_CHILDS;
    // Should a be placed before b?
    clSortFunc_t func = [&](clRowEntry* a, clRowEntry* b) {
        return (a->GetLabel().CmpNoCase(b->GetLabel()) < 0);
    };
    
    // Load the right click menu
    m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("dbg_watch_rmenu"));
    Connect(
        XRCID("del_expr"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(WatchesTable::OnDeleteWatch), NULL, this);
    Connect(XRCID("del_expr_all"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnDeleteAll),
            NULL,
            this);
    Connect(XRCID("edit_expr"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuEditExpr),
            NULL,
            this);
    Connect(XRCID("copy_value"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuCopyValue),
            NULL,
            this);
    Connect(XRCID("add_watch"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnNewWatch_Internal),
            NULL,
            this);
    Connect(XRCID("copy_both"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuCopyBoth),
            NULL,
            this);

    Connect(XRCID("watches_df_natural"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
    Connect(XRCID("watches_df_hex"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
    Connect(XRCID("watches_df_bin"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
    Connect(XRCID("watches_df_octal"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
    Connect(XRCID("watches_df_decimal"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);

    // UI events
    Connect(XRCID("edit_expr"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WatchesTable::OnMenuEditExprUI), NULL, this);
    Connect(XRCID("del_expr"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WatchesTable::OnDeleteWatchUI), NULL, this);

    EventNotifier::Get()->Connect(
        wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, clCommandEventHandler(WatchesTable::OnTypeResolveError), NULL, this);
    SetDropTarget(new WatchDropTarget(this));
}

WatchesTable::~WatchesTable()
{
    EventNotifier::Get()->Connect(
        wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, clCommandEventHandler(WatchesTable::OnTypeResolveError), NULL, this);

    if(m_rclickMenu) {
        delete m_rclickMenu;
        m_rclickMenu = NULL;
    }

    Clear();
    m_listTable->DeleteAllItems();
}

void WatchesTable::Clear()
{
    wxTreeItemId root = m_listTable->GetRootItem();
    if(root.IsOk()) {
        if(m_listTable->HasChildren(root)) {
            m_listTable->DeleteChildren(root);
        }
    }
}

void WatchesTable::InitTable()
{
    m_listTable->AddRoot("Watches");
    m_listTable->AddHeader(_("Expression"));
    m_listTable->AddHeader(_("Value"));
    m_listTable->AddHeader(_("Type"));
}

void WatchesTable::OnListKeyDown(wxTreeEvent& event)
{
    if(event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {

        wxTreeItemId item = m_listTable->GetSelection();
        if(item.IsOk() == false) return;

        DoDeleteWatch(item);
        m_listTable->Delete(item);

    } else {
        event.Skip();
    }
}

void WatchesTable::OnItemRightClick(wxTreeEvent& event)
{
    wxUnusedVar(event);
    if(m_rclickMenu) {
        PopupMenu(m_rclickMenu);
    }
}

void WatchesTable::OnNewWatch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString expr = wxGetTextFromUser(_("Expression to watch:"), _("New watch"));
    if(expr.IsEmpty() == false) {
        AddExpression(expr);
        RefreshValues(false);
    }
}

void WatchesTable::OnNewWatchUI(wxUpdateUIEvent& event) { event.Enable(true); }

void WatchesTable::OnDeleteAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Clear();
}

void WatchesTable::OnDeleteAllUI(wxUpdateUIEvent& event)
{
    wxTreeItemId root = m_listTable->GetRootItem();

    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_listTable->GetFirstChild(root, cookie);

    event.Enable(root.IsOk() && child.IsOk());
}

void WatchesTable::OnDeleteWatch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = m_listTable->GetSelection();
    if(item.IsOk() == false) return;

    DoDeleteWatch(item);
    m_listTable->Delete(item);
}

void WatchesTable::OnDeleteWatchUI(wxUpdateUIEvent& event)
{
    wxTreeItemId item = m_listTable->GetSelection();
    event.Enable(item.IsOk() && m_listTable->GetItemParent(item) == m_listTable->GetRootItem());
}

void WatchesTable::AddExpression(const wxString& expr)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && ManagerST::Get()->DbgCanInteract()) {
        dbgr->ResolveType(expr, DBG_USERR_WATCHTABLE);

    } else {
        DebuggerEventData dummy;
        dummy.m_expression = expr;
        dummy.m_evaluated = expr;
        OnTypeResolved(dummy);
    }
}

void WatchesTable::UpdateVariableObjects()
{
    IDebugger *debugger = DebuggerMgr::Get().GetActiveDebugger();
    CHECK_PTR_RET(debugger);

    wxTreeItemId root = m_listTable->GetRootItem();
    wxTreeItemIdValue cookieOne;
    wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
    while( item.IsOk() ) {
        wxString gdbID = DoGetGdbId(item);
        if(!gdbID.IsEmpty()) {
            debugger->UpdateWatch(gdbID);
        }
        item = m_listTable->GetNextChild(root, cookieOne);
    }
}

void WatchesTable::RefreshValues(bool repositionEditor)
{
    // indicate in the global manager if we want to actually reposition the editor's position after the
    // dbgr->QueryFileLine() refresh
    ManagerST::Get()->SetRepositionEditor(repositionEditor);

    // loop over the childrens, if we got a valid variable object, re-evaluate it
    wxTreeItemId root = m_listTable->GetRootItem();
    if(root.IsOk() == false) return;

    // Obtain the debugger and make sure that we can interact with it
    IDebugger* dbgr = DoGetDebugger();
    if(!dbgr) return;

    // rese tree items colour to black
    DoResetItemColour(root, 0);

    // Send command to update all variable objects of this tree
    UpdateVariableObjects();

    // Loop over the top level entries and search for items that has no gdbId
    // for those items, create a variable object
    wxTreeItemIdValue cookieOne;
    wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
    while(item.IsOk()) {

        DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
        if(data && data->_gdbId.IsEmpty()) {
            dbgr->CreateVariableObject(m_listTable->GetItemText(item), false, m_DBG_USERR);
            m_createVarItemId[m_listTable->GetItemText(item)] = item;
        }

        item = m_listTable->GetNextChild(root, cookieOne);
    }
}

wxArrayString WatchesTable::GetExpressions()
{
    wxArrayString expressions;

    wxTreeItemId root = m_listTable->GetRootItem();
    if(root.IsOk() && m_listTable->HasChildren(root)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);
        while(item.IsOk()) {
            expressions.Add(m_listTable->GetItemText(item));
            item = m_listTable->GetNextChild(root, cookie);
        }
    }

    return expressions;
}

void WatchesTable::OnMenuEditExpr(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = m_listTable->GetSelection();
    if(item.IsOk() == false) return;

    IDebugger* dbgr = DoGetDebugger();
    if(!dbgr) {
        return;
    }

    wxString newExpr = wxGetTextFromUser(_("Update expression:"), _("Update Watch"), m_listTable->GetItemText(item));
    if(newExpr.IsEmpty()) return;

    DoUpdateExpression(item, newExpr);
}

void WatchesTable::DoUpdateExpression(const wxTreeItemId& item, const wxString& newExpr)
{
    IDebugger* dbgr = DoGetDebugger();
    if(!dbgr) {
        return;
    }

    DoDeleteWatch(item);
    m_listTable->Delete(item);
    AddExpression(newExpr);
}

void WatchesTable::OnMenuCopyBoth(wxCommandEvent& event)
{
    wxTreeItemId item = m_listTable->GetSelection();
    if(item.IsOk()) {
        CopyToClipboard(m_listTable->GetItemText(item) + wxT(" ") + m_listTable->GetItemText(item, 1));
    }
}

void WatchesTable::OnMenuCopyValue(wxCommandEvent& event)
{
    wxTreeItemId item = m_listTable->GetSelection();
    if(item.IsOk()) {
        CopyToClipboard(m_listTable->GetItemText(item, 1));
    }
}

void WatchesTable::OnNewWatch_Internal(wxCommandEvent& event)
{
    wxString expr = event.GetString();
    if(expr.empty() == false) {
        AddExpression(expr);
        RefreshValues(false);
    }
}

void WatchesTable::OnCreateVariableObject(const DebuggerEventData& event)
{
    wxString expr = event.m_expression;
    IDebugger* dbgr = DoGetDebugger();
    std::map<wxString, wxTreeItemId>::iterator iter = m_createVarItemId.find(expr);
    if(iter != m_createVarItemId.end()) {
        wxTreeItemId item = iter->second;
        m_createVarItemId.erase(iter);

        if(event.m_variableObject.gdbId.IsEmpty() == false) {
            // set the variable object
            DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
            if(data) {
                data->_gdbId = event.m_variableObject.gdbId;

                // set the type
                m_listTable->SetItemText(item, event.m_variableObject.typeName, 2);
                if (dbgr->GetDebuggerInformation().defaultHexDisplay == true)
                  dbgr->SetVariableObbjectDisplayFormat(DoGetGdbId(item), DBG_DF_HEXADECIMAL);
                // refresh this item only
                if(dbgr) DoRefreshItem(dbgr, item, true);

                // Query the debugger to see if this node has a children
                // In case it does, we add a dummy node so we will get the [+] sign
                dbgr->ListChildren(data->_gdbId, m_QUERY_NUM_CHILDS);
                m_listChildItemId[data->_gdbId] = item;
            }

        } else {

            if(dbgr) {
                // the refresh will create the item again if it has no gdbId
                DoRefreshItem(dbgr, item, true);
            }
        }
    }
}

void WatchesTable::OnListChildren(const DebuggerEventData& event)
{
    wxString gdbId = event.m_expression;
    std::map<wxString, wxTreeItemId>::iterator iter = m_listChildItemId.find(gdbId);
    if(iter == m_listChildItemId.end()) return;

    wxTreeItemId item = iter->second;
    m_listChildItemId.erase(iter);

    if(event.m_userReason == m_QUERY_NUM_CHILDS) {
        if(event.m_varObjChildren.empty() == false) m_listTable->AppendItem(item, wxT("<dummy>"));

    } else if(event.m_userReason == m_LIST_CHILDS) {
        if(event.m_varObjChildren.empty() == false) {
            for(size_t i = 0; i < event.m_varObjChildren.size(); i++) {
                IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
                if(!dbgr || !ManagerST::Get()->DbgCanInteract()) return;

                VariableObjChild ch = event.m_varObjChildren.at(i);
                if(ch.varName == wxT("public") || ch.varName == wxT("private") || ch.varName == wxT("protected")) {
                    // not really a node...
                    // ask for information about this node children
                    dbgr->ListChildren(ch.gdbId, m_LIST_CHILDS);
                    m_listChildItemId[ch.gdbId] = item;

                } else {

                    DbgTreeItemData* data = new DbgTreeItemData();
                    data->_gdbId = ch.gdbId;
                    wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);
                    m_listTable->SetItemText(child,ch.type, 2);

                    // Add a dummy node
                    if(child.IsOk() && ch.numChilds > 0) {
                        m_listTable->AppendItem(child, wxT("<dummy>"));
                    }

                    // refresh this item only
                    dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
                    // ask the value for this node
                    m_gdbIdToTreeId[data->_gdbId] = child;
                }
            }
        }
    }
}

void WatchesTable::OnItemExpanding(wxTreeEvent& event)
{
    wxBusyCursor bc;
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_listTable->GetFirstChild(event.GetItem(), cookie);

    IDebugger* dbgr = DoGetDebugger();
    if(!dbgr) {
        // dont allow the expansion of this item
        event.Veto();
        return;
    }

    if(child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {
        // a dummy node, replace it with the real node content
        m_listTable->Delete(child);

        DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(event.GetItem());
        if(data) {
            if(data->_gdbId.IsEmpty() == false) {
                dbgr->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
                dbgr->ListChildren(data->_gdbId, m_LIST_CHILDS);
                m_listChildItemId[data->_gdbId] = event.GetItem();
            }
        }
    }
}

void WatchesTable::OnMenuEditExprUI(wxUpdateUIEvent& event)
{
    // we enable it for root elements only
    wxTreeItemId item = m_listTable->GetSelection();
    event.Enable(item.IsOk() && m_listTable->GetItemParent(item) == m_listTable->GetRootItem());
}

void WatchesTable::OnListEditLabelBegin(wxTreeEvent& event)
{
    if(event.GetItem().IsOk()) {
        if(m_listTable->GetItemParent(event.GetItem()) != m_listTable->GetRootItem()) {
            event.Veto();
            return;
        }
    }
    event.Skip();
}

void WatchesTable::OnListEditLabelEnd(wxTreeEvent& event)
{
    if(event.GetItem().IsOk()) {
        DoUpdateExpression(event.GetItem(), event.GetLabel());
    }
    event.Skip();
}

void WatchesTable::OnMenuDisplayFormat(wxCommandEvent& event)
{
    DisplayFormat df = DBG_DF_NATURAL;
    if(event.GetId() == XRCID("watches_df_natural")) {
        df = DBG_DF_NATURAL;

    } else if(event.GetId() == XRCID("watches_df_hex")) {
        df = DBG_DF_HEXADECIMAL;

    } else if(event.GetId() == XRCID("watches_df_bin")) {
        df = DBG_DF_BINARY;

    } else if(event.GetId() == XRCID("watches_df_octal")) {
        df = DBG_DF_OCTAL;

    } else if(event.GetId() == XRCID("watches_df_decimal")) {
        df = DBG_DF_DECIMAL;
    }
    wxArrayTreeItemIds arr;
    m_listTable->GetSelections(arr);
    IDebugger* dbgr = DoGetDebugger();
    if (!dbgr) return;
    for (size_t i = 0; i < arr.GetCount();i++){
        wxTreeItemId item = arr.Item(i);
        if (!item.IsOk()) return;
        wxString gdbId = DoGetGdbId(item);
        if(gdbId.IsEmpty() == false) {
            dbgr->SetVariableObbjectDisplayFormat(gdbId, df);
            dbgr->UpdateVariableObject(gdbId, m_DBG_USERR);
            DoRefreshItem(dbgr, item, true);
        }       
    }
 
}

void WatchesTable::OnUpdateVariableObject(const DebuggerEventData& event)
{
    if(event.m_varObjUpdateInfo.refreshIds.IsEmpty()) return;

    wxArrayString itemsToRefresh = event.m_varObjUpdateInfo.refreshIds;
    IDebugger* dbgr = DoGetDebugger();
    if(dbgr) {
        DoRefreshItemRecursively(dbgr, m_listTable->GetRootItem(), itemsToRefresh);
    }
}

void WatchesTable::OnTypeResolved(const DebuggerEventData& event)
{
    wxString expr = ::DbgPrependCharPtrCastIfNeeded(event.m_expression, event.m_evaluated);

    // make sure that the expression does not exist
    wxTreeItemId root = m_listTable->GetRootItem();
    if(!root.IsOk()) {
        return;
    }

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!dbgr) {
        return;
    }

    // Obtain the debugger and make sure that we can interact with it
    if(!ManagerST::Get()->DbgCanInteract()) return;

    // Append the new item and call the debugger to create a new variable object for this
    // expression
    wxTreeItemId item = m_listTable->AppendItem(root, expr, -1, -1, new DbgTreeItemData());
    dbgr->CreateVariableObject(expr, false, m_DBG_USERR);
    m_createVarItemId[expr] = item;
}

void WatchesTable::OnRefresh(wxCommandEvent& event)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!dbgr || !ManagerST::Get()->DbgCanInteract()) {
        return;
    }

    // First delete all variable objects
    wxArrayString watches;

    // Loop over the top level entries and search for items that has no gdbId
    // for those items, create a variable object
    wxTreeItemIdValue cookieOne;
    wxTreeItemId root = m_listTable->GetRootItem();
    wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
    while(item.IsOk()) {

        watches.Add(m_listTable->GetItemText(item));
        DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
        if(data && data->_gdbId.IsEmpty()) {
            dbgr->DeleteVariableObject(data->_gdbId);
        }
        item = m_listTable->GetNextChild(root, cookieOne);
    }
    m_createVarItemId.clear();
    Clear();
    for(size_t i = 0; i < watches.GetCount(); ++i) {
        AddExpression(watches.Item(i));
    }
}

void WatchesTable::OnRefreshUI(wxUpdateUIEvent& event)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    event.Enable(dbgr && ManagerST::Get()->DbgCanInteract());
}

void WatchesTable::OnTypeResolveError(clCommandEvent& event)
{
    DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(event.GetClientObject());
    if(ded && ded->m_userReason == m_DBG_USERR) {
        // this event was meant for us
        // could not resolve type
        wxTreeItemId id = DoFindItemByExpression(ded->m_expression);
        if(id.IsOk()) {
            // update
            m_listTable->SetItemText(id, ded->m_text, 1);
        } else {
            // add
            wxTreeItemId child = m_listTable->AppendItem(m_listTable->GetRootItem(), ded->m_expression);
            m_listTable->SetItemText(child, ded->m_text, 1);
        }
    } else {
        event.Skip();
    }
}

// ----------------------------------------------------------------

bool WatchDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
    m_parent->AddExpression(text);
    return false; // so the text won't get cut from the editor...
}
