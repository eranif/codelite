//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : sftp_workspace_settings.cpp
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

#include "sftp_workspace_settings.h"

SFTPWorkspaceSettings::SFTPWorkspaceSettings()
    : clConfigItem("sftp-workspace-settings")
{
}

SFTPWorkspaceSettings::~SFTPWorkspaceSettings() {}

void SFTPWorkspaceSettings::FromJSON(const JSONItem& json)
{
    m_remoteWorkspacePath = json.namedObject("m_remoteWorkspacePath").toString();
    m_account = json.namedObject("m_account").toString();
}

JSONItem SFTPWorkspaceSettings::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_account", m_account);
    element.addProperty("m_remoteWorkspacePath", m_remoteWorkspacePath);
    return element;
}

void SFTPWorkspaceSettings::Load(SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile)
{
    wxFileName fn(workspaceFile);
    if(fn.IsOk() && fn.FileExists()) {
        fn.SetName("sftp-workspace-settings");
        fn.SetExt("conf");
        fn.AppendDir(".codelite");
        clConfig config(fn.GetFullPath());
        config.ReadItem(&settings);
    }
}

void SFTPWorkspaceSettings::Save(const SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile)
{
    wxFileName fn(workspaceFile);
    fn.SetName("sftp-workspace-settings");
    fn.SetExt("conf");
    fn.AppendDir(".codelite");
    clConfig config(fn.GetFullPath());
    config.WriteItem(&settings);
}

void SFTPWorkspaceSettings::Clear()
{
    m_account.Clear();
    m_remoteWorkspacePath.Clear();
}
