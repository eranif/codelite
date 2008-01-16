#include "configuration_mapping.h"
#include "xmlutils.h"
#include "macros.h"

BuildMatrix::BuildMatrix(wxXmlNode *node){
	if(node){
		wxXmlNode *config = node->GetChildren();
		while(config){
			if(config->GetName() == wxT("WorkspaceConfiguration")){
				m_configurationList.push_back(new WorkspaceConfiguration(config));
			}
			config = config->GetNext(); 
		}
	}else{
		//construct default empty mapping with a default build configuration
		m_configurationList.push_back(new WorkspaceConfiguration(wxT("Debug"), true));
	}
}

BuildMatrix::~BuildMatrix(){
}

wxXmlNode *BuildMatrix::ToXml() const {
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildMatrix"));
	std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		node->AddChild((*iter)->ToXml());
	}
	return node;
}

void BuildMatrix::RemoveConfiguration(const wxString &configName){
	bool isSelected = false;
	std::list<WorkspaceConfigurationPtr>::iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->GetName() == configName){
			isSelected = (*iter)->IsSelected();
			m_configurationList.erase(iter);
			break;
		}
	}

	if(isSelected){
		//the deleted configuration was the selected one,
		//set the first one as selected
		if(m_configurationList.empty() == false){
			(*m_configurationList.begin())->SetSelected(true);
		}
	}
}

void BuildMatrix::SetConfiguration(WorkspaceConfigurationPtr conf){
	RemoveConfiguration(conf->GetName());
	m_configurationList.push_back(conf);
}

wxString BuildMatrix::GetProjectSelectedConf(const wxString &configName, const wxString &project) const
{
	std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->GetName() == configName){
			WorkspaceConfiguration::ConfigMappingList list = (*iter)->GetMapping();
			WorkspaceConfiguration::ConfigMappingList::const_iterator it = list.begin();
			for(; it != list.end(); it++){
				if((*it).m_project == project){
					return (*it).m_name;
				}
			}
			break;
		}
	}
	return wxEmptyString;
}


wxString BuildMatrix::GetSelectedConfigurationName() const 
{
	std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->IsSelected()){
			return (*iter)->GetName();
		}
	}
	return wxEmptyString;
}

WorkspaceConfigurationPtr BuildMatrix::GetConfigurationByName(const wxString &name) const
{
	return FindConfiguration(name);
}

WorkspaceConfigurationPtr BuildMatrix::FindConfiguration(const wxString &name) const
{
	std::list<WorkspaceConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->GetName() == name){
			return (*iter);
		}
	}
	return NULL;
}

void BuildMatrix::SetSelectedConfigurationName(const wxString &name)
{
	//find the current selected configuration
	std::list<WorkspaceConfigurationPtr>::iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->IsSelected()){
			(*iter)->SetSelected(false);
			break;
		}
	}
	//set the new one
	WorkspaceConfigurationPtr c = FindConfiguration(name);
	if(c){
		c->SetSelected(true);
	}
}

//------------------------------------------------
// WorkspaceConfiguration object
//------------------------------------------------
WorkspaceConfiguration::WorkspaceConfiguration()
: m_name(wxEmptyString)
, m_isSelected(false)
{
}

WorkspaceConfiguration::WorkspaceConfiguration(const wxString &name, bool selected)
: m_name(name)
, m_isSelected(selected)
{
}

WorkspaceConfiguration::WorkspaceConfiguration(wxXmlNode *node){
	if(node){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		m_isSelected = XmlUtils::ReadBool(node, wxT("Selected"));
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("Project")){
				wxString projName = XmlUtils::ReadString(child, wxT("Name"));
				wxString conf = XmlUtils::ReadString(child, wxT("ConfigName"));
				m_mappingList.push_back(ConfigMappingEntry(projName, conf));
			}
			child = child->GetNext();
		}
	}else{
		m_isSelected = false;
		m_name = wxEmptyString;
	}
}

WorkspaceConfiguration::~WorkspaceConfiguration(){
}

wxXmlNode *WorkspaceConfiguration::ToXml() const{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("WorkspaceConfiguration"));
	node->AddProperty(wxT("Name"), m_name);
	node->AddProperty(wxT("Selected"), BoolToString(m_isSelected));

	WorkspaceConfiguration::ConfigMappingList::const_iterator iter = m_mappingList.begin();
	for(; iter  != m_mappingList.end(); iter++){
		wxXmlNode *projNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
		projNode->AddProperty(wxT("Name"), iter->m_project);
		projNode->AddProperty(wxT("ConfigName"), iter->m_name);
		node->AddChild(projNode);
	}
	return node;
}

