//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configuration_mapping.cpp
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

#include "configuration_mapping.h"

#include "macros.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"

BuildMatrix::BuildMatrix(wxXmlNode* node, const wxString& selectedConfiguration)
    : m_selectedConfiguration(selectedConfiguration)
{
    if(node) {
        wxXmlNode* config = node->GetChildren();
        while(config) {
            if(config->GetName() == wxT("WorkspaceConfiguration")) {
                m_configurationList.push_back(new WorkspaceConfiguration(config));
            }
            config = config->GetNext();
        }
    } else {
        // construct default empty mapping with a default build configuration
        m_configurationList.push_back(new WorkspaceConfiguration(wxT("Debug")));
        m_configurationList.push_back(new WorkspaceConfiguration(wxT("Release")));
    }

    // verify the selected configuration
    if(m_selectedConfiguration.IsEmpty() || !FindConfiguration(m_selectedConfiguration)) {
        SelectFirstConfiguration();
    }
}

BuildMatrix::~BuildMatrix() {}

wxXmlNode* BuildMatrix::ToXml() const
{
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildMatrix"));
    std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
    for(; iter != m_configurationList.end(); iter++) {
        node->AddChild((*iter)->ToXml());
    }
    return node;
}

void BuildMatrix::RemoveConfiguration(const wxString& configName)
{
    std::list<WorkspaceConfigurationPtr>::iterator iter = m_configurationList.begin();
    for(; iter != m_configurationList.end(); iter++) {
        if((*iter)->GetName() == configName) {
            m_configurationList.erase(iter);
            break;
        }
    }

    if(m_selectedConfiguration == configName) {
        // the deleted configuration was the selected one,
        // set the first one as selected
        SelectFirstConfiguration();
    }
}

void BuildMatrix::SetConfiguration(WorkspaceConfigurationPtr conf)
{
    RemoveConfiguration(conf->GetName());
    m_configurationList.push_back(conf);
}

wxString BuildMatrix::GetProjectSelectedConf(const wxString& configName, const wxString& project) const
{
    std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
    for(; iter != m_configurationList.end(); iter++) {
        if((*iter)->GetName() == configName) {
            WorkspaceConfiguration::ConfigMappingList list = (*iter)->GetMapping();
            WorkspaceConfiguration::ConfigMappingList::const_iterator it = list.begin();
            for(; it != list.end(); it++) {
                if((*it).m_project == project) {
                    return (*it).m_name;
                }
            }
            break;
        }
    }
    return wxEmptyString;
}

WorkspaceConfigurationPtr BuildMatrix::GetConfigurationByName(const wxString& name) const
{
    return FindConfiguration(name);
}

WorkspaceConfigurationPtr BuildMatrix::FindConfiguration(const wxString& name) const
{
    std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
    for(; iter != m_configurationList.end(); iter++) {
        if((*iter)->GetName() == name) {
            return (*iter);
        }
    }
    return NULL;
}

void BuildMatrix::SetSelectedConfigurationName(const wxString& name)
{
    if(FindConfiguration(name)) {
        m_selectedConfiguration = name;
    }
}

void BuildMatrix::SelectFirstConfiguration()
{
    if(!m_configurationList.empty()) {
        m_selectedConfiguration = m_configurationList.front()->GetName();
    } else {
        // there are no available configurations...
        m_selectedConfiguration = wxEmptyString;
    }
}

void BuildMatrix::RenameProject(const wxString& oldname, const wxString& newname)
{
    std::list<WorkspaceConfigurationPtr>::iterator iter = m_configurationList.begin();
    for(; iter != m_configurationList.end(); ++iter) {
        (*iter)->RenameProject(oldname, newname);
    }
}

//------------------------------------------------
// WorkspaceConfiguration object
//------------------------------------------------
WorkspaceConfiguration::WorkspaceConfiguration()
    : m_name(wxEmptyString)
{
}

WorkspaceConfiguration::WorkspaceConfiguration(const wxString& name)
    : m_name(name)
{
}

WorkspaceConfiguration::WorkspaceConfiguration(wxXmlNode* node)
{
    if(node) {
        m_name = XmlUtils::ReadString(node, wxT("Name"));
        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Project")) {
                wxString projName = XmlUtils::ReadString(child, wxT("Name"));
                wxString conf = XmlUtils::ReadString(child, wxT("ConfigName"));
                m_mappingList.push_back(ConfigMappingEntry(projName, conf));
            } else if(child->GetName() == "Environment") {
                m_environmentVariables = child->GetNodeContent();
            }
            child = child->GetNext();
        }
    } else {
        m_name = wxEmptyString;
        m_environmentVariables.Clear();
    }
}

WorkspaceConfiguration::~WorkspaceConfiguration() {}

wxXmlNode* WorkspaceConfiguration::ToXml() const
{
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("WorkspaceConfiguration"));
    node->AddAttribute(wxT("Name"), m_name);

    wxXmlNode* env = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Environment");
    XmlUtils::SetNodeContent(env, m_environmentVariables);
    node->AddChild(env);

    WorkspaceConfiguration::ConfigMappingList::const_iterator iter = m_mappingList.begin();
    for(; iter != m_mappingList.end(); iter++) {
        wxXmlNode* projNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
        projNode->AddAttribute(wxT("Name"), iter->m_project);
        projNode->AddAttribute(wxT("ConfigName"), iter->m_name);
        node->AddChild(projNode);
    }
    return node;
}

void WorkspaceConfiguration::RenameProject(const wxString& oldname, const wxString& newname)
{
    ConfigMappingList::iterator iter = m_mappingList.begin();
    for(; iter != m_mappingList.end(); ++iter) {
        if(iter->m_project == oldname) {
            iter->m_project = newname;
        }
    }
}
