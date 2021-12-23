//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configuration_mapping.h
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
#ifndef CONFIGURATION_MAPPING_H
#define CONFIGURATION_MAPPING_H

#include "codelite_exports.h"
#include "list"
#include "smart_ptr.h"
#include "wx/string.h"
#include "wx/xml/xml.h"

class WXDLLIMPEXP_SDK ConfigMappingEntry
{
public:
    wxString m_project;
    wxString m_name;

public:
    ConfigMappingEntry(const wxString& project, const wxString& name)
        : m_project(project)
        , m_name(name)
    {
    }

    ConfigMappingEntry()
        : m_project(wxEmptyString)
        , m_name(wxEmptyString)
    {
    }

    virtual ~ConfigMappingEntry() {}
};

class WXDLLIMPEXP_SDK WorkspaceConfiguration
{
public:
    typedef std::list<ConfigMappingEntry> ConfigMappingList;

private:
    wxString m_name;
    ConfigMappingList m_mappingList;
    wxString m_environmentVariables;

public:
    void RenameProject(const wxString& oldname, const wxString& newname);
    WorkspaceConfiguration();
    WorkspaceConfiguration(wxXmlNode* node);
    WorkspaceConfiguration(const wxString& name);
    virtual ~WorkspaceConfiguration();
    wxXmlNode* ToXml() const;

    const wxString& GetName() const { return m_name; }
    const ConfigMappingList& GetMapping() const { return m_mappingList; }
    void SetConfigMappingList(const ConfigMappingList& mapList) { m_mappingList = mapList; }
    void SetName(const wxString& name) { m_name = name; }
    const wxString& GetEnvironmentVariables() const { return m_environmentVariables; }
    void SetEnvironmentVariables(const wxString& environmentVariables)
    {
        this->m_environmentVariables = environmentVariables;
    }
};

typedef SmartPtr<WorkspaceConfiguration> WorkspaceConfigurationPtr;

class WXDLLIMPEXP_SDK BuildMatrix
{
    std::list<WorkspaceConfigurationPtr> m_configurationList;
    wxString m_selectedConfiguration;

protected:
    WorkspaceConfigurationPtr FindConfiguration(const wxString& name) const;
    void SelectFirstConfiguration();

public:
    void RenameProject(const wxString& oldname, const wxString& newname);

    BuildMatrix(wxXmlNode* node, const wxString& selectedConfiguration);
    virtual ~BuildMatrix();
    wxXmlNode* ToXml() const;
    const std::list<WorkspaceConfigurationPtr>& GetConfigurations() const { return m_configurationList; }
    void RemoveConfiguration(const wxString& configName);
    void SetConfiguration(WorkspaceConfigurationPtr conf);
    wxString GetProjectSelectedConf(const wxString& configName, const wxString& project) const;
    wxString GetSelectedConfigurationName() const { return m_selectedConfiguration; }
    void SetSelectedConfigurationName(const wxString& name);
    WorkspaceConfigurationPtr GetConfigurationByName(const wxString& name) const;
};

typedef SmartPtr<BuildMatrix> BuildMatrixPtr;

#endif // CONFIGURATION_MAPPING_H
