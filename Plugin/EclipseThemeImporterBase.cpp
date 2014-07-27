#include "EclipseThemeImporterBase.h"
#include "drawingutils.h"
#include "xmlutils.h"
#include <wx/colour.h>

EclipseThemeImporterBase::EclipseThemeImporterBase()
{
}

EclipseThemeImporterBase::~EclipseThemeImporterBase()
{
}

bool EclipseThemeImporterBase::GetProperty(const wxString& name, EclipseThemeImporterBase::Property& prop) const
{
    prop.colour = "";
    prop.isBold = false;
    prop.isItalic = false;
    if(!m_doc.IsOk())
        return false;

    wxXmlNode* child = m_doc.GetRoot()->GetChildren();
    while(child) {
        if(child->GetName() == name) {
            prop.colour = child->GetAttribute("color");
            prop.isBold = child->GetAttribute("bold", "false") == "true";
            prop.isItalic = child->GetAttribute("italic", "false") == "true";
            return true;
        }
        child = child->GetNext();
    }
    return false;
}

bool EclipseThemeImporterBase::Load(const wxFileName& eclipseXml)
{
    return m_doc.Load(eclipseXml.GetFullPath());
}

bool EclipseThemeImporterBase::IsDarkTheme() const
{
    // load the theme background colour
    EclipseThemeImporterBase::Property p;
    if(!GetProperty("background", p))
        return false;

    // test the colour
    return DrawingUtils::IsDark(p.colour);
}

wxString EclipseThemeImporterBase::GetName() const
{
    if(!IsValid())
        return "";
    return m_doc.GetRoot()->GetAttribute("name");
}

wxString EclipseThemeImporterBase::GetOutputFile(const wxString& language) const
{
    wxString name = GetName();
    name.MakeLower();

    // Normalize the file name
    name.Replace(" ", "_");
    name.Replace("::", "_");
    name.Replace("(", "_");
    name.Replace(")", "_");
    name.Replace(":", "_");
    name.Replace(",", "_");
    name.Replace(".", "_");
    name.Replace(";", "_");

    wxString xmlFileName;
    xmlFileName << "lexer_" << language.Lower() << "_" << name << ".xml";
    return xmlFileName;
}

void EclipseThemeImporterBase::AddProperty(wxXmlNode* properties,
                                           const wxString& id,
                                           const wxString& name,
                                           const wxString& colour,
                                           const wxString& bgColour,
                                           bool bold,
                                           bool italic)
{
    wxASSERT(!colour.IsEmpty());
    wxASSERT(!bgColour.IsEmpty());

    wxXmlNode* property = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Property");
    properties->AddChild(property);

    property->AddAttribute("Id", id);
    property->AddAttribute("Name", name);
    property->AddAttribute("Bold", bold ? "yes" : "no");
    property->AddAttribute("Italic", italic ? "yes" : "no");
    property->AddAttribute("Face", "");
    property->AddAttribute("Colour", colour);
    property->AddAttribute("BgColour", bgColour);
    property->AddAttribute("Underline", "no");
    property->AddAttribute("EolFilled", "no");
    property->AddAttribute("Alpha", "50");
    property->AddAttribute("Size", "11");
}

void EclipseThemeImporterBase::AddBaseProperties(wxXmlNode* node, const wxString& lang, const wxString& id)
{
    node->AddAttribute("Name", lang);
    node->AddAttribute("Theme", GetName());
    node->AddAttribute("IsActive", "No");
    node->AddAttribute("UseCustomTextSelFgColour", "Yes");
    node->AddAttribute("StylingWithinPreProcessor", "yes");
    node->AddAttribute("Id", id); // C++ lexer id is 3

    // Set keywords
    {
        wxXmlNode* keywords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "KeyWords0");
        node->AddChild(keywords);
        XmlUtils::SetNodeContent(keywords, GetKeywords0());
    }
    {
        wxXmlNode* keywords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "KeyWords1");
        node->AddChild(keywords);
        XmlUtils::SetNodeContent(keywords, GetKeywords1());
    }
    {
        wxXmlNode* keywords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "KeyWords2");
        node->AddChild(keywords);
        XmlUtils::SetNodeContent(keywords, GetKeywords2());
    }
    {
        wxXmlNode* keywords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "KeyWords3");
        node->AddChild(keywords);
        XmlUtils::SetNodeContent(keywords, GetKeywords3());
    }
    {
        wxXmlNode* keywords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "KeyWords4");
        node->AddChild(keywords);
        XmlUtils::SetNodeContent(keywords, GetKeywords4());
    }
    wxXmlNode* extensions = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Extensions");
    node->AddChild(extensions);
    XmlUtils::SetNodeContent(extensions, GetFileExtensions());
}

void EclipseThemeImporterBase::AddCommonProperties(wxXmlNode* propertiesNode)
{
    // Set the brace match based on the background colour
    Property background, foreground, selectionBackground, selectionForeground;

    GetProperty("background", background);
    GetProperty("foreground", foreground);
    GetProperty("selectionForeground", selectionForeground);
    GetProperty("selectionBackground", selectionBackground);
    
    wxString whitespaceColour;
    if(IsDarkTheme()) {
        // dark theme
        // Whitespace should be a bit lighether
        whitespaceColour = wxColour(background.colour).ChangeLightness(150).GetAsString(wxC2S_HTML_SYNTAX);
        AddProperty(propertiesNode, "34", "Brace match", "yellow", background.colour, true);
        AddProperty(propertiesNode, "35", "Brace bad match", "red", background.colour, true);
        AddProperty(propertiesNode, "37", "Indent Guide", whitespaceColour, background.colour);

    } else {
        // light theme
        whitespaceColour = wxColour(background.colour).ChangeLightness(50).GetAsString(wxC2S_HTML_SYNTAX);
        AddProperty(propertiesNode, "34", "Brace match", "black", "cyan", true);
        AddProperty(propertiesNode, "35", "Brace bad match", "black", "red", true);
        AddProperty(propertiesNode, "37", "Indent Guide", whitespaceColour, background.colour);
    }
    AddProperty(propertiesNode, "-1", "Fold Margin", background.colour, background.colour);
    AddProperty(propertiesNode, "-2", "Text Selection", selectionForeground.colour, selectionBackground.colour);
    AddProperty(propertiesNode, "-3", "Caret Colour", IsDarkTheme() ? "white" : "black", background.colour);
    AddProperty(propertiesNode, "-4", "Whitespace", whitespaceColour, background.colour);
    AddProperty(propertiesNode, "38", "Calltip", foreground.colour, background.colour);
}
