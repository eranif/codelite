#include "text_editor.h"
#include <event_notifier.h>

const wxEventType wxEVT_CMD_TEXT_EDITOR = ::wxNewEventType();

TextEditor::TextEditor(wxWindow* parent)
    : TextEditorBaseClass(parent)
{
    Hide();
}

TextEditor::~TextEditor() {}

void TextEditor::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_ESCAPE) {
        Hide();
    } else {
        event.Skip();
    }
}

void TextEditor::OnKillFocus(wxFocusEvent& event)
{
    event.Skip();
    Hide();
    wxCommandEvent evt(wxEVT_CMD_TEXT_EDITOR);
    evt.SetString(m_textCtrl->GetValue());
    EventNotifier::Get()->AddPendingEvent(evt);
}

void TextEditor::OnTextEnter(wxCommandEvent& event)
{
    Hide();
    wxCommandEvent evt(wxEVT_CMD_TEXT_EDITOR);
    evt.SetString(m_textCtrl->GetValue());
    EventNotifier::Get()->AddPendingEvent(evt);
}

void TextEditor::SetValue(const wxString& value) { m_textCtrl->ChangeValue(value); }

void TextEditor::MoveTo(const wxRect& rect)
{
    wxSize sz = rect.GetSize();
    sz.IncBy(1, 1);
    SetSize(sz);
    Move(rect.GetTopLeft());
    Show();
    m_textCtrl->SelectAll();
    m_textCtrl->SetFocus();
}
