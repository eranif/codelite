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
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "macros.h"
#include "plugin.h"
#include "wxStringHash.h"

#include <unordered_map>

class RustPlugin : public IPlugin
{
    std::unordered_map<wxString, wxString> m_cargoTomlDigest;
    wxFileName m_cargoTomlFile;

public:
    RustPlugin(IManager* manager);
    virtual ~RustPlugin();

protected:
    void OnFolderContextMenu(clContextMenuEvent& event);
    void OnRustWorkspaceFileCreated(clFileSystemEvent& event);
    void OnNewWorkspace(clCommandEvent& e);
    wxString GetRustTool(const wxString& toolname) const;
    void OnBuildErrorLineClicked(clBuildEvent& event);
    void AddRustcCompilerIfMissing();
    void OnBuildEnded(clBuildEvent& event);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

    wxFileName find_cargo_toml(const wxFileName& workspaceFile) const;

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
