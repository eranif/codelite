//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBLocalsView.cpp
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

#include "LLDBLocalsView.h"
#include "LLDBPlugin.h"
#include "file_logger.h"
#include <wx/treelist.h>
#include <wx/wupdlock.h>
#include <wx/textdlg.h>
#include "globals.h"
#include <algorithm>

#define LOCALS_VIEW_NAME_COL_IDX 0
#define LOCALS_VIEW_SUMMARY_COL_IDX 1
#define LOCALS_VIEW_VALUE_COL_IDX 2
#define LOCALS_VIEW_TYPE_COL_IDX 3

LLDBLocalsView::LLDBLocalsView(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBLocalsViewBase(parent)
    , m_plugin(plugin)
{
    m_treeList = new clTreeListCtrl(this,
                                    wxID_ANY,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES |
                                        wxTR_ROW_LINES | wxTR_TWIST_BUTTONS | wxTR_MULTIPLE);
    m_treeList->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    m_treeList->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    m_treeList->AddColumn(_("Name"), 150);
    m_treeList->AddColumn(_("Summary"), 300);
    m_treeList->AddColumn(_("Value"), 300);
    m_treeList->AddColumn(_("Type"), 300);

    m_treeList->AddRoot(_("Local Variables"));
    GetSizer()->Add(m_treeList, 1, wxEXPAND | wxALL, 2);

    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_EXITED, &LLDBLocalsView::OnLLDBExited, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_LOCALS_UPDATED, &LLDBLocalsView::OnLLDBLocalsUpdated, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBLocalsView::OnLLDBVariableExpanded, this);

    m_treeList->Bind(wxEVT_COMMAND_TREE_ITEM_EXPANDING, &LLDBLocalsView::OnItemExpanding, this);
    m_treeList->Bind(wxEVT_CONTEXT_MENU, &LLDBLocalsView::OnLocalsContextMenu, this);
    GetSizer()->Layout();
}

LLDBLocalsView::~LLDBLocalsView()
{
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_EXITED, &LLDBLocalsView::OnLLDBExited, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_LOCALS_UPDATED, &LLDBLocalsView::OnLLDBLocalsUpdated, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBLocalsView::OnLLDBVariableExpanded, this);
    m_treeList->Unbind(wxEVT_COMMAND_TREE_ITEM_EXPANDING, &LLDBLocalsView::OnItemExpanding, this);
    m_treeList->Unbind(wxEVT_CONTEXT_MENU, &LLDBLocalsView::OnLocalsContextMenu, this);
}

void LLDBLocalsView::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    Cleanup();
    m_expandedItems.clear();
}

void LLDBLocalsView::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    Cleanup();
}

void LLDBLocalsView::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    Cleanup();
}

void LLDBLocalsView::OnLLDBLocalsUpdated(LLDBEvent& event)
{
    // FIXME: optimize this to only retrieve the top levle variables
    // the children should be obtained in the 'OnItemExpading' event handler
    event.Skip();
    wxWindowUpdateLocker locker(m_treeList);
    Enable(true);

    m_treeList->DeleteChildren(m_treeList->GetRootItem());
    m_pathToItem.clear();

    clDEBUG() << "Updating locals view" << clEndl;
    DoAddVariableToView(event.GetVariables(), m_treeList->GetRootItem());

    // Re-expand all the items
    std::for_each(m_expandedItems.begin(), m_expandedItems.end(), [&](const wxString& path) {
        // Locate the item ID
        if(m_pathToItem.count(path)) {
            const wxTreeItemId& itemToExpand = m_pathToItem[path];
            if(m_treeList->HasChildren(itemToExpand)) {
                m_treeList->Expand(itemToExpand);
            }
        }
    });
}

void LLDBLocalsView::DoAddVariableToView(const LLDBVariable::Vect_t& variables, wxTreeItemId parent)
{
    for(size_t i = 0; i < variables.size(); ++i) {
        LLDBVariable::Ptr_t variable = variables.at(i);
        LLDBVariableClientData* cd = new LLDBVariableClientData(variable);
        wxTreeItemId item = m_treeList->AppendItem(parent, variable->GetName(), wxNOT_FOUND, wxNOT_FOUND, cd);
        m_treeList->SetItemText(item,
                                LOCALS_VIEW_SUMMARY_COL_IDX,
                                variable->GetSummary().IsEmpty() ? variable->GetValue() : variable->GetSummary());
        cd->SetPath(GetItemPath(item));

        // Keep a map between the 'path'->wxTreeItemId
        if(m_pathToItem.count(cd->GetPath())) {
            m_pathToItem.erase(cd->GetPath());
        }
        m_pathToItem.insert(std::make_pair(cd->GetPath(), item));

        m_treeList->SetItemText(item, LOCALS_VIEW_VALUE_COL_IDX, variable->GetValue());
        m_treeList->SetItemText(item, LOCALS_VIEW_TYPE_COL_IDX, variable->GetType());
        if(variable->IsValueChanged()) {
            m_treeList->SetItemTextColour(item, "RED");
        }
        if(variable->HasChildren()) {
            // insert dummy item here
            m_treeList->AppendItem(item, "<dummy>");
        }
    }

    if(!variables.empty()) {
        m_treeList->Expand(parent);
    }
}

void LLDBLocalsView::OnItemExpanding(wxTreeEvent& event)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeList->GetFirstChild(event.GetItem(), cookie);
    if(m_treeList->GetItemText(child) == "<dummy>") {
        event.Veto();
        m_treeList->DeleteChildren(event.GetItem());

        // query the debugger about the children of this node
        if(m_plugin->GetLLDB()->IsCanInteract()) {
            int variableId = GetItemData(event.GetItem())->GetVariable()->GetLldbId();
            m_plugin->GetLLDB()->RequestVariableChildren(variableId);
            m_pendingExpandItems.insert(std::make_pair(variableId, event.GetItem()));
        }

    } else {
        event.Skip();
    }
}

LLDBVariableClientData* LLDBLocalsView::GetItemData(const wxTreeItemId& id)
{
    LLDBVariableClientData* cd = dynamic_cast<LLDBVariableClientData*>(m_treeList->GetItemData(id));
    return cd;
}

void LLDBLocalsView::Cleanup()
{
    m_treeList->DeleteChildren(m_treeList->GetRootItem());
    m_pendingExpandItems.clear();
    m_pathToItem.clear();
}

void LLDBLocalsView::OnLLDBVariableExpanded(LLDBEvent& event)
{
    int variableId = event.GetVariableId();
    // try to locate this item in our map
    LLDBLocalsView::IntItemMap_t::iterator iter = m_pendingExpandItems.find(variableId);
    if(iter == m_pendingExpandItems.end()) {
        // does not belong to us - skip it
        event.Skip();
        return;
    }

    // add the variables
    wxTreeItemId parentItem = iter->second;
    DoAddVariableToView(event.GetVariables(), parentItem);
    m_pendingExpandItems.erase(iter);

    LLDBVariableClientData* cd = GetItemData(parentItem);
    if(cd) {
        m_expandedItems.insert(cd->GetPath());
    }
}

void LLDBLocalsView::OnNewWatch(wxCommandEvent& event)
{
    wxString watch = ::wxGetTextFromUser(_("Expression to watch:"), _("Add New Watch"), "");
    if(watch.IsEmpty()) {
        return;
    }
    m_plugin->GetLLDB()->AddWatch(watch);

    // Refresh the locals view
    m_plugin->GetLLDB()->RequestLocals();
}

void LLDBLocalsView::OnDelete(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    GetWatchesFromSelections(items);
    if(items.IsEmpty()) return;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxTreeItemId item = items.Item(i);
        if(!GetItemData(item)) continue;
        LLDBVariableClientData* data = GetItemData(item);
        if(!data) continue;

        LLDBVariable::Ptr_t lldbVar = data->GetVariable();
        if(!lldbVar) continue;

        if(lldbVar->IsWatch()) {
            m_plugin->GetLLDB()->DeleteWatch(lldbVar->GetLldbId());
        }
    }
    // Refresh the locals view
    m_plugin->GetLLDB()->RequestLocals();
}

void LLDBLocalsView::OnDeleteUI(wxUpdateUIEvent& event)
{
    wxArrayTreeItemIds items;
    GetWatchesFromSelections(items);
    event.Enable(!items.IsEmpty());
}

void LLDBLocalsView::GetWatchesFromSelections(wxArrayTreeItemIds& items)
{
    items.Clear();
    wxArrayTreeItemIds arr;
    m_treeList->GetSelections(arr);

    for(size_t i = 0; i < arr.GetCount(); ++i) {
        wxTreeItemId item = arr.Item(i);
        if(!GetItemData(item)) continue;
        LLDBVariableClientData* data = GetItemData(item);
        if(!data) continue;

        LLDBVariable::Ptr_t lldbVar = data->GetVariable();
        if(!lldbVar) continue;

        if(lldbVar->IsWatch()) {
            items.Add(item);
        }
    }
}

void LLDBLocalsView::OnLocalsContextMenu(wxContextMenuEvent& event)
{

    wxMenu menu;
    menu.Append(wxID_COPY, _("Copy value to clipboard"));
    int selection = GetPopupMenuSelectionFromUser(menu);
    if(selection == wxID_COPY) {
        wxString content;
        wxArrayTreeItemIds arr;
        m_treeList->GetSelections(arr);
        for(size_t i = 0; i < arr.GetCount(); ++i) {
            wxString itemValue;
            wxTreeItemId item = arr.Item(i);
            if(!GetItemData(item)) continue;
            LLDBVariableClientData* data = GetItemData(item);
            if(!data) continue;
            LLDBVariable::Ptr_t lldbVar = data->GetVariable();
            if(!lldbVar) continue;
            if(!lldbVar->GetValue().IsEmpty()) {
                itemValue << lldbVar->GetValue();
            }

            if(!lldbVar->GetSummary().IsEmpty()) {
                if(!itemValue.IsEmpty()) itemValue << " ";
                itemValue << lldbVar->GetSummary();
            }

            if(!itemValue.IsEmpty()) {
                if(!content.IsEmpty()) {
                    content << "\n";
                }
                content << itemValue;
            }
        }

        if(!content.IsEmpty()) {
            ::CopyToClipboard(content);
        }
    }
}

wxString LLDBLocalsView::GetItemPath(const wxTreeItemId& item)
{
    wxString path;
    wxTreeItemId currentItem = item;
    while(currentItem.IsOk()) {
        LLDBVariableClientData* cd = GetItemData(currentItem);
        currentItem = m_treeList->GetItemParent(currentItem);
        if(cd) {
            if(!path.IsEmpty()) {
                path.Prepend(".");
            }
            path.Prepend(cd->GetVariable()->GetName());
        }
    }
    return path;
}
