//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatter.cpp
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
#include "ChatAI.hpp"

#include "ai/LLMManager.hpp"
#include "codelite_events.h"
#include "event_notifier.h"

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
} // namespace

ChatAI::ChatAI()
{
    llm::Manager::GetInstance().GetConfig().Load();
    m_chatWindowFrame = new ChatAIWindowFrame(EventNotifier::Get()->TopFrame(), this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &ChatAI::OnInitDone, this);
}

ChatAI::~ChatAI() { bool is_docked = m_dockedPaneId.has_value(); }

void ChatAI::ShowChatWindow(const wxString& prompt)
{
    ChatAIWindow* chat_view = GetChatWindow();
    if (!prompt.empty()) {
        chat_view->Chat(prompt);
    }
}

void ChatAI::OnInitDone(wxCommandEvent& event) { event.Skip(); }

ChatAIWindow* ChatAI::GetChatWindow()
{
    if (!m_chatWindowFrame->IsShown()) {
        m_chatWindowFrame->Show();
    }
    return m_chatWindowFrame->GetChatWindow();
}

void ChatAI::AppendTextAndStyle(const wxString& text)
{
    auto chat_window = GetChatWindow();
    chat_window->AppendText(text, true);
}
