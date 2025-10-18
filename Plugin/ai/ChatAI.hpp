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

#include <optional>

class WXDLLIMPEXP_SDK ChatAI : public wxEvtHandler
{
public:
    ChatAI();
    ~ChatAI() override = default;

    void DetachView(bool show_frame);
    void DockView();

private:
    void OnShowChatWindow(wxCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);

    ChatAIWindow* m_chatWindow{nullptr};
    ChatAIWindowFrame* m_chatWindowFrame{nullptr};
    std::optional<PaneId> m_dockedPaneId{std::nullopt};
};
