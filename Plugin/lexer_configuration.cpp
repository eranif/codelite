#include "lexer_configuration.h"
#include "xmlutils.h"
#include "macros.h"

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
		wxXmlNode *node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords"));
		if( node ){
			m_keyWords = node->GetNodeContent();
			m_keyWords.Replace(wxT("\n"), wxT(" "));
			m_keyWords.Replace(wxT("\r"), wxT(" "));
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
					wxString face = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
					wxString colour = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
					wxString bgcolour = XmlUtils::ReadString(prop, wxT("BgColour"), wxT("white"));
					long fontSize = XmlUtils::ReadLong(prop, wxT("Size"), 10);
					long propId   = XmlUtils::ReadLong(prop, wxT("Id"), 0);
					
					StyleProperty property = StyleProperty(propId, colour, bgcolour, fontSize, Name, face, bold.CmpNoCase(wxT("Yes")) == 0);
					m_properties.push_back( property );
				}
				prop = prop->GetNext();
			}
		}
	}
}

LexerConf::~LexerConf()
{
	if(m_doc.IsOk()){
		m_doc.Save(m_fileName.GetFullPath());
	}
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
	wxXmlNode *keyWords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords"));
	XmlUtils::SetNodeContent(keyWords, GetKeyWords());
	node->AddChild(keyWords);

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

		wxString strSize;
		strSize << p.GetFontSize();
		property->AddProperty(wxT("Size"), strSize);
		properties->AddChild(property);
	}
	node->AddChild( properties );
	return node;
}
