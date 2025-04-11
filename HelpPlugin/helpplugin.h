//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : helpplugin.h
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

#ifndef __HelpPlugin__
#define __HelpPlugin__

#include "plugin.h"
#include "cl_command_event.h"

class HelpPlugin : public IPlugin
{
protected:
    void OnEditorContextMenu(clContextMenuEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnHelpSettings(wxCommandEvent& event);
    
    void DoHelp(); // A proxy method so we can call the help async
    wxString DoBuildQueryString();
    
public:
    HelpPlugin(IManager* manager);
    ~HelpPlugin() override;

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
};

#endif // HelpPlugin
