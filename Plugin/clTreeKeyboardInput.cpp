#include "clTreeKeyboardInput.h"
#include "fileutils.h"
#include "macros.h"
#include <algorithm>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/uiaction.h>

clTreeKeyboardInput::clTreeKeyboardInput(clTreeCtrl* tree)
    : m_tree(tree)
{
    m_tree->EnableStyle(wxTR_ENABLE_SEARCH, true);
    m_tree->Bind(wxEVT_TREE_SEARCH_TEXT, &clTreeKeyboardInput::OnSearch, this);
    m_tree->Bind(wxEVT_TREE_CLEAR_SEARCH, &clTreeKeyboardInput::OnClearSearch, this);
    m_tree->Bind(wxEVT_TREE_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
}

clTreeKeyboardInput::~clTreeKeyboardInput()
{
    m_tree->Unbind(wxEVT_TREE_SEARCH_TEXT, &clTreeKeyboardInput::OnSearch, this);
    m_tree->Unbind(wxEVT_TREE_CLEAR_SEARCH, &clTreeKeyboardInput::OnClearSearch, this);
    m_tree->Unbind(wxEVT_TREE_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
}

void clTreeKeyboardInput::OnSearch(wxTreeEvent& event)
{
    wxString find_what = event.GetString();
    m_tree->ClearAllHighlights();
    wxTreeItemId item = m_tree->FindNext(m_tree->GetFocusedItem(), find_what, 0, wxTR_SEARCH_DEFAULT);
    if(item.IsOk()) {
        m_tree->SelectItem(item);
        m_tree->HighlightText(item, true);
        m_tree->EnsureVisible(item);
    } else {
        m_tree->ClearAllHighlights();
        m_tree->ClearFindWhat();
    }
}

void clTreeKeyboardInput::OnClearSearch(wxTreeEvent& event) { m_tree->ClearAllHighlights(); }

void clTreeKeyboardInput::OnKeyDown(wxTreeEvent& event)
{
    if(event.GetKeyCode() == WXK_ESCAPE) {
        if(m_tree->GetFindWhat().IsEmpty()) {
            event.Skip();
        } else {
            // First time, just clear the highlights
            m_tree->ClearFindWhat();
            m_tree->ClearAllHighlights();
        }
    } else if((event.GetKeyCode() == WXK_DOWN) && !m_tree->GetFindWhat().IsEmpty()) {
        // Trigger continue search
        m_tree->ClearAllHighlights();
        wxTreeItemId item = m_tree->FindNext(m_tree->GetFocusedItem(), m_tree->GetFindWhat(), 0,
                                             wxTR_SEARCH_DEFAULT & ~wxTR_SEARCH_INCLUDE_CURRENT_ITEM);
        if(item.IsOk()) {
            m_tree->SelectItem(item);
            m_tree->HighlightText(item, true);
            m_tree->EnsureVisible(item);
        } else {
            m_tree->ClearAllHighlights();
            m_tree->ClearFindWhat();
            event.Skip();
        }
    } else if((event.GetKeyCode() == WXK_UP) && !m_tree->GetFindWhat().IsEmpty()) {
        // Trigger continue search
        m_tree->ClearAllHighlights();
        wxTreeItemId item = m_tree->FindPrev(m_tree->GetFocusedItem(), m_tree->GetFindWhat(), 0,
                                             wxTR_SEARCH_DEFAULT & ~wxTR_SEARCH_INCLUDE_CURRENT_ITEM);
        if(item.IsOk()) {
            m_tree->SelectItem(item);
            m_tree->HighlightText(item, true);
            m_tree->EnsureVisible(item);
        } else {
            m_tree->ClearAllHighlights();
            m_tree->ClearFindWhat();
            event.Skip();
        }
    } else {
        event.Skip();
    }
}
