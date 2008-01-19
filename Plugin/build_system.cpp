#include "build_system.h"
#include "xmlutils.h"

BuildSystem::BuildSystem(wxXmlNode *node)
{
	if(node){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		m_toolPath = XmlUtils::ReadString(node, wxT("ToolPath"));	
		m_toolOptions = XmlUtils::ReadString(node, wxT("Options"));	
		m_toolJobs = XmlUtils::ReadString(node, wxT("Jobs"), wxT("1"));
	}
}

BuildSystem::~BuildSystem()
{
}

wxXmlNode *BuildSystem::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildSystem"));
	node->AddProperty(wxT("Name"), m_name);
	node->AddProperty(wxT("ToolPath"), m_toolPath);
	node->AddProperty(wxT("Options"), m_toolOptions);
	node->AddProperty(wxT("Jobs"), m_toolJobs);
	return node;
}
