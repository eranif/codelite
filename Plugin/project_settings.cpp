//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : project_settings.cpp
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
#include "project_settings.h"
#include "xmlutils.h"
#include "project.h"

ProjectSettings::ProjectSettings(wxXmlNode *node)
{
	if (node) {
		// load configurations
		m_projectType = XmlUtils::ReadString(node, wxT("Type"));
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("Configuration")) {
				wxString configName = XmlUtils::ReadString(child, wxT("Name"));
				m_configs.insert(std::pair<wxString, BuildConfigPtr>(configName, new BuildConfig(child)));
			}
			child = child->GetNext();
		}
	} else {
		//create new settings with default values
		m_projectType = Project::STATIC_LIBRARY;
		m_configs.insert(std::pair<wxString, BuildConfigPtr>(wxT("Debug"), new BuildConfig(NULL)));
	}
}

ProjectSettings::~ProjectSettings()
{
}

ProjectSettings *ProjectSettings::Clone() const
{
	wxXmlNode *node = ToXml();
	ProjectSettings *cloned = new ProjectSettings(node);
	delete node;
	return cloned;
}

wxXmlNode *ProjectSettings::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Settings"));
	node->AddProperty(wxT("Type"), m_projectType);
	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.begin();
	for (; iter != m_configs.end(); iter++) {
		node->AddChild(iter->second->ToXml());
	}
	return node;
}

BuildConfigPtr ProjectSettings::GetBuildConfiguration(const wxString &configName) const
{
	wxString confName = configName;
	if (confName.IsEmpty()) {
		confName = wxT("Debug");
	}

	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.find(confName);
	if (iter == m_configs.end()) {
		return NULL;
	}
	return iter->second;
}

BuildConfigPtr ProjectSettings::GetFirstBuildConfiguration(ProjectSettingsCookie &cookie) const
{
	cookie.iter = m_configs.begin();
	if (cookie.iter != m_configs.end()) {
		BuildConfigPtr conf = cookie.iter->second;
		cookie.iter++;
		return conf;
	}
	return NULL;
}

BuildConfigPtr ProjectSettings::GetNextBuildConfiguration(ProjectSettingsCookie &cookie) const
{
	if (cookie.iter != m_configs.end()) {
		BuildConfigPtr conf = cookie.iter->second;
		cookie.iter++;
		return conf;
	}
	return NULL;
}

void ProjectSettings::SetBuildConfiguration(const BuildConfigPtr bc)
{
	m_configs[bc->GetName()] = bc;
}

void ProjectSettings::RemoveConfiguration(const wxString  &configName)
{
	std::map<wxString, BuildConfigPtr>::iterator iter = m_configs.find(configName);
	if (iter != m_configs.end()) {
		m_configs.erase(iter);
	}
}

wxString ProjectSettings::GetProjectType(const wxString &confName)
{
	// try to return the project type per configuration name. If no property name 'configurationType' exists,
	// return the parent configuration type
	if (confName.IsEmpty() == false) {
		std::map<wxString, BuildConfigPtr>::iterator iter = m_configs.find(confName);
		if (iter != m_configs.end()) {
			BuildConfigPtr conf = iter->second;
			wxString type = conf->GetProjectType();
			if (type.IsEmpty()) {
				type = m_projectType;
			}
			return type;
		}
	}
	return m_projectType;
}
