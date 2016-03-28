//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBTooltip.cpp
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

#include "LLDBTooltip.h"
#include "LLDBPlugin.h"
#include "macros.h"
#include "event_notifier.h"
#include <wx/wupdlock.h>
#include "cl_config.h"
#include "globals.h"

LLDBTooltip::LLDBTooltip(LLDBPlugin* plugin)
    : clResizableTooltip(plugin)
    , m_plugin(plugin)
{
    MSWSetNativeTheme(m_treeCtrl);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBTooltip::OnLLDBVariableExpanded, this);
}

LLDBTooltip::~LLDBTooltip()
{
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBTooltip::OnLLDBVariableExpanded, this);
}

void LLDBTooltip::OnItemExpanding(wxTreeEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    LLDBVariableClientData* data = ItemData(event.GetItem());

    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(event.GetItem(), cookie);
    if(m_treeCtrl->GetItemText(child) == "<dummy>") {

        // dummy item, remove it
        m_treeCtrl->DeleteChildren(event.GetItem());
        m_plugin->GetLLDB()->RequestVariableChildren(data->GetVariable()->GetLldbId());

        // Store the treeitemid parent in cache
        m_itemsPendingExpansion.insert(std::make_pair(data->GetVariable()->GetLldbId(), event.GetItem()));

    } else {
        event.Skip();
    }
}

void LLDBTooltip::Show(const wxString& displayName, LLDBVariable::Ptr_t variable)
{
    DoCleanup();
    wxTreeItemId item = m_treeCtrl->AddRoot(
        variable->ToString(displayName), wxNOT_FOUND, wxNOT_FOUND, new LLDBVariableClientData(variable));
    if(variable->HasChildren()) {
        m_treeCtrl->AppendItem(item, "<dummy>");
    }
    clResizableTooltip::ShowTip();
}

void LLDBTooltip::DoCleanup()
{
    m_treeCtrl->DeleteAllItems();
    m_itemsPendingExpansion.clear();
}

LLDBVariableClientData* LLDBTooltip::ItemData(const wxTreeItemId& item) const
{
    return dynamic_cast<LLDBVariableClientData*>(m_treeCtrl->GetItemData(item));
}

void LLDBTooltip::OnLLDBVariableExpanded(LLDBEvent& event)
{
    int variableId = event.GetVariableId();
    wxUnusedVar(variableId);
    std::map<int, wxTreeItemId>::iterator iter = m_itemsPendingExpansion.find(event.GetVariableId());
    if(iter == m_itemsPendingExpansion.end()) {
        // does not belong to us
        event.Skip();
        return;
    }

    wxTreeItemId parentItem = iter->second;

    // add the variables to the tree
    for(size_t i = 0; i < event.GetVariables().size(); ++i) {
        DoAddVariable(parentItem, event.GetVariables().at(i));
    }

    // Expand the parent item
    if(m_treeCtrl->HasChildren(parentItem)) {
        m_treeCtrl->Expand(parentItem);
    }

    // remove it
    m_itemsPendingExpansion.erase(iter);
}

void LLDBTooltip::DoAddVariable(const wxTreeItemId& parent, LLDBVariable::Ptr_t variable)
{
    wxTreeItemId item = m_treeCtrl->AppendItem(
        parent, variable->ToString(), wxNOT_FOUND, wxNOT_FOUND, new LLDBVariableClientData(variable));
    if(variable->HasChildren()) {
        m_treeCtrl->AppendItem(item, "<dummy>");
    }
}
