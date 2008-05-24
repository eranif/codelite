#include "archive.h"
#include <wx/colour.h>
#include <wx/xml/xml.h>
#include "serialized_object.h"

//helper functions
static wxXmlNode *FindNodeByName(const wxXmlNode *parent, const wxString &tagName, const wxString &name)
{
	if (!parent) {
		return NULL;
	}

	
	
	wxXmlNode *child = parent->GetChildren();
	while ( child ) {
		if ( child->GetName() == tagName) {
			if ( child->GetPropVal(wxT("Name"), wxEmptyString) == name) {
				return child;
			}
		}
		child = child->GetNext();
	}
	return NULL;
}

Archive::Archive()
		: m_root(NULL)
{
}

Archive::~Archive()
{
}

void Archive::Write(const wxString &name, SerializedObject *obj)
{
	Archive arch;
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("SerializedObject"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Name"), name);

	arch.SetXmlNode(node);
	obj->Serialize(arch);
}

void Archive::Read(const wxString &name, SerializedObject *obj)
{
	Archive arch;
	wxXmlNode *node = FindNodeByName(m_root, wxT("SerializedObject"), name);
	if (node) {
		arch.SetXmlNode(node);
		obj->DeSerialize(arch);
	}
}

void Archive::Write(const wxString &name, const wxArrayString &arr)
{
	if (!m_root) {
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxArrayString"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Name"), name);

	//add an entry for each wxString in the array
	for (size_t i=0; i<arr.GetCount(); i++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
		node->AddChild(child);
		child->AddProperty(wxT("Value"), arr.Item(i));
	}
}

void Archive::Write(const wxString &name, const StringMap &str_map)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("StringMap"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Name"), name);

	//add an entry for each wxString in the array
	StringMap::const_iterator iter = str_map.begin();
	for ( ; iter != str_map.end(); iter++ ) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("MapEntry"));
		node->AddChild(child);
		child->AddProperty(wxT("Key"), iter->first);
		child->AddProperty(wxT("Value"), iter->second);
	}
}

void Archive::Write(const wxString &name, wxSize size)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxSize"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Name"), name);

	wxString xstr, ystr;
	xstr << size.x;
	ystr << size.y;

	node->AddProperty(wxT("x"), xstr);
	node->AddProperty(wxT("y"), ystr);
}

void Archive::Write(const wxString &name, wxPoint pt)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxPoint"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Name"), name);

	wxString xstr, ystr;
	xstr << pt.x;
	ystr << pt.y;

	node->AddProperty(wxT("x"), xstr);
	node->AddProperty(wxT("y"), ystr);
}

void Archive::Read(const wxString &name, wxArrayString &arr)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = FindNodeByName(m_root, wxT("wxArrayString"), name);
	if (node) {
		//fill the output array with the values
		arr.Clear();
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("wxString")) {
				wxString value;
				value = child->GetPropVal(wxT("Value"), wxEmptyString);
				arr.Add(value);
			}
			child = child->GetNext();
		}
	}
}

void Archive::Read(const wxString &name, StringMap &str_map)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = FindNodeByName(m_root, wxT("StringMap"), name);
	if (node) {
		//fill the output array with the values
		str_map.clear();
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("MapEntry")) {
				wxString value;
				wxString key;
				key = child->GetPropVal(wxT("Key"), wxEmptyString);
				value = child->GetPropVal(wxT("Value"), wxEmptyString);
				str_map[key] = value;
			}
			child = child->GetNext();
		}
	}
}

void Archive::Read(const wxString &name, wxSize &size)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = FindNodeByName(m_root, wxT("wxSize"), name);
	if (node) {
		long v;
		wxString value;
		value = node->GetPropVal(wxT("x"), wxEmptyString);
		value.ToLong(&v);
		size.x = v;

		value = node->GetPropVal(wxT("y"), wxEmptyString);
		value.ToLong(&v);
		size.y = v;
	}
}

void Archive::Read(const wxString &name, wxPoint &pt)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = FindNodeByName(m_root, wxT("wxPoint"), name);
	if (node) {
		long v;
		wxString value;
		value = node->GetPropVal(wxT("x"), wxEmptyString);
		value.ToLong(&v);
		pt.x = v;

		value = node->GetPropVal(wxT("y"), wxEmptyString);
		value.ToLong(&v);
		pt.y = v;
	}
}

void Archive::Write(const wxString &name, int value)
{
	if (!m_root) {
		return;
	}

	WriteSimple(value, wxT("int"), name);
}

void Archive::Read(const wxString &name, int &value)
{
	if (!m_root) {
		return;
	}

	long v;
	ReadSimple(v, wxT("int"), name);
	value = v;
}

void Archive::Write(const wxString &name, long value)
{
	if (!m_root) {
		return;
	}
	WriteSimple(value, wxT("long"), name);
}

void Archive::Read(const wxString &name, long &value)
{
	if (!m_root) {
		return;
	}
	ReadSimple(value, wxT("long"), name);
}

void Archive::Write(const wxString &name, bool value)
{
	if (!m_root) {
		return;
	}
	WriteSimple(value ? 1 : 0, wxT("bool"), name);
}

void Archive::Read(const wxString &name, bool &value)
{
	if (!m_root) { 
		return;
	}

	long v;
	ReadSimple(v, wxT("bool"), name);
	v  == 0 ? value = false : value = true;
}

void Archive::Write(const wxString &name, const wxString &str)
{
	if (!m_root) {
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), str);
	node->AddProperty(wxT("Name"), name);
}

void Archive::Read(const wxString &name, wxString &value)
{
	if (!m_root) {
		return;
	}
	wxXmlNode *node = FindNodeByName(m_root, wxT("wxString"), name);
	if (node) {
		value = node->GetPropVal(wxT("Value"), wxEmptyString);
	}
}

void Archive::Read(const wxString &name, size_t &value)
{
	long v = 0;
	Read(name, v);
	value = v;
}

void Archive::Read(const wxString &name, wxFileName &fileName)
{
	wxString value;
	Read(name, value);
	fileName = wxFileName(value);
}

void Archive::Write(const wxString &name, size_t value)
{
	Write(name, (long)value);
}

void Archive::Write(const wxString &name, const wxFileName &fileName)
{
	Write(name, fileName.GetFullPath());
}

void Archive::SetXmlNode(wxXmlNode *node)
{
	m_root = node;
}

void Archive::WriteSimple(long value, const wxString &typeName, const wxString &name)
{
	if (!m_root)
		return;

	wxString propValue;
	propValue << value;

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, typeName);
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), propValue);
	node->AddProperty(wxT("Name"), name);
}

void Archive::ReadSimple(long &value, const wxString &typeName, const wxString &name)
{
	if (!m_root)
		return;

	value = 0;
	wxXmlNode *node = FindNodeByName(m_root, typeName, name);
	if (node) {
		wxString val = node->GetPropVal(wxT("Value"), wxEmptyString);
		val.ToLong(&value);
	}
}

void Archive::Read(const wxString& name, wxColour& colour)
{
	if (!m_root) {
		return;
	}

	wxXmlNode *node = FindNodeByName(m_root, wxT("wxColour"), name);
	wxString value;
	if (node) {
		value = node->GetPropVal(wxT("Value"), wxEmptyString);
	}

	if (value.IsEmpty()) {
		return;
	}

	colour = wxColour(value);
}

void Archive::Write(const wxString& name, const wxColour& colour)
{
	if (!m_root) {
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxColour"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), colour.GetAsString());
	node->AddProperty(wxT("Name"), name);
}
