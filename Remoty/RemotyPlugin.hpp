//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2021 by Eran Ifrah
// file name            : RemotyPlugin.hpp
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

#ifndef __REMOTYPLUGIN_HPP__
#define __REMOTYPLUGIN_HPP__

#include "clFileSystemEvent.h"
#include "clTabTogglerHelper.h"
#include "cl_command_event.h"
#include "macros.h"
#include "plugin.h"

class RemotyWorkspace;
class RemotyPlugin : public IPlugin
{
    RemotyWorkspace* m_workspace = nullptr;

public:
    RemotyPlugin(IManager* manager);
    virtual ~RemotyPlugin();

protected:
    void OnFolderContextMenu(clContextMenuEvent& event);
    void OnNewWorkspace(clCommandEvent& e);
    void OnRecentWorkspaces(clRecentWorkspaceEvent& event);
    void OnPluginOpenWorkspace(clWorkspaceEvent& event);

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
    IManager* GetManager() { return m_mgr; }
};

#endif // __RemotyPlugin_HPP__
