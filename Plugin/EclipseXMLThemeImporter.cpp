#include "EclipseXMLThemeImporter.h"
#include <wx/stc/stc.h>

EclipseXMLThemeImporter::EclipseXMLThemeImporter() { SetFileExtensions("*.xml;*.project;*.xrc;*.plist"); }

EclipseXMLThemeImporter::~EclipseXMLThemeImporter() {}

LexerConf::Ptr_t EclipseXMLThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "xml", 5);
    CHECK_PTR_RET_NULL(lexer);

    wxString defaultTextColour;
    if(IsDarkTheme()) {
        defaultTextColour = "white";
        AddProperty(lexer, wxSTC_H_DEFAULT, "Default", "white", m_background.colour);

    } else {
        defaultTextColour = "black";
        AddProperty(lexer, wxSTC_H_DEFAULT, "Default", "black", m_background.colour);
    }

    AddProperty(lexer, wxSTC_H_TAG, "Tag", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_TAGUNKNOWN, "Tag Unknown", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_ATTRIBUTE, "Attribute", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_ATTRIBUTEUNKNOWN, "Attribute Unknown", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_NUMBER, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_DOUBLESTRING, "Double String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_SINGLESTRING, "Single String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_OTHER, "Other", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_ENTITY, "Entity", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_TAGEND, "Tag End", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_XMLSTART, "XML Start", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_XMLEND, "XML End", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_CDATA, "CDATA", defaultTextColour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
