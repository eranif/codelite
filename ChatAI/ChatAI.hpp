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
#include "LLMClientBase.hpp"
#include "cl_command_event.h"
#include "plugin.h"

#include <memory>

/// Return the log handle of this library
clModuleLogger& GetLogHandle();

// Helper macros to be used outside of this library
#define CHATAI_LOG (GetLogHandle())

#define CHATAI_DEBUG() LOG_DEBUG(CHATAI_LOG)
#define CHATAI_TRACE() LOG_TRACE(CHATAI_LOG)
#define CHATAI_ERROR() LOG_ERROR(CHATAI_LOG)
#define CHATAI_WARNING() LOG_WARNING(CHATAI_LOG)
#define CHATAI_SYSTEM() LOG_SYSTEM(CHATAI_LOG)

class ChatAI : public IPlugin
{
public:
    ChatAI(IManager* manager);
    ~ChatAI() override;
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void HookPopupMenu(wxMenu* menu, MenuType type) override;
    void UnPlug() override;
    ChatAIConfig& GetConfig() { return m_cli->GetConfig(); }
    std::shared_ptr<LLMClientBase> GetClient() { return m_cli; }

private:
    void OnShowChatWindow(wxCommandEvent& event);
    void OnIsLlmAvailable(clLLMEvent& event);
    void OnLlmRequest(clLLMEvent& event);
    void OnInitDone(wxCommandEvent& event);

    ChatAIWindow* m_chatWindow{nullptr};
    std::shared_ptr<LLMClientBase> m_cli;
};
