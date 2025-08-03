//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSPackageJSON.h
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

#ifndef NODEJSPACKAGEJSON_H
#define NODEJSPACKAGEJSON_H

#include <wx/filename.h>

class NodeJSPackageJSON
{
    wxFileName m_filename;
    wxString m_name;
    wxString m_version;
    wxString m_description;
    wxFileName m_script;
    wxArrayString m_args;

public:
    NodeJSPackageJSON() = default;
    virtual ~NodeJSPackageJSON() = default;
    
    /**
     * @brief load the local package.json file stored under the .codelite folder
     */
    bool Load(const wxString& projectPath);
    
    /**
     * @brief create the local package.json file under the .codelite folder
     * baed on the project package.json file
     */
    bool Create(const wxString& projectPath);
    
    /**
     * @brief save the settings
     */
    bool Save(const wxString& projectPath);
    
    void SetDescription(const wxString& description) { this->m_description = description; }
    void SetName(const wxString& name) { this->m_name = name; }
    void SetVersion(const wxString& version) { this->m_version = version; }
    const wxString& GetDescription() const { return m_description; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetVersion() const { return m_version; }
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    const wxFileName& GetFilename() const { return m_filename; }
    void SetScript(const wxFileName& script) { this->m_script = script; }
    const wxFileName& GetScript() const { return m_script; }
    void SetArgs(const wxArrayString& args) { this->m_args = args; }
    const wxArrayString& GetArgs() const { return m_args; }
};

#endif // NODEJSPACKAGEJSON_H
