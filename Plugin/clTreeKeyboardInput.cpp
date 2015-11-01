#include "clTreeKeyboardInput.h"
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include "macros.h"
#include "fileutils.h"

clTreeKeyboardInput::clTreeKeyboardInput(wxTreeCtrl* tree, eSearchFlags flags)
    : m_tree(tree)
    , m_flags(flags)
{
    m_tree->Bind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
    m_text = new wxTextCtrl(m_tree, wxID_ANY);
    m_text->Hide();
    m_text->Bind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnTextKeyDown, this);
    m_text->Bind(wxEVT_COMMAND_TEXT_UPDATED, &clTreeKeyboardInput::OnTextUpdated, this);
    m_tree->Bind(wxEVT_SET_FOCUS, &clTreeKeyboardInput::OnTreeFocus, this);
}

clTreeKeyboardInput::~clTreeKeyboardInput()
{
    m_tree->Unbind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
    m_text->Unbind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnTextKeyDown, this);
    m_text->Unbind(wxEVT_COMMAND_TEXT_UPDATED, &clTreeKeyboardInput::OnTextUpdated, this);
    m_tree->Unbind(wxEVT_SET_FOCUS, &clTreeKeyboardInput::OnTreeFocus, this);
}

void clTreeKeyboardInput::OnKeyDown(wxKeyEvent& event)
{
    event.Skip(false);
    wxChar ch = event.GetKeyCode();
    if(ch == WXK_UP || ch == WXK_DOWN || ch == WXK_LEFT || ch == WXK_RIGHT || ch == WXK_RETURN ||
       ch == WXK_NUMPAD_ENTER || ch == WXK_NUMPAD_SUBTRACT || ch == WXK_NUMPAD_MULTIPLY) {
        event.Skip();
        return;
    }

    if(!m_text->IsShown()) {
        wxSize sz = m_text->GetSize();
        sz.x = m_tree->GetClientRect().GetWidth();
        wxPoint pt = m_tree->GetClientRect().GetBottomLeft();
        pt.y -= sz.GetHeight();
        m_text->SetSize(sz);
        m_text->Move(pt);
        m_text->Show();
    }

    m_text->EmulateKeyPress(event);
    m_text->CallAfter(&wxTextCtrl::SetFocus);
}

void clTreeKeyboardInput::OnTextKeyDown(wxKeyEvent& event)
{
    event.Skip();
    wxChar ch = event.GetKeyCode();
    if(ch == WXK_ESCAPE) {
        event.Skip(false);
        m_text->ChangeValue("");
        m_text->Hide();
    }
}

void clTreeKeyboardInput::OnTextUpdated(wxCommandEvent& event)
{
    wxTreeItemId startItem = m_tree->GetRootItem();
    CHECK_ITEM_RET(startItem);
    
    if(startItem.IsOk() && m_tree->ItemHasChildren(startItem)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(startItem, cookie);
        while(child.IsOk()) {
            child = m_tree->GetNextChild(startItem, cookie);
        }
    }
    CheckItemForMatch(startItem);
}

void clTreeKeyboardInput::SelecteItem(const wxTreeItemId& item)
{
    m_tree->UnselectAll();
    m_tree->EnsureVisible(item);
    m_tree->SetFocusedItem(item);
    m_tree->SelectItem(item);
}

void clTreeKeyboardInput::OnTreeFocus(wxFocusEvent& event)
{
    event.Skip();
    // The tree got the focus. Hide the text control if any
    if(m_text->IsShown()) {
        m_text->ChangeValue("");
        m_text->Hide();
    }
}

bool clTreeKeyboardInput::CheckItemForMatch(const wxTreeItemId& startItem)
{
    // First we check the current item
    {
        wxString text = m_tree->GetItemText(startItem);
        if(FileUtils::FuzzyMatch(m_text->GetValue(), text)) {
            // select this item
            CallAfter(&clTreeKeyboardInput::SelecteItem, startItem);
            return true;
        }
    }

    // Check the item children if the item has children and only
    // if the item is expanded
    if(m_tree->ItemHasChildren(startItem) && m_tree->IsExpanded(startItem)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(startItem, cookie);
        while(child.IsOk()) {
            if(CheckItemForMatch(child)) {
                return true;
            }
            child = m_tree->GetNextChild(startItem, cookie);
        }
    }

    // Check the item siblings (we go down)
    wxTreeItemId item = m_tree->GetNextSibling(startItem);
    while(item.IsOk()) {
        if(CheckItemForMatch(item)) {
            return true;
        }
        item = m_tree->GetNextSibling(item);
    }
    return false;
}
