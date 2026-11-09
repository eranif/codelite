//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newplugindata.h
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
#pragma once

#include <wx/string.h>

class NewPluginData
{
    wxString m_projectPath;
    wxString m_pluginName;
    wxString m_codelitePath;
    wxString m_pluginDescription;

public:
    NewPluginData();
    ~NewPluginData();

    // Setters
    void SetCodelitePath(const wxString& codelitePath) { this->m_codelitePath = codelitePath; }
    void SetPluginDescription(const wxString& pluginDescription) { this->m_pluginDescription = pluginDescription; }
    void SetPluginName(const wxString& pluginName) { this->m_pluginName = pluginName; }
    void SetProjectPath(const wxString& projectPath) { this->m_projectPath = projectPath; }
    // Getters
    const wxString& GetCodelitePath() const { return m_codelitePath; }
    const wxString& GetPluginDescription() const { return m_pluginDescription; }
    const wxString& GetPluginName() const { return m_pluginName; }
    const wxString& GetProjectPath() const { return m_projectPath; }
};
