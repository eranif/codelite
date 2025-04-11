//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : databaseexplorer.h
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

#ifndef __DatabaseExplorer__
#define __DatabaseExplorer__

#include "plugin.h"
#include "DbViewerPanel.h"
#include "cl_command_event.h"

class DatabaseExplorer : public IPlugin
{
public:
    DatabaseExplorer(IManager* manager);
    ~DatabaseExplorer() override;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void HookPopupMenu(wxMenu* menu, MenuType type) override;
    void UnPlug() override;

    static IManager* GetManager();
    static DbViewerPanel* GetViewerPanel() { return m_dbViewerPanel; }

protected:
    void OnAbout(wxCommandEvent& e);
    void OnOpenWithDBE(clCommandEvent& e);
    void DoOpenFile(const wxFileName& filename);
    void OnToggleTab(clCommandEvent& event);
    void OnExecuteSQL(wxCommandEvent& event);
    static DbViewerPanel* m_dbViewerPanel;
};

#endif // DatabaseExplorer
