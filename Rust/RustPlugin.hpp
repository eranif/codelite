//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2021 by Eran Ifrah
// file name            : RustPlugin.hpp
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

#ifndef __RUSTPLUGIN_HPP__
#define __RUSTPLUGIN_HPP__

#include "clFileSystemEvent.h"
#include "clTabTogglerHelper.h"
#include "cl_command_event.h"
#include "macros.h"
#include "plugin.h"

class RustPlugin : public IPlugin
{
public:
    RustPlugin(IManager* manager);
    virtual ~RustPlugin();

protected:
    void OnFolderContextMenu(clContextMenuEvent& event);
    void OnRustWorkspaceFileCreated(clFileSystemEvent& event);
    void OnNewWorkspace(clCommandEvent& e);
    wxString GetRustTool(const wxString& toolname) const;

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
    IManager* GetManager() { return m_mgr; }
};

#endif // __RUSTPLUGIN_HPP__
