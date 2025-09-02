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
#include "OllamaClient.hpp"
#include "plugin.h"

class ChatAI : public IPlugin
{
public:
    ChatAI(IManager* manager);
    ~ChatAI() override;
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void HookPopupMenu(wxMenu* menu, MenuType type) override;
    void UnPlug() override;
    ChatAIConfig& GetConfig() { return m_cli.GetConfig(); }
    bool IsRunning() const;

private:
    void OnShowChatWindow(wxCommandEvent& event);
    void OnPrompt(clCommandEvent& event);
    void OnInterrupt(clCommandEvent& event);
    void OnStopLlamaCli(clCommandEvent& event);
    void OnStartLlamCli(clCommandEvent& event);
    void OnSettings(wxCommandEvent& event);

    ChatAIWindow* m_chatWindow = nullptr;
    OllamaClient m_cli;
};
