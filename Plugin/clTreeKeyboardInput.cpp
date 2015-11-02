#include "clTreeKeyboardInput.h"
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include "macros.h"
#include "fileutils.h"
#include <algorithm>

clTreeKeyboardInput::clTreeKeyboardInput(wxTreeCtrl* tree, eSearchFlags flags)
    : m_tree(tree)
    , m_flags(flags)
{
    m_tree->Bind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
    m_text = new wxTextCtrl(m_tree, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_text->Hide();
    m_text->Bind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnTextKeyDown, this);
    m_text->Bind(wxEVT_COMMAND_TEXT_UPDATED, &clTreeKeyboardInput::OnTextUpdated, this);
    m_text->Bind(wxEVT_COMMAND_TEXT_ENTER, &clTreeKeyboardInput::OnTextEnter, this);
    m_tree->Bind(wxEVT_SET_FOCUS, &clTreeKeyboardInput::OnTreeFocus, this);
}

clTreeKeyboardInput::~clTreeKeyboardInput()
{
    m_tree->Unbind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
    m_text->Unbind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnTextKeyDown, this);
    m_text->Unbind(wxEVT_COMMAND_TEXT_UPDATED, &clTreeKeyboardInput::OnTextUpdated, this);
    m_text->Unbind(wxEVT_COMMAND_TEXT_ENTER, &clTreeKeyboardInput::OnTextEnter, this);
    m_tree->Unbind(wxEVT_SET_FOCUS, &clTreeKeyboardInput::OnTreeFocus, this);
}

void clTreeKeyboardInput::OnKeyDown(wxKeyEvent& event)
{
    event.Skip(false);
    wxChar ch = event.GetKeyCode();
    if(ch == WXK_UP || ch == WXK_DOWN || ch == WXK_LEFT || ch == WXK_RIGHT || ch == WXK_RETURN ||
       ch == WXK_NUMPAD_ENTER || ch == WXK_NUMPAD_SUBTRACT || ch == WXK_NUMPAD_MULTIPLY || ch == WXK_CONTROL ||
       ch == WXK_COMMAND || ch == WXK_SHIFT) {
        event.Skip();
        return;
    }

    if(!m_text->IsShown()) {
        DoShowTextBox();
    }

    if(wxIsprint(ch)) {
        m_text->ChangeValue(wxString() << ch);
        m_text->SetInsertionPoint(m_text->GetLastPosition());
    }
    CallAfter(&clTreeKeyboardInput::SetTextFocus);
}

void clTreeKeyboardInput::OnTextKeyDown(wxKeyEvent& event)
{
    event.Skip();
    wxChar ch = event.GetKeyCode();
    if(ch == WXK_ESCAPE) {
        event.Skip(false);
        Clear();
        m_tree->CallAfter(&wxTreeCtrl::SetFocus);
    }
}

void clTreeKeyboardInput::OnTextUpdated(wxCommandEvent& event)
{
    GetChildren();

    auto iter = m_items.begin();
    for(; iter != m_items.end(); ++iter) {
        wxString text = m_tree->GetItemText(*iter);
        if(FileUtils::FuzzyMatch(m_text->GetValue(), text)) {
            CallAfter(&clTreeKeyboardInput::SelecteItem, (*iter));
            return;
        }
    }
}

void clTreeKeyboardInput::SelecteItem(const wxTreeItemId& item)
{
    m_tree->UnselectAll();
    m_tree->EnsureVisible(item);
    m_tree->SetFocusedItem(item);
    m_tree->SelectItem(item);

    // Adjust the text box position and size
    DoShowTextBox();
}

void clTreeKeyboardInput::OnTreeFocus(wxFocusEvent& event)
{
    event.Skip();
    // The tree got the focus. Hide the text control if any
    if(m_text->IsShown()) {
        Clear();
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

void clTreeKeyboardInput::SetTextFocus()
{
    m_text->SetFocus();
    // Remove the selection
    m_text->SelectNone();
    m_tree->UnselectAll();
}

void clTreeKeyboardInput::OnTextEnter(wxCommandEvent& event)
{
    // Emulate "Item Activated" event
    wxTreeItemId item = m_tree->GetFocusedItem();
    CHECK_ITEM_RET(item);

    wxTreeEvent activateEvent(wxEVT_TREE_ITEM_ACTIVATED);
    activateEvent.SetEventObject(m_tree);
    activateEvent.SetItem(item);
    m_tree->GetEventHandler()->AddPendingEvent(activateEvent);

    // Hide the text control
    Clear();
}

void clTreeKeyboardInput::GetChildren(const wxTreeItemId& from)
{
    m_items.clear();
    wxTreeItemId startItem = m_tree->GetRootItem();
    if(startItem.IsOk() && m_tree->ItemHasChildren(startItem)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(startItem, cookie);
        while(child.IsOk()) {
            DoGetChildren(child);
            child = m_tree->GetNextChild(startItem, cookie);
        }
    }

    if(from.IsOk()) {
        std::list<wxTreeItemId> items;
        auto iter =
            std::find_if(m_items.begin(), m_items.end(), [&](const wxTreeItemId& item) { return item == from; });
        if(iter != m_items.end()) {
            // we got a match
            items.insert(items.end(), iter, m_items.end());
            m_items.swap(items);
        }
    }
}

void clTreeKeyboardInput::DoGetChildren(const wxTreeItemId& parent)
{
    m_items.push_back(parent);
    if(m_tree->ItemHasChildren(parent) && m_tree->IsExpanded(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(parent, cookie);
        while(child.IsOk()) {
            DoGetChildren(child);
            child = m_tree->GetNextChild(parent, cookie);
        }
    }
}

void clTreeKeyboardInput::Clear()
{
    m_text->ChangeValue("");
    m_text->Hide();
    m_items.clear();
}

void clTreeKeyboardInput::DoShowTextBox()
{
    wxSize sz = m_text->GetSize();
    sz.x = m_tree->GetClientRect().GetWidth();
    wxPoint pt = m_tree->GetClientRect().GetBottomLeft();
    pt.y -= sz.GetHeight();
    m_text->SetSize(sz);
    m_text->Move(pt);
    if(!m_text->IsShown()) {
        m_text->Show();
        m_text->ChangeValue("");
        m_tree->UnselectAll();
        m_items.clear();
    }
}
