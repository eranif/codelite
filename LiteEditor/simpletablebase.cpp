//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : simpletablebase.cpp
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "manager.h"
#include "plugin.h"
#include "simpletablebase.h"

///////////////////////////////////////////////////////////////////////////

DebuggerTreeListCtrlBase::DebuggerTreeListCtrlBase(wxWindow* parent, wxWindowID id, bool withButtonsPane,
                                                   const wxPoint& pos, const wxSize& size, long style)
    : LocalsTableBase(parent, id, pos, size, style)
    , m_withButtons(withButtonsPane)
{
    m_listTable->SetShowHeader(true);
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_REFRESH, _("Refresh"), images->Add("file_reload"));
    m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnRefresh, this, wxID_REFRESH);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DebuggerTreeListCtrlBase::OnRefreshUI, this, wxID_REFRESH);

    if(m_withButtons) {
        m_toolbar->AddTool(wxID_DELETE, _("Delete"), images->Add("clean"));
        m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnDeleteWatch, this, wxID_DELETE);
        m_toolbar->Bind(wxEVT_UPDATE_UI, &DebuggerTreeListCtrlBase::OnDeleteWatchUI, this, wxID_DELETE);

        m_toolbar->AddTool(wxID_NEW, _("New"), images->Add("file_new"));
        m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnNewWatch, this, wxID_NEW);
        m_toolbar->Bind(wxEVT_UPDATE_UI, &DebuggerTreeListCtrlBase::OnNewWatchUI, this, wxID_NEW);
    } else {
        m_toolbar->AddTool(wxID_SORT_ASCENDING, _("Sort"), images->Add("sort"));
        m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnSortItems, this, wxID_SORT_ASCENDING);
    }
    m_toolbar->Realize();
}

DebuggerTreeListCtrlBase::~DebuggerTreeListCtrlBase() {}

IDebugger* DebuggerTreeListCtrlBase::DoGetDebugger()
{
    if(!ManagerST::Get()->DbgCanInteract())
        return NULL;

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    return dbgr;
}

void DebuggerTreeListCtrlBase::DoResetItemColour(const wxTreeItemId& item, size_t itemKind)
{
    wxColour bgColour = wxNullColour;
    wxColour fgColour = wxNullColour;

    wxTreeItemIdValue cookieOne;
    wxTreeItemId child = m_listTable->GetFirstChild(item, cookieOne);
    while(child.IsOk()) {
        DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(child);

        bool resetColor = ((itemKind == 0) || (data && (data->_kind & itemKind)));
        if(resetColor) {
            m_listTable->SetItemTextColour(child, fgColour, 1);
        }

        m_listTable->SetItemBackgroundColour(child, bgColour, 1);

        if(m_listTable->HasChildren(child)) {
            DoResetItemColour(child, itemKind);
        }
        child = m_listTable->GetNextChild(item, cookieOne);
    }
}

void DebuggerTreeListCtrlBase::OnEvaluateVariableObj(const DebuggerEventData& event)
{
    wxString gdbId = event.m_expression;
    wxString value = event.m_evaluated;

    std::map<wxString, wxTreeItemId>::iterator iter = m_gdbIdToTreeId.find(gdbId);
    if(iter != m_gdbIdToTreeId.end()) {
        wxString newValue = value;
        wxString curValue = m_listTable->GetItemText(iter->second, 1);
        if(!(newValue == curValue || curValue.IsEmpty())) {
            m_listTable->SetItemTextColour(iter->second, *wxRED, 1);
        }
        m_listTable->SetItemText(iter->second, value, 1);

        // keep the red items IDs in the array
        m_gdbIdToTreeId.erase(iter);
    }
}

void DebuggerTreeListCtrlBase::DoRefreshItemRecursively(IDebugger* dbgr, const wxTreeItemId& item,
                                                        wxArrayString& itemsToRefresh)
{
    if(itemsToRefresh.IsEmpty())
        return;

    wxTreeItemIdValue cookieOne;
    wxTreeItemId exprItem = m_listTable->GetFirstChild(item, cookieOne);
    while(exprItem.IsOk()) {

        DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(exprItem));
        if(data) {
            int where = itemsToRefresh.Index(data->_gdbId);
            if(where != wxNOT_FOUND) {
                dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
                m_gdbIdToTreeId[data->_gdbId] = exprItem;
                itemsToRefresh.RemoveAt((size_t)where);
            }
        }

        if(m_listTable->HasChildren(exprItem)) {
            DoRefreshItemRecursively(dbgr, exprItem, itemsToRefresh);
        }
        exprItem = m_listTable->GetNextChild(item, cookieOne);
    }
}

void DebuggerTreeListCtrlBase::Clear()
{
    wxTreeItemId root = m_listTable->GetRootItem();
    if(root.IsOk()) {
        if(m_listTable->HasChildren(root)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);

            while(item.IsOk()) {
                DoDeleteWatch(item);
                item = m_listTable->GetNextChild(root, cookie);
            }

            m_listTable->DeleteChildren(root);
        }
    }

    m_listChildItemId.clear();
    m_createVarItemId.clear();
    m_gdbIdToTreeId.clear();
    m_curStackInfo.Clear();
}

void DebuggerTreeListCtrlBase::DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item, bool forceCreate)
{
    if(!dbgr || !item.IsOk())
        return;

    DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
    if(data && data->_gdbId.IsEmpty() == false) {
        dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
        m_gdbIdToTreeId[data->_gdbId] = item;

    } else if(data && forceCreate) {

        // try to re-create this variable object
        if(m_withButtons) {
            // HACK: m_withButton is set to true when we are in the context of
            // the 'Watches' table
            dbgr->CreateVariableObject(m_listTable->GetItemText(item), true, m_DBG_USERR);
        } else {
            dbgr->CreateVariableObject(m_listTable->GetItemText(item), false, m_DBG_USERR);
        }

        m_createVarItemId[m_listTable->GetItemText(item)] = item;
    }
}

wxString DebuggerTreeListCtrlBase::DoGetGdbId(const wxTreeItemId& item)
{
    wxString gdbId;
    if(!item.IsOk())
        return gdbId;

    DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(item);
    if(data) {
        return data->_gdbId;
    }
    return gdbId;
}

wxTreeItemId DebuggerTreeListCtrlBase::DoFindItemByGdbId(const wxString& gdbId)
{
    wxTreeItemId root = m_listTable->GetRootItem();
    wxTreeItemIdValue cookieOne;
    wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
    while(item.IsOk()) {

        wxString id = DoGetGdbId(item);
        if(id.IsEmpty() == false && id == gdbId)
            return item;

        item = m_listTable->GetNextChild(root, cookieOne);
    }
    return wxTreeItemId();
}

void DebuggerTreeListCtrlBase::DoDeleteWatch(const wxTreeItemId& item)
{
    IDebugger* dbgr = DoGetDebugger();
    if(!dbgr || !item.IsOk()) {
        return;
    }

    wxString gdbId = DoGetGdbId(item);
    if(gdbId.IsEmpty() == false) {
        dbgr->DeleteVariableObject(gdbId);
    }

#ifdef __WXMAC__

    // Mac's GDB does not delete all the children of the variable object
    // instead we will do it manually

    if(m_listTable->HasChildren(item)) {
        // Delete this item children
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_listTable->GetFirstChild(item, cookie);
        while(child.IsOk()) {
            gdbId = DoGetGdbId(child);
            if(gdbId.IsEmpty() == false) {
                dbgr->DeleteVariableObject(gdbId);
            }

            if(m_listTable->HasChildren(child)) {
                DoDeleteWatch(child);
            }

            child = m_listTable->GetNextChild(item, cookie);
        }
    }
#endif
}

wxTreeItemId DebuggerTreeListCtrlBase::DoFindItemByExpression(const wxString& expr)
{
    wxTreeItemId root = m_listTable->GetRootItem();
    wxTreeItemIdValue cookieOne;
    wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
    while(item.IsOk()) {

        if(m_listTable->GetItemText(item) == expr)
            return item;
        item = m_listTable->GetNextChild(root, cookieOne);
    }
    return wxTreeItemId();
}

void DebuggerTreeListCtrlBase::ResetTableColors() { DoResetItemColour(m_listTable->GetRootItem(), 0); }

wxString DebuggerTreeListCtrlBase::GetItemPath(const wxTreeItemId& item)
{
    wxArrayString pathArr;
    if(item.IsOk() == false)
        return wxT("");

    DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(item);
    if(data && data->_gdbId.IsEmpty()) {
        // not a variable object item
        return m_listTable->GetItemText(item);
    }

    wxTreeItemId parent = item;
    while(parent.IsOk() && m_listTable->GetRootItem() != parent) {
        DbgTreeItemData* itemData = (DbgTreeItemData*)m_listTable->GetItemData(parent);
        if(itemData && !itemData->_isFake) {
            pathArr.Add(m_listTable->GetItemText(parent));
        }
        parent = m_listTable->GetItemParent(parent);
    }

    if(pathArr.IsEmpty())
        return wxT("");

    wxString itemPath;
    for(int i = (int)pathArr.GetCount() - 1; i >= 0; i--) {
        itemPath << pathArr.Item(i) << wxT(".");
    }
    itemPath.RemoveLast();
    return itemPath;
}

void DebuggerTreeListCtrlBase::OnCreateVariableObjError(const DebuggerEventData& event)
{
    // failed to create a variable object!
    // remove this expression from the table
    wxTreeItemId root = m_listTable->GetRootItem();
    wxTreeItemIdValue cookieOne;
    wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
    while(item.IsOk()) {

        if(event.m_expression == m_listTable->GetItemText(item)) {
            m_listTable->Delete(item);
            break;
        }
        item = m_listTable->GetNextChild(root, cookieOne);
    }
}

void DebuggerTreeListCtrlBase::OnDeleteWatch(wxCommandEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnDeleteWatchUI(wxUpdateUIEvent& event) { event.Enable(!m_withButtons); }

void DebuggerTreeListCtrlBase::OnItemExpanding(wxTreeEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnItemRightClick(wxTreeEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnListEditLabelBegin(wxTreeEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnListEditLabelEnd(wxTreeEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnListKeyDown(wxTreeEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnNewWatch(wxCommandEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnNewWatchUI(wxUpdateUIEvent& event) { event.Enable(m_withButtons); }

void DebuggerTreeListCtrlBase::OnRefresh(wxCommandEvent& event) { event.Skip(); }

void DebuggerTreeListCtrlBase::OnRefreshUI(wxUpdateUIEvent& event) { event.Skip(); }
void DebuggerTreeListCtrlBase::OnSortItems(wxCommandEvent& event) {}
