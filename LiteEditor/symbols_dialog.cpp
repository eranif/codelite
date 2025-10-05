//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : symbols_dialog.cpp
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
#include "symbols_dialog.h"
#include "manager.h"
#include "macros.h"
#include "globals.h"
#include "windowattrmanager.h"

BEGIN_EVENT_TABLE(SymbolsDialog, SymbolsDialogBase)
EVT_CHAR_HOOK(SymbolsDialog::OnCharHook)
END_EVENT_TABLE()

SymbolsDialog::SymbolsDialog(wxWindow* parent)
    : SymbolsDialogBase(parent)
    , m_line(wxNOT_FOUND)
    , m_file(wxEmptyString)
    , m_selectedItem(wxNOT_FOUND)
{
    // Initialise the list control
    m_results->InsertColumn(0, _("Symbol"));
    m_results->InsertColumn(1, _("Kind"));
    m_results->InsertColumn(2, _("File"));
    m_results->InsertColumn(3, _("Line"));
    m_results->InsertColumn(4, _("Pattern"));

    m_results->Connect(
        wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(SymbolsDialog::OnItemDeselected), NULL, this);
    CentreOnParent();
    SetName("SymbolsDialog");
    WindowAttrManager::Load(this);
}

void SymbolsDialog::AddSymbol(const TagEntryPtr& tag, bool sel)
{
    //-------------------------------------------------------
    // Populate the columns
    //-------------------------------------------------------

    wxString line;
    line << tag->GetLine();

    long index = AppendListCtrlRow(m_results);
    SetColumnText(m_results, index, 0, tag->GetFullDisplayName());
    SetColumnText(m_results, index, 1, tag->GetKind());
    SetColumnText(m_results, index, 2, tag->GetFile());
    SetColumnText(m_results, index, 3, line);
    SetColumnText(m_results, index, 4, tag->GetPattern());

    // list ctrl can reorder items, so use returned index to insert tag
    m_tags.insert(m_tags.begin() + index, tag);
}

void SymbolsDialog::AddSymbols(const std::vector<TagEntryPtr>& tags, size_t sel)
{
    wxUnusedVar(sel);
    for(size_t i = 0; i < tags.size(); i++) {
        AddSymbol(tags[i], false);
    }
    m_results->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(1, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(2, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(3, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(4, wxLIST_AUTOSIZE);

    m_results->SetFocus();
    if(tags.empty() == false) {
        m_results->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_results->SetItemState(0, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        m_tag = m_tags[m_selectedItem = 0];
    }
}

void SymbolsDialog::UpdateFileAndLine(wxListEvent& event)
{
    wxString line_number;
    m_file = GetColumnText(m_results, event.m_itemIndex, 2);
    line_number = GetColumnText(m_results, event.m_itemIndex, 3);
    m_pattern = GetColumnText(m_results, event.m_itemIndex, 4);
    line_number.ToLong(&m_line);
}

void SymbolsDialog::OnItemSelected(wxListEvent& event)
{
    UpdateFileAndLine(event);
    m_tag = m_tags[m_selectedItem = event.m_itemIndex];
}

void SymbolsDialog::OnItemActivated(wxListEvent& event)
{
    UpdateFileAndLine(event);
    EndModal(wxID_OK);
}

void SymbolsDialog::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}

void SymbolsDialog::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}

void SymbolsDialog::OnItemDeselected(wxListEvent& event)
{
    wxUnusedVar(event);
    m_selectedItem = wxNOT_FOUND;
    m_tag = NULL;
}

void SymbolsDialog::OnCharHook(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_DOWN) {

        if(m_selectedItem == wxNOT_FOUND && m_results->GetItemCount() > 0) {
            m_tag = m_tags[m_selectedItem = 0];
        }

        if(m_selectedItem == wxNOT_FOUND) return;

        if(m_results->GetItemCount() > m_selectedItem + 1) {
            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_DROPHILITED);
            m_tag = m_tags[++m_selectedItem];

            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_results->EnsureVisible(m_selectedItem);
            return;
        }
    } else if(event.GetKeyCode() == WXK_UP) {
        if(m_selectedItem == wxNOT_FOUND && m_results->GetItemCount() > 0) {
            m_tag = m_tags[m_selectedItem = 0];
        }

        if(m_selectedItem == wxNOT_FOUND) return;

        // select the previous one if we can
        if((m_selectedItem - 1) >= 0) {
            // we can select the next one
            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_DROPHILITED);
            m_tag = m_tags[--m_selectedItem];

            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_results->EnsureVisible(m_selectedItem);
        }
        return;
    }
    event.Skip();
}
