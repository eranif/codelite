//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2024 by Eran Ifrah
// file name            : ChatAI.h
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
#pragma once

#include "ChatAIWindow.hpp"
#include "ChatAIWindowFrame.hpp"
#include "imanager.h"

class WXDLLIMPEXP_SDK ChatAI : public wxEvtHandler
{
public:
    ChatAI();
    ~ChatAI() override;

    void DetachView(bool show_frame);
    void DockView();
    /**
     * @brief Displays the Chat AI window and optionally sends an initial prompt.
     *
     * This method makes sure the chat UI is visible. If a docked pane ID is present,
     * the associated frame is shown; otherwise the management window for the chat AI
     * is opened. The input control receives focus after the window becomes visible.
     * If @p prompt is not empty, the prompt is forwarded to the chat view via
     * {@code Chat()}.
     *
     * @param prompt The initial prompt to send to the chat view. An empty string
     *               results in no automatic chat message.
     */
    void ShowChatWindow(const wxString& prompt = wxEmptyString);

    void AppendTextAndStyle(const wxString& text);

private:
    void OnInitDone(wxCommandEvent& event);
    ChatAIWindow* GetChatWindow();

    ChatAIWindow* m_chatWindow{nullptr};
    ChatAIWindowFrame* m_chatWindowFrame{nullptr};
    std::optional<PaneId> m_dockedPaneId{std::nullopt};
};
