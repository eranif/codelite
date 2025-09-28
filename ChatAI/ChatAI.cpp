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
#include "ollama/helpers.hpp"

#include <wx/app.h> //wxInitialize/wxUnInitialize
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

// our logger object
INITIALISE_MODULE_LOG(CHATAI_LOG_HANDLER, "ChatAI", "chat_ai.log");
clModuleLogger& GetLogHandle() { return CHATAI_LOG_HANDLER(); }

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
} // namespace

// Allocate the code formatter on the heap, it will be freed by
// the application
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new ChatAI(manager); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor("Eran Ifrah");
    info.SetName("ChatAI");
    info.SetDescription(_("A built-in AI assistance"));
    info.SetVersion("v1.0");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

ChatAI::ChatAI(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("A built-in AI assistance");
    m_shortName = _("A built-in AI assistance");

    clKeyboardManager::Get()->AddAccelerator(_("Chat AI"),
                                             {
                                                 {"chatai_show_window", _("Show AI Chat Window"), "Ctrl-Shift-H"},
                                             });
    wxTheApp->Bind(wxEVT_MENU, &ChatAI::OnShowChatWindow, this, XRCID("chatai_show_window"));

    llm::Manager::GetInstance().GetConfig().Load();
    m_chatWindow = new ChatAIWindow(m_mgr->BookGet(PaneId::SIDE_BAR));
    m_mgr->BookAddPage(PaneId::SIDE_BAR, m_chatWindow, CHAT_AI_LABEL, "chat-bot");
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &ChatAI::OnInitDone, this);
}

ChatAI::~ChatAI() {}

void ChatAI::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &ChatAI::OnShowChatWindow, this, XRCID("chatai_show_window"));
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &ChatAI::OnInitDone, this);

    // before this plugin is un-plugged we must remove the tab we added
    if (!m_mgr->BookDeletePage(PaneId::SIDE_BAR, m_chatWindow)) {
        m_chatWindow->Destroy();
    }
    m_chatWindow = nullptr;
    llm::Manager::GetInstance().Restart();
}

void ChatAI::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void ChatAI::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }

void ChatAI::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(type);
    wxUnusedVar(menu);
}

void ChatAI::OnShowChatWindow(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clGetManager()->ShowManagementWindow(CHAT_AI_LABEL, true);
    m_chatWindow->GetStcInput()->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAI::OnInitDone(wxCommandEvent& event) { event.Skip(); }
