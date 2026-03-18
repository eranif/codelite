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
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

ChatAI::ChatAI()
{
    m_dock_chat = clConfig::Get().Read(CHAT_AI_DOCKED, m_dock_chat);
    llm::Manager::GetInstance().GetConfig().Load();

    if (m_dock_chat) {
        m_chatWindow = new ChatAIWindow(clGetManager()->BookGet(PaneId::BOTTOM_BAR));
        clGetManager()->BookAddPage(PaneId::BOTTOM_BAR, m_chatWindow, CHAT_AI_LABEL);
    } else {
        m_chatWindowFrame = new ChatAIWindowFrame(EventNotifier::Get()->TopFrame(), this);
        m_chatWindow = m_chatWindowFrame->GetChatWindow();
    }
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &ChatAI::OnInitDone, this);
}

ChatAI::~ChatAI() { EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &ChatAI::OnInitDone, this); }

void ChatAI::ShowChatWindow(const wxString& prompt)
{
    EnsureVisible();
    if (!prompt.empty()) {
        m_chatWindow->Chat(prompt);
    }
    m_chatWindow->GetStcInput()->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAI::OnInitDone(wxCommandEvent& event) { event.Skip(); }

void ChatAI::EnsureVisible()
{
    if (m_dock_chat) {
        clGetManager()->ShowManagementWindow(CHAT_AI_LABEL, true);
    } else {
        if (!m_chatWindowFrame->IsShown()) {
            m_chatWindowFrame->Show();
        }
    }
}

ChatAIWindow* ChatAI::GetChatWindow()
{
    EnsureVisible();
    return m_chatWindow;
}

void ChatAI::AppendTextAndStyle(const wxString& text)
{
    auto chat_window = GetChatWindow();
    chat_window->AppendText(text, true);
}
