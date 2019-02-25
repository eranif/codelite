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
#include "globals.h"
#include <algorithm>
#include <wx/textdlg.h>
#include <wx/treelist.h>
#include <wx/wupdlock.h>
#include "LLDBProtocol/LLDBFormat.h"

#define LOCALS_VIEW_NAME_COL_IDX 0
#define LOCALS_VIEW_SUMMARY_COL_IDX 1
#define LOCALS_VIEW_VALUE_COL_IDX 2
#define LOCALS_VIEW_TYPE_COL_IDX 3

namespace
{
const int lldbLocalsViewEditValueMenuId = XRCID("lldb_locals_view_edit_value");
const int lldbLocalsViewAddWatchContextMenuId = XRCID("lldb_locals_view_add_watch");
const int lldbLocalsViewRemoveWatchContextMenuId = XRCID("lldb_locals_view_remove_watch");
} // namespace

LLDBLocalsView::LLDBLocalsView(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBLocalsViewBase(parent)
    , m_plugin(plugin)
{
    m_treeList = new clThemedTreeCtrl(this);

    m_treeList->AddHeader(_("Name"));
    m_treeList->AddHeader(_("Summary"));
    m_treeList->AddHeader(_("Value"));
    m_treeList->AddHeader(_("Type"));
    m_treeList->AddRoot(_("Local Variables"));
    GetSizer()->Add(m_treeList, 1, wxEXPAND | wxALL, 0);

    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_EXITED, &LLDBLocalsView::OnLLDBExited, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_LOCALS_UPDATED, &LLDBLocalsView::OnLLDBLocalsUpdated, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBLocalsView::OnLLDBVariableExpanded, this);

    m_treeList->Bind(wxEVT_COMMAND_TREE_ITEM_EXPANDING, &LLDBLocalsView::OnItemExpanding, this);
    m_treeList->Bind(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, &LLDBLocalsView::OnItemCollapsed, this);
    m_treeList->Bind(wxEVT_COMMAND_TREE_ITEM_MENU, &LLDBLocalsView::OnLocalsContextMenu, this);
    m_treeList->Bind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &LLDBLocalsView::OnBeginDrag, this);
    m_treeList->Bind(wxEVT_COMMAND_TREE_END_DRAG, &LLDBLocalsView::OnEndDrag, this);
    m_treeList->Bind(wxEVT_COMMAND_TREE_KEY_DOWN, &LLDBLocalsView::OnKeyDown, this);

    // Construct the toolbar
    m_toolbar->AddTool(wxID_NEW, _("New"), clGetManager()->GetStdIcons()->LoadBitmap("file_new"));
    m_toolbar->AddTool(wxID_DELETE, _("Delete"), clGetManager()->GetStdIcons()->LoadBitmap("clean"));
    m_toolbar->Bind(wxEVT_TOOL, &LLDBLocalsView::OnNewWatch, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &LLDBLocalsView::OnDelete, this, wxID_DELETE);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &LLDBLocalsView::OnDeleteUI, this, wxID_DELETE);
    m_toolbar->Realize();

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
    m_treeList->Unbind(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, &LLDBLocalsView::OnItemCollapsed, this);
    m_treeList->Unbind(wxEVT_COMMAND_TREE_ITEM_MENU, &LLDBLocalsView::OnLocalsContextMenu, this);
    m_treeList->Unbind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &LLDBLocalsView::OnBeginDrag, this);
    m_treeList->Unbind(wxEVT_COMMAND_TREE_END_DRAG, &LLDBLocalsView::OnEndDrag, this);
    m_treeList->Unbind(wxEVT_COMMAND_TREE_KEY_DOWN, &LLDBLocalsView::OnKeyDown, this);
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
    // FIXME: optimize this to only retrieve the top level variables
    // the children should be obtained in the 'OnItemExpading' event handler
    event.Skip();
    wxWindowUpdateLocker locker(m_treeList);
    Enable(true);

    m_pendingExpandItems.clear();
    m_treeList->DeleteChildren(m_treeList->GetRootItem());
    m_pathToItem.clear();
    m_dragItem.Unset();

    clDEBUG() << "Updating locals view" << clEndl;
    DoAddVariableToView(event.GetVariables(), m_treeList->GetRootItem());

    // Re-expand all the items
    ExpandPreviouslyExpandedItems();
}

void LLDBLocalsView::DoAddVariableToView(const LLDBVariable::Vect_t& variables, wxTreeItemId parent)
{
    for(size_t i = 0; i < variables.size(); ++i) {
        LLDBVariable::Ptr_t variable = variables.at(i);
        LLDBVariableClientData* cd = new LLDBVariableClientData(variable);
        wxTreeItemId item = m_treeList->AppendItem(parent, variable->GetName(), wxNOT_FOUND, wxNOT_FOUND, cd);
        m_treeList->SetItemText(item, variable->GetSummary().IsEmpty() ? variable->GetValue() : variable->GetSummary(),
                                LOCALS_VIEW_SUMMARY_COL_IDX);
        cd->SetPath(GetItemPath(item));

        // Keep a map between the 'path'->wxTreeItemId
        m_pathToItem.erase(cd->GetPath());
        m_pathToItem.emplace(cd->GetPath(), item);

        m_treeList->SetItemText(item, variable->GetValue(), LOCALS_VIEW_VALUE_COL_IDX);
        m_treeList->SetItemText(item, variable->GetType(), LOCALS_VIEW_TYPE_COL_IDX);
        if(variable->IsValueChanged()) { m_treeList->SetItemTextColour(item, "RED"); }
        if(variable->HasChildren()) {
            // insert dummy item here
            m_treeList->AppendItem(item, "<dummy>");
        }
    }
    if(!variables.empty()) { m_treeList->Expand(parent); }
}

void LLDBLocalsView::ExpandPreviouslyExpandedItems()
{
    for(const auto& path : m_expandedItems) {
        const auto iter = m_pathToItem.find(path);
        if(iter != m_pathToItem.end()) {
            const auto& itemToExpand = iter->second;

            if(m_treeList->HasChildren(itemToExpand) && !m_treeList->IsExpanded(itemToExpand)) {
                m_treeList->Expand(itemToExpand);
            }
        }
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
            if(m_pendingExpandItems.insert(std::make_pair(variableId, event.GetItem())).second) {
                m_plugin->GetLLDB()->RequestVariableChildren(variableId);
            }
        }

    } else {
        event.Skip();
    }
}

void LLDBLocalsView::OnItemCollapsed(wxTreeEvent& event)
{
    event.Skip();

    const auto cd = GetItemData(event.GetItem());
    if(cd) {
        m_expandedItems.erase(cd->GetPath());
        const auto variable = cd->GetVariable();
        if(variable) { m_pendingExpandItems.erase(variable->GetLldbId()); }
    }
}

LLDBVariableClientData* LLDBLocalsView::GetItemData(const wxTreeItemId& id) const
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

    // Might be able to expand more previously expanded items now.
    ExpandPreviouslyExpandedItems();

    LLDBVariableClientData* cd = GetItemData(parentItem);
    if(cd) { m_expandedItems.insert(cd->GetPath()); }
}

void LLDBLocalsView::OnNewWatch(wxCommandEvent& event)
{
    wxString watch = ::wxGetTextFromUser(_("Expression to watch:"), _("Add New Watch"), "");
    if(watch.IsEmpty()) { return; }
    m_plugin->GetLLDB()->AddWatch(watch);

    // Refresh the locals view
    m_plugin->GetLLDB()->RequestLocals();
}

void LLDBLocalsView::OnDelete(wxCommandEvent& event) { DoDelete(); }

void LLDBLocalsView::OnDeleteUI(wxUpdateUIEvent& event)
{
    wxArrayTreeItemIds items;
    GetWatchesFromSelections(items);
    event.Enable(!items.IsEmpty());
}

void LLDBLocalsView::GetWatchesFromSelections(wxArrayTreeItemIds& items) const
{
    wxArrayTreeItemIds arr;
    m_treeList->GetSelections(arr);
    GetWatchesFromSelections(arr, items);
}

void LLDBLocalsView::GetWatchesFromSelections(const wxArrayTreeItemIds& selections, wxArrayTreeItemIds& items) const
{
    items.Clear();

    for(size_t i = 0; i < selections.GetCount(); ++i) {
        const auto item = selections.Item(i);
        const auto lldbVar = GetVariableFromItem(item);
        if(lldbVar && lldbVar->IsWatch()) { items.Add(item); }
    }
}

void LLDBLocalsView::OnLocalsContextMenu(wxTreeEvent& event)
{
    wxArrayTreeItemIds selections;
    m_treeList->GetSelections(selections);
    if(0 == selections.GetCount()) { return; }

    wxMenu menu;
    menu.Append(wxID_COPY, wxString::Format("Copy value%s to clipboard", ((1 == selections.GetCount()) ? "" : "s")));
    if(1 == selections.GetCount()) { menu.Append(lldbLocalsViewEditValueMenuId, _("Edit value")); }
    menu.Append(lldbLocalsViewAddWatchContextMenuId,
                wxString::Format("Add watch%s", ((1 == selections.GetCount()) ? "" : "es")));

    wxArrayTreeItemIds watches;
    GetWatchesFromSelections(watches);
    if(watches.GetCount()) {
        menu.Append(lldbLocalsViewRemoveWatchContextMenuId,
                    wxString::Format("Remove watch%s", ((1 == watches.GetCount()) ? "" : "es")));
    }

    wxMenu* pSubMenu = LLDBFormat::CreateMenu();
    menu.AppendSubMenu(pSubMenu, _("Display as"));

    const int selection = GetPopupMenuSelectionFromUser(menu);
    if(selection == wxID_COPY) {
        wxString content;
        wxArrayTreeItemIds arr;
        m_treeList->GetSelections(arr);
        for(size_t i = 0; i < arr.GetCount(); ++i) {
            wxString itemValue = m_treeList->GetItemText(arr.Item(i), LOCALS_VIEW_VALUE_COL_IDX);
            wxString itemSummary = m_treeList->GetItemText(arr.Item(i), LOCALS_VIEW_SUMMARY_COL_IDX);
            if(itemValue == itemSummary) { itemValue.clear(); }
            wxString itemValueCombined;

            // Combine the summary and value columns into one column
            if(!itemValue.IsEmpty()) { itemValueCombined << itemValue; }
            if(!itemSummary.IsEmpty()) {
                if(!itemValueCombined.IsEmpty()) { itemValueCombined << " "; }
                itemValueCombined << itemSummary;
            }

            if(!itemValueCombined.IsEmpty()) {
                if(!content.IsEmpty()) { content << "\n"; }
                content << itemValueCombined;
            }
        }
        if(!content.IsEmpty()) { ::CopyToClipboard(content); }
    } else if(selection == lldbLocalsViewEditValueMenuId) {
        EditVariable();
    } else if(selection == lldbLocalsViewAddWatchContextMenuId) {
        AddWatch();
    } else if(selection == lldbLocalsViewRemoveWatchContextMenuId) {
        DoDelete();
    } else if(LLDBFormat::GetFormatID(selection) != eLLDBFormat::kFormatInvalid) {
        SetVariableDisplayFormat(LLDBFormat::GetFormatID(selection));
    }
}

void LLDBLocalsView::AddWatch()
{
    bool shouldRefresh = false;

    wxArrayTreeItemIds arr;
    m_treeList->GetSelections(arr);

    for(size_t i = 0; i < arr.GetCount(); ++i) {
        const auto item = arr.Item(i);
        if(AddWatch(item)) { shouldRefresh = true; }
    }

    if(shouldRefresh) { m_plugin->GetLLDB()->RequestLocals(); }
}

bool LLDBLocalsView::AddWatch(const wxTreeItemId& item)
{
    const auto lldbVar = GetVariableFromItem(item);
    if(!lldbVar) { return false; }

    m_plugin->GetLLDB()->AddWatch(lldbVar->GetExpression());
    return true;
}

wxString LLDBLocalsView::GetItemPath(const wxTreeItemId& item)
{
    wxString path;
    wxTreeItemId currentItem = item;
    while(currentItem.IsOk()) {
        LLDBVariableClientData* cd = GetItemData(currentItem);
        currentItem = m_treeList->GetItemParent(currentItem);
        if(cd) {
            if(!path.IsEmpty()) { path.Prepend("."); }
            path.Prepend(cd->GetVariable()->GetName());
        }
    }
    return path;
}

bool LLDBLocalsView::DoDelete()
{
    bool shouldRefresh = false;
    wxArrayTreeItemIds items;
    GetWatchesFromSelections(items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        const auto lldbVar = GetVariableFromItem(items.Item(i));
        if(lldbVar && lldbVar->IsWatch()) {
            m_plugin->GetLLDB()->DeleteWatch(lldbVar->GetLldbId());
            shouldRefresh = true;
        }
    }

    if(shouldRefresh) { m_plugin->GetLLDB()->RequestLocals(); }

    return shouldRefresh;
}

void LLDBLocalsView::OnBeginDrag(wxTreeEvent& event) { m_dragItem = event.GetItem(); }

void LLDBLocalsView::OnEndDrag(wxTreeEvent& event)
{
    const auto dragItem = m_dragItem;
    m_dragItem.Unset();

    // Only allow dropping into empty space in order to create a watch (VS style).
    if(!event.GetItem().IsOk() && AddWatch(dragItem)) { m_plugin->GetLLDB()->RequestLocals(); }
}

bool LLDBLocalsView::EditVariable()
{
    wxArrayTreeItemIds selections;
    m_treeList->GetSelections(selections);
    if(1 != selections.GetCount()) { return false; }

    wxTreeItemId item = selections.Item(0);
    wxString currentValue = m_treeList->GetItemText(item, LOCALS_VIEW_VALUE_COL_IDX);
    wxString newValue = ::wxGetTextFromUser(_("New value:"), _("Edit"), "");
    if(newValue.IsEmpty()) { return false; }

    const auto lldbVar = GetVariableFromItem(item);
    if(!lldbVar) { return false; }
    m_plugin->GetLLDB()->SetVariableValue(lldbVar->GetLldbId(), newValue);
    return true;
}

void LLDBLocalsView::OnKeyDown(wxTreeEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_F2:
        if(EditVariable()) { return; }
        break;
    case WXK_DELETE:
        if(DoDelete()) { return; }
        break;

    default:
        break;
    }

    event.Skip();
}

void LLDBLocalsView::SetVariableDisplayFormat(const eLLDBFormat format)
{
    bool shouldRefresh = false;

    wxArrayTreeItemIds selections;
    m_treeList->GetSelections(selections);

    for(size_t i = 0; i < selections.GetCount(); ++i) {
        const auto lldbVar = GetVariableFromItem(selections.Item(i));
        if(lldbVar) {
            m_plugin->GetLLDB()->SetVariableDisplayFormat(lldbVar->GetLldbId(), format);
            shouldRefresh = true;
        }
    }

    if(shouldRefresh) { m_plugin->GetLLDB()->RequestLocals(); }
}

LLDBVariable::Ptr_t LLDBLocalsView::GetVariableFromItem(const wxTreeItemId& item) const
{
    if(!item.IsOk()) { return LLDBVariable::Ptr_t(); }

    const auto data = GetItemData(item);
    if(!data) { return LLDBVariable::Ptr_t(); }

    return data->GetVariable();
}
