#include "envvar.h"
#include "wx/utils.h"
#include "wx/xml/xml.h"
#include "xmlutils.h"

EnvironmentVariebles::EnvironmentVariebles(wxXmlNode *node){
	if(node){
		//read values XML 
		wxXmlNode *child = node->GetChildren();
		while(child){
			wxString name = XmlUtils::ReadString(child, wxT("Name"));
			wxString value = XmlUtils::ReadString(child, wxT("Value"));
			SetEnv(name, value);
			child = child->GetNext();
		}
	}else{
		//do nothing
	}
}

EnvironmentVariebles::~EnvironmentVariebles()
{
}

void EnvironmentVariebles::SetEnv(const wxString &name, const wxString &value)
{
	m_variables[name] = value;
}

wxString EnvironmentVariebles::GetEnv(const wxString &name) const
{
	wxString value(wxEmptyString);

	//try to find in the map
	EnvironmentVariebles::ConstIterator iter = m_variables.find(name);
	if(iter == End()){
		//no match, try to find in the system
		wxGetEnv(name, &value);
		//does not matter if we didnt find match, simply return an empty string
		return value;
	}else{
		//we found it in our map
		return iter->second;
	}
}

void EnvironmentVariebles::DeleteEnv(const wxString &name)
{
	std::map<wxString, wxString>::iterator iter = m_variables.find(name);
	if(iter != End()){
		m_variables.erase(iter);
	}
}

wxXmlNode *EnvironmentVariebles::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Environment"));
	EnvironmentVariebles::ConstIterator iter = Begin();
	for(; iter != End(); iter++)
	{
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Variable"));
		node->AddChild(child);
		child->AddProperty(wxT("Name"), iter->first);
		child->AddProperty(wxT("Value"), iter->second);
	}
	return node;
}
EnvironmentVariebles::ConstIterator EnvironmentVariebles::Begin() const
{
	return m_variables.begin();
}

EnvironmentVariebles::ConstIterator EnvironmentVariebles::End() const
{
	return m_variables.end();
}


