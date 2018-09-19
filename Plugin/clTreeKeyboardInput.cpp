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
    //m_tree->Bind(wxEVT_TREE_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
    //m_text = new wxTextCtrl(m_tree->GetParent(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    //m_text->Hide();
    //m_text->Bind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnTextKeyDown, this);
    //m_text->Bind(wxEVT_COMMAND_TEXT_UPDATED, &clTreeKeyboardInput::OnTextUpdated, this);
    //m_text->Bind(wxEVT_COMMAND_TEXT_ENTER, &clTreeKeyboardInput::OnTextEnter, this);
    //m_tree->Bind(wxEVT_SET_FOCUS, &clTreeKeyboardInput::OnTreeFocus, this);
    //m_tree->Bind(wxEVT_SIZE, &clTreeKeyboardInput::OnTreeSize, this);
}

clTreeKeyboardInput::~clTreeKeyboardInput()
{
    //m_tree->Unbind(wxEVT_TREE_KEY_DOWN, &clTreeKeyboardInput::OnKeyDown, this);
    //m_text->Unbind(wxEVT_KEY_DOWN, &clTreeKeyboardInput::OnTextKeyDown, this);
    //m_text->Unbind(wxEVT_COMMAND_TEXT_UPDATED, &clTreeKeyboardInput::OnTextUpdated, this);
    //m_text->Unbind(wxEVT_COMMAND_TEXT_ENTER, &clTreeKeyboardInput::OnTextEnter, this);
    //m_tree->Unbind(wxEVT_SET_FOCUS, &clTreeKeyboardInput::OnTreeFocus, this);
    //m_tree->Unbind(wxEVT_SIZE, &clTreeKeyboardInput::OnTreeSize, this);
}

void clTreeKeyboardInput::OnKeyDown(wxTreeEvent& event)
{
    event.Skip(false);

    int ch = event.GetKeyCode();
    static std::set<int> ignoreKeys;
    if(ignoreKeys.empty()) {
        ignoreKeys.insert(WXK_UP);
        ignoreKeys.insert(WXK_DOWN);
        ignoreKeys.insert(WXK_LEFT);
        ignoreKeys.insert(WXK_RIGHT);
        ignoreKeys.insert(WXK_RETURN);
        ignoreKeys.insert(WXK_NUMPAD_ENTER);
        ignoreKeys.insert(WXK_NUMPAD_SUBTRACT);
        ignoreKeys.insert(WXK_NUMPAD_MULTIPLY);
        ignoreKeys.insert(WXK_CONTROL);
        ignoreKeys.insert(WXK_COMMAND);
        ignoreKeys.insert(WXK_SHIFT);
        ignoreKeys.insert(WXK_ESCAPE);
        ignoreKeys.insert(WXK_DELETE);
        ignoreKeys.insert(WXK_END);
        ignoreKeys.insert(WXK_INSERT);
        ignoreKeys.insert(WXK_HOME);
        ignoreKeys.insert(WXK_PAGEUP);
        ignoreKeys.insert(WXK_PAGEDOWN);
        ignoreKeys.insert(WXK_SCROLL);
        ignoreKeys.insert(WXK_PRINT);
        ignoreKeys.insert(WXK_PAUSE);
        ignoreKeys.insert(WXK_NUMLOCK);

        for(int i = WXK_NUMPAD_SPACE; i <= WXK_NUMPAD_DIVIDE; ++i) {
            ignoreKeys.insert(i);
        }

        for(int i = WXK_NUMPAD0; i <= WXK_NUMPAD9; ++i) {
            ignoreKeys.insert(i);
        }

        for(int i = WXK_F1; i <= WXK_F24; ++i) {
            ignoreKeys.insert(i);
        }
    }
    const wxKeyEvent& keyEvent = event.GetKeyEvent();
    if((keyEvent.GetModifiers() != wxMOD_NONE && keyEvent.GetModifiers() != wxMOD_SHIFT) || ignoreKeys.count(ch)) {
        event.Skip();
        return;
    }

    if(!m_text->IsShown()) { DoShowTextBox(); }

    CallAfter(&clTreeKeyboardInput::SetTextFocus);
    CallAfter(&clTreeKeyboardInput::SimulateKeyDown, event.GetKeyEvent());
}

void clTreeKeyboardInput::OnTextKeyDown(wxKeyEvent& event)
{
    event.Skip();
    wxChar ch = event.GetKeyCode();
    if(ch == WXK_ESCAPE) {
        event.Skip(false);
        Clear();
        m_tree->CallAfter(&clTreeCtrl::SetFocus);
    } else if(ch == WXK_DOWN || ch == WXK_UP) {
        event.Skip(false);
        wxTreeItemId match = FindItem(m_tree->GetSelection(), m_text->GetValue(), (ch == WXK_UP));
        if(match.IsOk()) { CallAfter(&clTreeKeyboardInput::SelecteItem, match); }
    }
}

void clTreeKeyboardInput::OnTextUpdated(wxCommandEvent& event)
{
    event.Skip();
    wxTreeItemId match = FindItem(m_tree->GetSelection(), m_text->GetValue(), false);
    if(match.IsOk()) { CallAfter(&clTreeKeyboardInput::SelecteItem, match); }
}

void clTreeKeyboardInput::SelecteItem(const wxTreeItemId& item)
{
    m_tree->SelectItem(item);
    m_tree->EnsureVisible(item);

    // Adjust the text box position and size
    DoShowTextBox();
}

void clTreeKeyboardInput::OnTreeFocus(wxFocusEvent& event)
{
    event.Skip();
    // The tree got the focus. Hide the text control if any
    if(m_text->IsShown()) { Clear(); }
}

void clTreeKeyboardInput::SetTextFocus()
{
    m_text->SetFocus();
    // Remove the selection
    m_text->SelectNone();
    m_text->SetInsertionPoint(m_text->GetLastPosition());
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

void clTreeKeyboardInput::Clear()
{
    m_text->ChangeValue("");
    m_text->Hide();
    m_items.clear();
}

void clTreeKeyboardInput::DoShowTextBox()
{
    wxSize sz = m_text->GetSize();
    sz.x = (m_tree->GetClientRect().GetWidth() / 2);
    m_text->SetSize(sz);
    wxPoint pt = m_tree->GetClientRect().GetTopLeft();
    pt.x += sz.x;
    m_text->Move(pt);
    if(!m_text->IsShown()) {
        m_text->Show();
        m_text->ChangeValue("");
        m_items.clear();
    }
}

void clTreeKeyboardInput::OnTreeSize(wxSizeEvent& event)
{
    event.Skip();
    if(m_text->IsShown()) {
        DoShowTextBox(); // Adjust the box size and position
    }
}

void clTreeKeyboardInput::SimulateKeyDown(const wxKeyEvent& event)
{
    // must be called after SetTextFocus() function to ensure that the simulation is running
    // on the input text control
    wxUIActionSimulator sim;
    sim.KeyDown(event.GetKeyCode(), event.GetModifiers());
    wxYield();
}

wxTreeItemId clTreeKeyboardInput::FindItem(const wxTreeItemId& from, const wxString& pattern, bool goingUp) const
{
    wxTreeItemId nextItem = goingUp ? m_tree->GetPrevItem(from) : m_tree->GetNextItem(from);
    while(nextItem.IsOk()) {
        if(FileUtils::FuzzyMatch(pattern, m_tree->GetItemText(nextItem))) { return nextItem; }
        nextItem = goingUp ? m_tree->GetPrevItem(nextItem) : m_tree->GetNextItem(nextItem);
    }
    return wxTreeItemId();
}
