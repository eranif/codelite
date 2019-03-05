//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : workspacesvnsettings.cpp
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

#include "workspacesvnsettings.h"
#include "cl_standard_paths.h"
#include "workspace.h"
#include "cl_config.h"

WorkspaceSvnSettings::WorkspaceSvnSettings()
    : clConfigItem("svn-settings")
{
}

WorkspaceSvnSettings::WorkspaceSvnSettings(const wxFileName& fn)
    : clConfigItem("svn-settings")
    , m_workspaceFileName(fn)
{
}

WorkspaceSvnSettings::~WorkspaceSvnSettings()
{
}

void WorkspaceSvnSettings::FromJSON(const JSONItem& json)
{
    m_repoPath = json.namedObject("m_repoPath").toString();
}

JSONItem WorkspaceSvnSettings::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("m_repoPath", m_repoPath);
    return json;
}

WorkspaceSvnSettings& WorkspaceSvnSettings::Load()
{
    clConfig conf(GetLocalConfigFile().GetFullPath());
    conf.ReadItem( this );
    return *this;
}

void WorkspaceSvnSettings::Save()
{
    clConfig conf(GetLocalConfigFile().GetFullPath());
    conf.WriteItem( this );
}

wxFileName WorkspaceSvnSettings::GetLocalConfigFile() const
{
    if ( m_workspaceFileName.IsOk() && m_workspaceFileName.Exists() ) {
        wxFileName fn(m_workspaceFileName.GetPath(), "subversion.conf");
        fn.AppendDir(".codelite");
        return fn;

    } else {
        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "subversion.conf");
        fn.AppendDir("config");
        return fn;
        
    }
}


