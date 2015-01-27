#include "EclipseThemeImporterBase.h"
#include "drawingutils.h"
#include "xmlutils.h"
#include <wx/colour.h>
#include "cl_standard_paths.h"
#include "globals.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"

EclipseThemeImporterBase::EclipseThemeImporterBase() {}

EclipseThemeImporterBase::~EclipseThemeImporterBase() {}

bool EclipseThemeImporterBase::GetProperty(const wxString& name, EclipseThemeImporterBase::Property& prop) const
{
    prop.colour = "";
    prop.isBold = false;
    prop.isItalic = false;
    if(!m_doc.IsOk()) return false;

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

wxXmlNode*
EclipseThemeImporterBase::InitializeImport(const wxFileName& eclipseXml, const wxString& langName, int langId)
{
    m_langName = langName;
    if(!m_doc.Load(eclipseXml.GetFullPath())) return NULL;

    wxXmlNode* lexer = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Lexer");
    m_codeliteDoc.SetRoot(lexer);

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    wxXmlNode* properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Properties");
    m_codeliteDoc.GetRoot()->AddChild(properties);

    // Read the basic properties
    if(!GetProperty("background", m_background)) return NULL;
    if(!GetProperty("foreground", m_foreground)) return NULL;
    if(!GetProperty("selectionForeground", m_selectionForeground)) return NULL;
    if(!GetProperty("selectionBackground", m_selectionBackground)) return NULL;
    if(!GetProperty("lineNumber", m_lineNumber)) return NULL;
    if(!GetProperty("singleLineComment", m_singleLineComment)) return NULL;
    if(!GetProperty("multiLineComment", m_multiLineComment)) return NULL;
    if(!GetProperty("number", m_number)) return NULL;
    if(!GetProperty("string", m_string)) return NULL;
    if(!GetProperty("operator", m_oper)) return NULL;
    if(!GetProperty("keyword", m_keyword)) return NULL;
    if(!GetProperty("class", m_klass)) {
        m_klass = m_foreground;
    }
    if(!GetProperty("localVariable", m_variable)) {
        m_variable = m_foreground;
    }
    if(!GetProperty("javadoc", m_javadoc)) {
        m_javadoc = m_multiLineComment;
    }

    if(!GetProperty("javadocKeyword", m_javadocKeyword)) {
        m_javadocKeyword = m_multiLineComment;
    }

    return properties;
}

bool EclipseThemeImporterBase::FinalizeImport(wxXmlNode* propertiesNode)
{
    AddCommonProperties(propertiesNode);
    wxString codeliteXmlFile =
        wxFileName(clStandardPaths::Get().GetUserLexersDir(), GetOutputFile(m_langName)).GetFullPath();
    
    // Update the lexer colours
    LexerConf::Ptr_t lexer(new LexerConf);
    lexer->FromXml(m_codeliteDoc.GetRoot());
    ColoursAndFontsManager::Get().UpdateLexerColours(lexer, true);
    wxXmlNode* xmlnode = lexer->ToXml();
    m_codeliteDoc.SetRoot(xmlnode);
    
    // Save the lexer to xml
    return ::SaveXmlToFile(&m_codeliteDoc, codeliteXmlFile);
}

bool EclipseThemeImporterBase::IsDarkTheme() const
{
    // load the theme background colour
    EclipseThemeImporterBase::Property p;
    if(!GetProperty("background", p)) return false;

    // test the colour
    return DrawingUtils::IsDark(p.colour);
}

wxString EclipseThemeImporterBase::GetName() const
{
    if(!IsValid()) return "";
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
                                           bool italic,
                                           bool isEOLFilled)
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
    property->AddAttribute("EolFilled", isEOLFilled ? "yes" : "no");
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
    Property background, foreground, selectionBackground, selectionForeground, lineNumber;

    GetProperty("background", background);
    GetProperty("foreground", foreground);
    GetProperty("selectionForeground", selectionForeground);
    GetProperty("selectionBackground", selectionBackground);
    GetProperty("lineNumber", lineNumber);

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
    AddProperty(propertiesNode,
                "33",
                "Line Numbers",
                lineNumber.colour,
                background.colour,
                lineNumber.isBold,
                lineNumber.isItalic);
}
