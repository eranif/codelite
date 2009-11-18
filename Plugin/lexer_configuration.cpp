//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : lexer_configuration.cpp
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
#include <wx/utils.h>
#include "globals.h"
#include "lexer_configuration.h"
#include "xmlutils.h"
#include "macros.h"

static bool StringTolBool(const wxString &s) {
	bool res = s.CmpNoCase(wxT("Yes")) == 0 ? true : false;
	return res;
}

LexerConf::LexerConf(const wxString &fileName)
: m_fileName(fileName)
{
	m_fileName.MakeAbsolute();
	m_doc.Load(m_fileName.GetFullPath());
	if(m_doc.GetRoot()){
		Parse(m_doc.GetRoot());
	}
}

void LexerConf::Save()
{
	//replace the root node with the new xml representation for this object
	m_doc.SetRoot(ToXml());

	if(m_doc.IsOk()){
		//we never save to the default file, but rather we create our own copy of it
		wxString userExt( clGetUserName() + wxT("_xml"));
		if(m_fileName.GetExt() != userExt) {
			m_fileName.SetExt( userExt );
		}

		m_doc.Save(m_fileName.GetFullPath());
	}
}

void LexerConf::Parse(wxXmlNode *element)
{
	if( element ){
		m_lexerId = XmlUtils::ReadLong(element, wxT("Id"), 0);

		// read the lexer name
		m_name = element->GetPropVal(wxT("Name"), wxEmptyString);

		// load key words
		wxXmlNode *node = NULL;
		node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords0"));
		if( node ){
			m_keyWords[0] = node->GetNodeContent();
			m_keyWords[0].Replace(wxT("\n"), wxT(" "));
			m_keyWords[0].Replace(wxT("\r"), wxT(" "));
		}

		node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords1"));
		if( node ){
			m_keyWords[1] = node->GetNodeContent();
			m_keyWords[1].Replace(wxT("\n"), wxT(" "));
			m_keyWords[1].Replace(wxT("\r"), wxT(" "));
		}

		node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords2"));
		if( node ){
			m_keyWords[2] = node->GetNodeContent();
			m_keyWords[2].Replace(wxT("\n"), wxT(" "));
			m_keyWords[2].Replace(wxT("\r"), wxT(" "));
		}

		node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords3"));
		if( node ){
			m_keyWords[3] = node->GetNodeContent();
			m_keyWords[3].Replace(wxT("\n"), wxT(" "));
			m_keyWords[3].Replace(wxT("\r"), wxT(" "));
		}

		node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords4"));
		if( node ){
			m_keyWords[4] = node->GetNodeContent();
			m_keyWords[4].Replace(wxT("\n"), wxT(" "));
			m_keyWords[4].Replace(wxT("\r"), wxT(" "));
		}

		// load extensions
		node = XmlUtils::FindFirstByTagName(element, wxT("Extensions"));
		if( node ){
			m_extension = node->GetNodeContent();
		}

		// load properties
		// Search for <properties>
		node = XmlUtils::FindFirstByTagName(element, wxT("Properties"));
		if( node )
		{
			// We found the element, read the attributes
			wxXmlNode* prop = node->GetChildren();
			while( prop )
			{
				if(prop->GetName() == wxT("Property")){
					// Read the font attributes
					wxString Name = XmlUtils::ReadString(prop, wxT("Name"), wxT("DEFAULT"));
					wxString bold = XmlUtils::ReadString(prop, wxT("Bold"), wxT("no"));
					wxString italic = XmlUtils::ReadString(prop, wxT("Italic"), wxT("no"));
					wxString underline = XmlUtils::ReadString(prop, wxT("Underline"), wxT("no"));
					wxString strikeout = XmlUtils::ReadString(prop, wxT("Strikeout"), wxT("no"));
					wxString face = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
					wxString colour = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
					wxString bgcolour = XmlUtils::ReadString(prop, wxT("BgColour"), wxT("white"));
					long fontSize = XmlUtils::ReadLong(prop, wxT("Size"), 10);
					long propId   = XmlUtils::ReadLong(prop, wxT("Id"), 0);

					StyleProperty property = StyleProperty(propId, colour, bgcolour, fontSize, Name, face,
										StringTolBool(bold),
										StringTolBool(italic),
										StringTolBool(underline));
					m_properties.push_back( property );
				}
				prop = prop->GetNext();
			}
		}
	}
}

LexerConf::~LexerConf()
{
}

wxXmlNode *LexerConf::ToXml() const
{
	//convert the lexer back xml node
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Lexer"));
	//set the lexer name
	node->AddProperty(wxT("Name"), GetName());

	wxString strId;
	strId << GetLexerId();
	node->AddProperty(wxT("Id"), strId);

	//set the keywords node
	wxXmlNode *keyWords0 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords0"));
	XmlUtils::SetNodeContent(keyWords0, GetKeyWords(0));
	node->AddChild(keyWords0);

	wxXmlNode *keyWords1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords1"));
	XmlUtils::SetNodeContent(keyWords1, GetKeyWords(1));
	node->AddChild(keyWords1);

	wxXmlNode *keyWords2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords2"));
	XmlUtils::SetNodeContent(keyWords2, GetKeyWords(2));
	node->AddChild(keyWords2);

	wxXmlNode *keyWords3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords3"));
	XmlUtils::SetNodeContent(keyWords3, GetKeyWords(3));
	node->AddChild(keyWords3);

	wxXmlNode *keyWords4 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords4"));
	XmlUtils::SetNodeContent(keyWords4, GetKeyWords(4));
	node->AddChild(keyWords4);

	//set the extensions node
	wxXmlNode *extesions = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Extensions"));
	XmlUtils::SetNodeContent(extesions, GetFileSpec());
	node->AddChild(extesions);

	//set the properties
	wxXmlNode *properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Properties"));
	std::list<StyleProperty>::const_iterator iter = m_properties.begin();
	for(; iter != m_properties.end(); iter ++){
		StyleProperty p = (*iter);
		wxXmlNode *property = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Property"));

		wxString strId;
		strId << p.GetId();
		property->AddProperty(wxT("Id"), strId);
		property->AddProperty(wxT("Name"), p.GetName());
		property->AddProperty(wxT("Bold"), BoolToString(p.IsBold()));
		property->AddProperty(wxT("Face"), p.GetFaceName());
		property->AddProperty(wxT("Colour"), p.GetFgColour());
		property->AddProperty(wxT("BgColour"), p.GetBgColour());
		property->AddProperty(wxT("Italic"), BoolToString(p.GetItalic()));
		property->AddProperty(wxT("Underline"), BoolToString(p.GetUnderlined()));

		wxString strSize;
		strSize << p.GetFontSize();
		property->AddProperty(wxT("Size"), strSize);
		properties->AddChild(property);
	}
	node->AddChild( properties );
	return node;
}
