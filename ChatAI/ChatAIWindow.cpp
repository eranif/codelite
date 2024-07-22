#include "ChatAIWindow.hpp"

#include "ColoursAndFontsManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

wxDEFINE_EVENT(wxEVT_CHATAI_SEND, wxCommandEvent);

ChatAIWindow::ChatAIWindow(wxWindow* parent)
    : AssistanceAIChatWindowBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
}

ChatAIWindow::~ChatAIWindow() {}

void ChatAIWindow::OnSend(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clCommandEvent sendEvent{ wxEVT_CHATAI_SEND };
    sendEvent.SetString(m_stcInput->GetText());
    EventNotifier::Get()->AddPendingEvent(sendEvent);
}

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event) { event.Enable(!m_stcInput->IsEmpty()); }

void ChatAIWindow::OnUpdateTheme(wxCommandEvent& event)
{
    event.Skip();
    UpdateTheme();
}

void ChatAIWindow::UpdateTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_PTR_RET(lexer);

    lexer->ApplySystemColours(m_stcInput);
    lexer->ApplySystemColours(m_stcOutput);
    m_stcInput->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);
}

void ChatAIWindow::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE) {
        clGetManager()->ToggleOutputPane();
        auto editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);

        // Set the focus to the active editor
        editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
    } else {
        event.Skip();
    }
}