#include "project_settings.h"
#include "xmlutils.h"
#include "project.h"

ProjectSettings::ProjectSettings(wxXmlNode *node)
{
	if(node){
		// load configurations
		m_projectType = XmlUtils::ReadString(node, wxT("Type"));
		wxXmlNode *child = node->GetChildren();
		while(child) {
			if(child->GetName() == wxT("Configuration")){
				wxString configName = XmlUtils::ReadString(child, wxT("Name"));
				m_configs.insert(std::pair<wxString, BuildConfigPtr>(configName, new BuildConfig(child)));
			}
			child = child->GetNext();
		}
	}else{
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
	for(; iter != m_configs.end(); iter++){
		node->AddChild(iter->second->ToXml());
	}
	return node;
}

BuildConfigPtr ProjectSettings::GetBuildConfiguration(const wxString &configName) const
{
	wxString confName = configName;
	if(confName.IsEmpty()){
		confName = wxT("Debug");
	}

	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.find(confName);
	if(iter == m_configs.end()){
		return NULL;
	}
	return iter->second;
}

BuildConfigPtr ProjectSettings::GetFirstBuildConfiguration(ProjectSettingsCookie &cookie) const
{
	cookie.iter = m_configs.begin();
	if(cookie.iter != m_configs.end()){
		BuildConfigPtr conf = cookie.iter->second;
		cookie.iter++;
		return conf;
	}
	return NULL;
}

BuildConfigPtr ProjectSettings::GetNextBuildConfiguration(ProjectSettingsCookie &cookie) const
{
	if(cookie.iter != m_configs.end()){
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
	if(iter != m_configs.end()){
		m_configs.erase(iter);
	}
}
