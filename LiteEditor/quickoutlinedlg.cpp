//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickoutlinedlg.cpp
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

#include "cl_editor.h"
#include "cpp_symbol_tree.h"
#include "drawingutils.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "quickoutlinedlg.h"
#include "windowattrmanager.h"

QuickOutlineDlg::QuickOutlineDlg(wxWindow* parent, int id, wxPoint pos, wxSize size, int style)
    : wxDialog(parent, id, wxEmptyString, pos, size, wxRESIZE_BORDER | wxCAPTION)
{
    const wxColour& bgColour = DrawingUtils::GetColours().GetBgColour();
    SetBackgroundColour(bgColour);
    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(dialogSizer);
    wxPanel* mainPanel = new wxPanel(this);
    mainPanel->SetBackgroundColour(bgColour);

    dialogSizer->Add(mainPanel, 1, wxEXPAND);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainPanel->SetSizer(mainSizer);

    // build the outline view
    m_treeOutline =
        new CppSymbolTree(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_ROW_LINES | wxTR_HIDE_ROOT);
    m_treeOutline->Bind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this);
    m_treeOutline->SetSymbolsImages(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    m_treeOutline->EnableStyle(wxTR_ENABLE_SEARCH, false);
    Connect(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED, wxCommandEventHandler(QuickOutlineDlg::OnItemSelected), NULL, this);

    m_searchCtrl =
        new wxTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_RICH | wxTE_PROCESS_ENTER);
    m_searchCtrl->Bind(wxEVT_TEXT, &QuickOutlineDlg::OnTextEntered, this);
    m_searchCtrl->Bind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this);
    m_searchCtrl->Bind(wxEVT_TEXT_ENTER, &QuickOutlineDlg::OnSearchEnter, this);
    mainSizer->Add(m_treeOutline, 1, wxEXPAND);
    mainSizer->Insert(0, m_searchCtrl, 0, wxEXPAND);
    SetName("QuickOutlineDlg");
    GetSizer()->Fit(this);
    Layout();
    ::clSetDialogBestSizeAndPosition(this);
}

QuickOutlineDlg::~QuickOutlineDlg() { m_treeOutline->Unbind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this); }

void QuickOutlineDlg::OnItemSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoHide();
}

void QuickOutlineDlg::OnTextEntered(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoFindNext();
}

void QuickOutlineDlg::OnSearchEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_treeOutline->ItemActivated();
    DoHide();
}

void QuickOutlineDlg::OnKeyDown(wxKeyEvent& e)
{
    e.Skip();
    if(e.GetEventObject() == m_searchCtrl) {
        bool hasFilter = !m_searchCtrl->GetValue().IsEmpty();
        if(e.GetKeyCode() == WXK_DOWN) {
            if(hasFilter) {
                DoFindNext();
            } else {
                m_treeOutline->LineDown();
            }
            e.Skip(false);
        } else if(e.GetKeyCode() == WXK_UP) {
            if(hasFilter) {
                DoFindPrev();
            } else {
                m_treeOutline->LineUp();
            }
            e.Skip(false);
        } else if(e.GetKeyCode() == WXK_PAGEDOWN) {
            m_treeOutline->PageDown();
            e.Skip(false);
        } else if(e.GetKeyCode() == WXK_PAGEUP) {
            m_treeOutline->PageUp();
            e.Skip(false);
        } else if(e.GetKeyCode() == WXK_ESCAPE) {
            if(m_searchCtrl->GetValue().IsEmpty()) {
                DoHide();
            } else {
                m_treeOutline->ClearAllHighlights();
                m_searchCtrl->ChangeValue("");
            }
        }
    } else {
        if(e.GetKeyCode() == WXK_ESCAPE) {
            DoHide();
        }
    }
}

bool QuickOutlineDlg::ParseActiveBuffer()
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(!editor) {
        return false;
    }

    wxString filename = editor->GetFileName().GetFullPath();
    TagEntryPtrVector_t tags;
    if(!TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags)) {
        // Parse and update the cache
        tags = TagsManagerST::Get()->ParseBuffer(editor->GetCtrl()->GetText(), editor->GetFileName().GetFullPath());
        TagsManagerST::Get()->GetFileCache()->Update(editor->GetFileName(), tags);
    }

    if(tags.empty()) {
        return false;
    }
    m_treeOutline->BuildTree(filename, tags);
    m_treeOutline->ExpandAll();
    wxTreeItemId selectItem = m_treeOutline->GetNextItem(m_treeOutline->GetRootItem());
    m_treeOutline->SelectItem(selectItem);
    m_searchCtrl->CallAfter(&wxTextCtrl::SetFocus);
    return true;
}

void QuickOutlineDlg::DoFindNext()
{
    m_treeOutline->ClearAllHighlights();
    wxString find_what = m_searchCtrl->GetValue();
    if(find_what.empty()) {
        return;
    }

    wxTreeItemId focusedItem = m_treeOutline->GetFocusedItem();
    CHECK_ITEM_RET(focusedItem);

    wxTreeItemId nextItem = m_treeOutline->GetNextItem(focusedItem);
    if(!nextItem.IsOk()) {
        nextItem = focusedItem;
    }

    wxTreeItemId item = m_treeOutline->FindNext(nextItem, find_what, 0, wxTR_SEARCH_DEFAULT);
    // if we fail to move on, use the current focused item
    if(!item.IsOk()) {
        item = m_treeOutline->FindPrev(focusedItem, find_what, 0, wxTR_SEARCH_DEFAULT);
    }
    if(item.IsOk()) {
        m_treeOutline->SelectItem(item);
        m_treeOutline->HighlightText(item, true);
        m_treeOutline->EnsureVisible(item);
    }
}

void QuickOutlineDlg::DoFindPrev()
{
    m_treeOutline->ClearAllHighlights();
    wxString find_what = m_searchCtrl->GetValue();
    if(find_what.empty()) {
        return;
    }

    wxTreeItemId focusedItem = m_treeOutline->GetFocusedItem();
    CHECK_ITEM_RET(focusedItem);

    wxTreeItemId prevItem = m_treeOutline->GetPrevItem(focusedItem);
    if(!prevItem.IsOk()) {
        prevItem = focusedItem;
    }

    wxTreeItemId item = m_treeOutline->FindPrev(prevItem, find_what, 0, wxTR_SEARCH_DEFAULT);
    // if we fail to move on, use the current focused item
    if(!item.IsOk()) {
        item = m_treeOutline->FindPrev(focusedItem, find_what, 0, wxTR_SEARCH_DEFAULT);
    }
    if(item.IsOk()) {
        m_treeOutline->SelectItem(item);
        m_treeOutline->HighlightText(item, true);
        m_treeOutline->EnsureVisible(item);
    }
}

void QuickOutlineDlg::DoHide()
{
    EndModal(wxID_OK);
    Hide();
    m_searchCtrl->Clear();
    m_treeOutline->DeleteAllItems();
}
