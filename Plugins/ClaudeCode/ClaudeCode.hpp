//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2026 The CodeLite Team
// File name            : ClaudeCode.hpp
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

#include "ClaudeCodePage.hpp"
#include "plugin.h"

class wxTerminalEvent;

class ClaudeCode : public IPlugin
{
public:
    ClaudeCode(IManager* manager);
    ~ClaudeCode() override;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    void CreatePluginMenu(wxMenu* pluginsMenu) override;

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    void UnPlug() override;

protected:
    void OnSettings(wxCommandEvent& event);
    void ShowClaudeTerminal();
    void OnPageClosing(wxNotifyEvent& event);
    void OnShowClaudeCode(wxCommandEvent& event);
    void OnAllPagesClosed(wxCommandEvent& event);
    void OnPageChanged(wxCommandEvent& event);
    void OnBlinkTimer(wxTimerEvent& event);

    /// The terminal rang the bell. Claude Code does this when it needs the user (a question, a
    /// permission prompt or a completed task).
    void OnTerminalBell(wxTerminalEvent& event);
    void OnTerminalLink(wxTerminalEvent& event);

    /// The user is looking at the terminal again, clear the attention state.
    void OnTerminalFocus(wxFocusEvent& event);

    /// Start / stop blinking the tab label of the Claude Code terminal.
    void StartAttentionBlink();
    void StopAttentionBlink();

    /// Apply `m_tabTitle` to the tab, with the attention marker if we are blinking.
    void UpdateTabLabel();

    /// Return true if the Claude Code tab is the selected tab and the main frame is active.
    bool IsClaudeTerminalVisible() const;

    ClaudeCodePage* m_claudeCodePage{nullptr};
    std::shared_ptr<std::function<void()>> m_showClaudeCode;

    /// The tab label without the attention marker.
    wxString m_tabTitle;
    wxTimer m_blinkTimer;
    bool m_needsAttention{false};
    bool m_blinkOn{false};
};
