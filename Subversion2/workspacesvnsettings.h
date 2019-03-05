//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : workspacesvnsettings.h
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

#ifndef WORKSPACESVNSETTINGS_H
#define WORKSPACESVNSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class WorkspaceSvnSettings : public clConfigItem
{
    wxString m_repoPath;
    wxFileName m_workspaceFileName;
    
public:
    WorkspaceSvnSettings(const wxFileName& fn);
    WorkspaceSvnSettings();
    virtual ~WorkspaceSvnSettings();

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    WorkspaceSvnSettings& Load();
    void Save();

    void SetRepoPath(const wxString& repoPath) {
        this->m_repoPath = repoPath;
    }
    const wxString& GetRepoPath() const {
        return m_repoPath;
    }
    wxFileName GetLocalConfigFile() const;
};

#endif // WORKSPACESVNSETTINGS_H
