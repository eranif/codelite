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

#include "Keyboard/clKeyboardManager.h"
#include "ai/LLMManager.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "macromanager.h"

#include <wx/app.h> //wxInitialize/wxUnInitialize
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
constexpr const char* kConfigIsViewDetached = "chat_ai.detached_view";
} // namespace

ChatAI::ChatAI()
{
    llm::Manager::GetInstance().GetConfig().Load();
    m_chatWindowFrame = new ChatAIWindowFrame(EventNotifier::Get()->TopFrame(), this);
    m_chatWindow = new ChatAIWindow(clGetManager()->BookGet(PaneId::SIDE_BAR), this);
    clGetManager()->BookAddPage(PaneId::SIDE_BAR, m_chatWindow, CHAT_AI_LABEL, "chat-bot");
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &ChatAI::OnInitDone, this);
}

void ChatAI::ShowChatWindow()
{
    if (m_dockedPaneId.has_value()) {
        m_chatWindowFrame->Show();
    } else {
        clGetManager()->ShowManagementWindow(CHAT_AI_LABEL, true);
        m_chatWindow->GetStcInput()->CallAfter(&wxStyledTextCtrl::SetFocus);
    }
}

void ChatAI::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    bool detached = clConfig::Get().Read(kConfigIsViewDetached, false);
    if (detached) {
        DetachView(false);
    }
}

void ChatAI::DetachView(bool show_frame)
{
    auto res = clGetManager()->FindPaneId(m_chatWindow);
    if (res.has_value()) {
        // First, remember the pane from which we are removing our view.
        m_dockedPaneId = res;
        clGetManager()->BookRemovePage(m_dockedPaneId.value(), m_chatWindow);
    }

    if (show_frame && !m_chatWindowFrame->IsShown()) {
        m_chatWindowFrame->Show();
    }
}

void ChatAI::DockView()
{
    if (m_chatWindowFrame->IsShown()) {
        m_chatWindowFrame->Hide();
    }

    auto res = clGetManager()->FindPaneId(m_chatWindow);
    if (res.has_value()) {
        // the view is already docked
        return;
    }

    PaneId pane_id = m_dockedPaneId.value_or(PaneId::SIDE_BAR);
    clGetManager()->BookAddPage(pane_id, m_chatWindow, CHAT_AI_LABEL, "chat-bot");
    clGetManager()->BookSelectPage(pane_id, m_chatWindow);
    m_dockedPaneId.reset();
}
