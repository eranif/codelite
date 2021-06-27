//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : localstable.cpp
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

#include "debuggerconfigtool.h"
#include "debuggermanager.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "localstable.h"
#include "macros.h"
#include "manager.h"
#include "new_quick_watch_dlg.h"
#include <algorithm>
#include <set>
#include <wx/regex.h>
#include <wx/utils.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(LocalsTable, DebuggerTreeListCtrlBase)
EVT_MENU(XRCID("Change_Value"), LocalsTable::OnEditValue)
EVT_UPDATE_UI(XRCID("Change_Value"), LocalsTable::OnEditValueUI)
END_EVENT_TABLE()

LocalsTable::LocalsTable(wxWindow* parent)
    : DebuggerTreeListCtrlBase(parent, wxID_ANY, false)
    , m_arrayAsCharPtr(false)
    , m_sortAsc(true)
{
    m_listTable->AddRoot(_("Locals"));
    m_listTable->AddHeader(_("Name"));
    m_listTable->AddHeader(_("Value"));
    m_listTable->AddHeader(_("Type"));

    // Only sort top level items, don't sort their children
    m_listTable->AddTreeStyle(wxTR_SORT_TOP_LEVEL);

    SetSortingFunction();
    m_DBG_USERR = DBG_USERR_LOCALS;
    m_QUERY_NUM_CHILDS = QUERY_LOCALS_CHILDS;
    m_LIST_CHILDS = LIST_LOCALS_CHILDS;

    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_FRAME_SELECTED, clCommandEventHandler(LocalsTable::OnStackSelected),
                                  NULL, this);
}

LocalsTable::~LocalsTable() {}

void LocalsTable::UpdateLocals(const LocalVariables& locals) { DoUpdateLocals(locals, DbgTreeItemData::Locals); }

void LocalsTable::UpdateFuncArgs(const LocalVariables& args) { DoUpdateLocals(args, DbgTreeItemData::FuncArgs); }

void LocalsTable::Initialize()
{
    // Read the debugger defined commands
    DebuggerSettingsPreDefMap data;
    DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);

    DebuggerInformation info;
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr) {
        DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);
        m_resolveLocals = info.resolveLocals;
        m_arrayAsCharPtr = info.charArrAsPtr;
        m_defaultHexDisplay = info.defaultHexDisplay;
    }

    m_preDefTypes = data.GetActiveSet();
    m_curStackInfo.Clear();
}

void LocalsTable::OnCreateVariableObj(const DebuggerEventData& event)
{
    wxString expr = event.m_expression;
    std::map<wxString, wxTreeItemId>::iterator iter = m_createVarItemId.find(expr);
    if(iter != m_createVarItemId.end()) {

        // set the variable object
        DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(iter->second));
        if(data) {

            data->_gdbId = event.m_variableObject.gdbId;
            data->_kind = DbgTreeItemData::VariableObject;

            // variable object's type name is extracted from the event.m_variableObject.typeName
            const wxString& localType = event.m_variableObject.typeName;
            m_listTable->SetItemText(iter->second, localType, 2);

            // refresh this item only
            IDebugger* dbgr = DoGetDebugger();
            if(dbgr)
                DoRefreshItem(dbgr, iter->second, false);

            dbgr->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
            dbgr->ListChildren(data->_gdbId, m_LIST_CHILDS);
            m_listChildItemId[data->_gdbId] = iter->second;
        }
        m_createVarItemId.erase(iter);
    }
}

void LocalsTable::OnListChildren(const DebuggerEventData& event)
{
    wxString gdbId = event.m_expression;
    std::map<wxString, wxTreeItemId>::iterator iter = m_listChildItemId.find(gdbId);
    if(iter == m_listChildItemId.end())
        return;

    IDebugger* dbgr = DoGetDebugger();
    CHECK_PTR_RET(dbgr);

    wxTreeItemId item = iter->second;
    m_listChildItemId.erase(iter);

    if(event.m_userReason == m_LIST_CHILDS) {
        m_listTable->Begin();
        if(event.m_varObjChildren.empty() == false) {
            for(size_t i = 0; i < event.m_varObjChildren.size(); i++) {
                const VariableObjChild& ch = event.m_varObjChildren.at(i);
                if(ch.varName == "public" || ch.varName == "private" || ch.varName == "protected") {
                    // not really a node...
                    // ask for information about this node children
                    dbgr->ListChildren(ch.gdbId, m_LIST_CHILDS);
                    m_listChildItemId[ch.gdbId] = item;

                } else {

                    DbgTreeItemData* data = new DbgTreeItemData();
                    data->_gdbId = ch.gdbId;
                    data->_isFake = ch.isAFake;

                    wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);
                    m_listTable->SetItemText(child, ch.type, 2);

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
        m_listTable->Commit();
    }
}

void LocalsTable::OnVariableObjUpdate(const DebuggerEventData& event)
{
    VariableObjectUpdateInfo updateInfo = event.m_varObjUpdateInfo;

    // remove all obsolete items
    for(size_t i = 0; i < updateInfo.removeIds.GetCount(); i++) {
        wxString gdbId = updateInfo.removeIds.Item(i);
        wxTreeItemId item = DoFindItemByGdbId(gdbId);
        if(item.IsOk()) {
            DoDeleteWatch(item);
            m_listTable->Delete(item);
        }
    }

    // refresh the values of the items that requires that
    IDebugger* dbgr = DoGetDebugger();
    if(dbgr) {
        wxArrayString itemsToRefresh = event.m_varObjUpdateInfo.refreshIds;
        DoRefreshItemRecursively(dbgr, m_listTable->GetRootItem(), itemsToRefresh);
    }
}

void LocalsTable::OnItemExpanding(wxTreeEvent& event)
{
    wxBusyCursor bc;
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_listTable->GetFirstChild(event.GetItem(), cookie);

    IDebugger* dbgr = DoGetDebugger();
    if(!dbgr || !event.GetItem()) {
        // dont allow the expansion of this item
        event.Veto();
        return;
    }

    size_t childCount = m_listTable->GetChildrenCount(event.GetItem());
    if(childCount > 1) {
        // make sure there is no <dummy> node and continue
        wxTreeItemIdValue cookieOne;
        wxTreeItemId dummyItem = m_listTable->GetFirstChild(event.GetItem(), cookieOne);
        while(dummyItem.IsOk()) {
            if(m_listTable->GetItemText(dummyItem) == wxT("<dummy>")) {
                m_listTable->Delete(dummyItem);
                event.Skip();
                return;
            }
            dummyItem = m_listTable->GetNextChild(event.GetItem(), cookieOne);
        }

    } else if(child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {

        // a dummy node, replace it with the real node content
        m_listTable->Delete(child);

        wxString gdbId = DoGetGdbId(event.GetItem());
        if(gdbId.IsEmpty() == false) {
            dbgr->UpdateVariableObject(gdbId, m_DBG_USERR);
            dbgr->ListChildren(gdbId, m_LIST_CHILDS);
            m_listChildItemId[gdbId] = event.GetItem();

        } else {
            // first time
            // create a variable object
            wxString expres = m_listTable->GetItemText(event.GetItem());
            DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(event.GetItem());
            if(data && data->_kind == DbgTreeItemData::FuncRetValue) {
                expres = data->_retValueGdbValue;
            }

            dbgr->CreateVariableObject(expres,                                       // the expression
                                       data->_kind == DbgTreeItemData::FuncRetValue, // bound to creation frame?
                                       m_DBG_USERR);                                 // user reason
            m_createVarItemId[expres] = event.GetItem();
        }
    }
}

void LocalsTable::DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved, size_t flags,
                                                  std::map<wxString, wxString>& oldValues)
{
    wxTreeItemIdValue cookie;
    std::vector<wxTreeItemId> itemsToRemove;

    wxTreeItemId item = m_listTable->GetFirstChild(m_listTable->GetRootItem(), cookie);
    while(item.IsOk()) {
        wxString gdbId = DoGetGdbId(item);
        if(gdbId.IsEmpty()) {
            DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
            if(data && (data->_kind & flags)) {
                // not a variable object entry, remove it
                itemsToRemove.push_back(item);
            }

        } else {
            itemsNotRemoved.Add(m_listTable->GetItemText(item));
        }
        item = m_listTable->GetNextChild(m_listTable->GetRootItem(), cookie);
    }

    for(size_t i = 0; i < itemsToRemove.size(); i++) {
        oldValues[m_listTable->GetItemText(itemsToRemove[i])] = m_listTable->GetItemText(itemsToRemove[i], 1);
        m_listTable->Delete(itemsToRemove[i]);
    }
}

void LocalsTable::DoUpdateLocals(const LocalVariables& localsUnSorted, size_t kind)
{
    LocalVariables locals = localsUnSorted;
    wxTreeItemId root = m_listTable->GetRootItem();
    if(!root.IsOk())
        return;

    IDebugger* dbgr = DoGetDebugger();
    wxArrayString itemsNotRemoved;
    // remove the non-variable objects and return a list
    // of all the variable objects (at the top level)

    std::map<wxString, wxString> oldValues;
    DoClearNonVariableObjectEntries(itemsNotRemoved, kind, oldValues);
    for(size_t i = 0; i < locals.size(); i++) {

        // try to replace the
        wxString newVarName;
        if(m_resolveLocals) {
            newVarName = m_preDefTypes.GetPreDefinedTypeForTypename(locals[i].type, locals[i].name);
        }

        // Evaluate arrays as char*?
        static wxRegEx reConstArr(wxT("(const )?[ ]*(w)?char(_t)? *[\\[0-9\\]]*"));
        wxString type = locals[i].type;
        if(m_arrayAsCharPtr && reConstArr.Matches(type)) {
            // array
            newVarName.Clear();
            newVarName << wxT("(char*)") << locals[i].name;
        }

        if(newVarName.IsEmpty() == false && !newVarName.Contains(wxT("@"))) {
            wxTreeItemId treeItem = DoFindItemByExpression(newVarName);
            if(treeItem.IsOk()) {
                // an item with this expression already exists, skip it
                continue;
            }

            // replace the local with a variable object
            // but make sure we dont enter a duplicate item
            if(itemsNotRemoved.Index(newVarName) == wxNOT_FOUND) {
                // this type has a pre-defined type, use it instead

                // Mark this item as VariableObject so incase another call
                // is made to this function, it wont get deleted by mistake
                DbgTreeItemData* data = new DbgTreeItemData();
                data->_kind = DbgTreeItemData::VariableObject;
                wxTreeItemId item = m_listTable->AppendItem(root, newVarName, -1, -1, data);

                m_listTable->AppendItem(item, wxT("<dummy>"));
                m_listTable->Collapse(item);

                if(dbgr) {
                    dbgr->CreateVariableObject(newVarName, false, m_DBG_USERR);
                    m_createVarItemId[newVarName] = item;
                }
            }

        } else {

            if(itemsNotRemoved.Index(locals[i].name) == wxNOT_FOUND) {
                // New entry
                wxTreeItemId item = m_listTable->AppendItem(root, locals[i].name, -1, -1, new DbgTreeItemData());
                // m_listTable->SetItemTextColour(item, DrawingUtils::GetMenuTextColour());

                m_listTable->SetItemText(item, locals[i].value, 1);
                m_listTable->SetItemText(item, locals[i].type, 2);

                std::map<wxString, wxString>::iterator iter = oldValues.find(locals[i].name);
                if(iter != oldValues.end() && iter->second != locals[i].value) {
                    m_listTable->SetItemTextColour(item, *wxRED, 1);
                }

                m_listTable->AppendItem(item, wxT("<dummy>"));
                m_listTable->Collapse(item);
            }
        }
    }
}

void LocalsTable::UpdateFrameInfo()
{
    if(ManagerST::Get()->DbgGetCurrentFrameInfo().IsValid() &&
       ManagerST::Get()->DbgGetCurrentFrameInfo() != m_curStackInfo) {
        Clear();
        m_curStackInfo = ManagerST::Get()->DbgGetCurrentFrameInfo();
        clMainFrame::Get()->GetDebuggerPane()->GetDebugWindow()->AppendLine(wxString::Format(
            _("Current scope is now set to: \"%s\", depth: %d\n"), m_curStackInfo.func.c_str(), m_curStackInfo.depth));
    }
}

void LocalsTable::OnRefresh(wxCommandEvent& event)
{
    IDebugger* dbgr = DoGetDebugger();
    if(dbgr) {
        Clear();
        dbgr->QueryLocals();
    }
}

void LocalsTable::OnRefreshUI(wxUpdateUIEvent& event) { event.Enable(DoGetDebugger() != NULL); }

void LocalsTable::OnItemRightClick(wxTreeEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("Change_Value"), _("Change value..."), wxT(""), wxITEM_NORMAL);
    PopupMenu(&menu);
}

void LocalsTable::OnEditValue(wxCommandEvent& event)
{
    wxTreeItemId selectedItem = m_listTable->GetSelection();
    if(selectedItem.IsOk() == false)
        return;

    wxString itemPath = GetItemPath(selectedItem);
    wxString newValue =
        wxGetTextFromUser(wxString::Format(_("Insert new value for '%s':"), itemPath.c_str()), _("Edit expression"));
    if(newValue.IsEmpty())
        return;

    IDebugger* debugger = DoGetDebugger();
    if(debugger) {
        debugger->AssignValue(itemPath, newValue);

        // refresh the item
        DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(selectedItem);
        if(data && data->_gdbId.IsEmpty()) {
            m_listTable->Delete(selectedItem);
            debugger->QueryLocals();

        } else {
            debugger->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
        }
    }
}

void LocalsTable::OnEditValueUI(wxUpdateUIEvent& event)
{
    IDebugger* debugger = DoGetDebugger();
    wxTreeItemId selectedItem = m_listTable->GetSelection();
    if(selectedItem.IsOk() == false) {
        event.Enable(false);

    } else {
        DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(selectedItem);
        event.Enable(debugger && data && (data->_gdbId.IsEmpty() || !data->_isFake));
    }
}

void LocalsTable::UpdateFuncReturnValue(const wxString& retValueGdbId)
{
    wxTreeItemId root = m_listTable->GetRootItem();
    if(!root.IsOk())
        return;

    wxArrayString itemsNotRemoved;
    // remove the non-variable objects and return a list
    // of all the variable objects (at the top level)
    wxTreeItemId item = DoFindItemByExpression(wxT("Function Returned"));
    if(item.IsOk()) {
        DoDeleteWatch(item);
    }
    m_listTable->Delete(item);

    DbgTreeItemData* data = new DbgTreeItemData();
    data->_kind = DbgTreeItemData::FuncRetValue;
    data->_retValueGdbValue = retValueGdbId;

    item = m_listTable->AppendItem(root, wxT("Function Returned"), -1, -1, data);

    m_listTable->SetItemText(item, retValueGdbId, 1);
    m_listTable->SetItemText(item, wxT(""), 2);
    m_listTable->SetItemTextColour(item, *wxRED, 1);

    m_listTable->AppendItem(item, wxT("<dummy>"));
    m_listTable->Collapse(item);

    // fake an expanding
    wxTreeEvent evt;
    evt.SetItem(item);
    OnItemExpanding(evt);
}

void LocalsTable::OnStackSelected(clCommandEvent& event)
{
    event.Skip();
    Clear();
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning() && ManagerST::Get()->IsDebuggerViewVisible(wxGetTranslation(DebuggerPane::LOCALS))) {
        dbgr->QueryLocals();
    }
}

void LocalsTable::OnSortItems(wxCommandEvent& event)
{
    // Change the sorting type and refresh the view
    m_sortAsc = !m_sortAsc;
    // Update the sorting function
    SetSortingFunction();
    OnRefresh(event);
}

void LocalsTable::SetSortingFunction()
{
    // Should a be placed before b?
    clSortFunc_t func = [=](clRowEntry* a, clRowEntry* b) {
        if(m_sortAsc) {
            return (a->GetLabel().CmpNoCase(b->GetLabel()) < 0);
        } else {
            return (a->GetLabel().CmpNoCase(b->GetLabel()) > 0);
        }
    };
    m_listTable->SetSortFunction(func);
}
