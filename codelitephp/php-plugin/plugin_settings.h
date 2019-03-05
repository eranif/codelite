//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : plugin_settings.h
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

#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class PluginSettings : public clConfigItem
{
    wxString m_openWorkspaceFilter;

public:
    PluginSettings();
    virtual ~PluginSettings();
    
    void SetOpenWorkspaceFilter(const wxString& openWorkspaceFilter) {
        this->m_openWorkspaceFilter = openWorkspaceFilter;
    }
    const wxString& GetOpenWorkspaceFilter() const {
        return m_openWorkspaceFilter;
    }
    
    static void Load(PluginSettings& settings);
    static void Save(const PluginSettings& settings);
    
public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
};

#endif // PLUGINSETTINGS_H
