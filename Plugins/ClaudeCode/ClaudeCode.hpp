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

#include "plugin.h"

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

    wxTerminalViewCtrl* m_claudeTerminal{nullptr};
    std::shared_ptr<std::function<void()>> m_showClaudeCode;
};
