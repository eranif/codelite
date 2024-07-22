#include "ChatAIWindow.hpp"

#include "ColoursAndFontsManager.h"
#include "codelite_events.h"
#include "event_notifier.h"

wxDEFINE_EVENT(wxEVT_CHATAI_SEND, wxCommandEvent);

ChatAIWindow::ChatAIWindow(wxWindow* parent)
    : AssistanceAIChatWindowBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
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

    lexer->Apply(m_stcInput);
    lexer->Apply(m_stcOutput);
    m_stcInput->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);
}
