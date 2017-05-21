//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2017 Anders Jenbo
// File name            : phpcs.h
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

#ifndef __phpcs__
#define __phpcs__

#include "plugin.h"

class phpcs : public IPlugin
{
    std::list<wxFileName> m_queue;
    wxString m_output;
    IProcess* m_process;
    wxString m_currentFileBeingProcessed;

protected:
    void DoProcessQueue();
    void DoCheckFile(const wxFileName& filename);

public:
    phpcs(IManager *manager);
    ~phpcs();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();

    void OnCheck(wxCommandEvent& e);
    void PhpCSDone(const wxString& lintOutput, const wxString& filename);
    void OnFileAction(clCommandEvent& e);
    void OnProcessTerminated(clProcessEvent &event);
    void OnProcessOutput(clProcessEvent &event);
};

#endif //phpcs
