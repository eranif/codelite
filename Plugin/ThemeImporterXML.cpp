#include "ThemeImporterXML.hpp"

#include <wx/stc/stc.h>

ThemeImporterXML::ThemeImporterXML() { SetFileExtensions("*.xml;*.project;*.xrc;*.plist"); }

ThemeImporterXML::~ThemeImporterXML() {}

LexerConf::Ptr_t ThemeImporterXML::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "xml", 5);
    CHECK_PTR_RET_NULL(lexer);

    wxString defaultTextColour;
    if(IsDarkTheme()) {
        defaultTextColour = "white";
        AddProperty(lexer, wxSTC_H_DEFAULT, "Default", "white", m_editor.bg_colour);

    } else {
        defaultTextColour = "black";
        AddProperty(lexer, wxSTC_H_DEFAULT, "Default", "black", m_editor.bg_colour);
    }

    AddProperty(lexer, wxSTC_H_TAG, "Tag", m_keyword);
    AddProperty(lexer, wxSTC_H_TAGUNKNOWN, "Tag Unknown", m_keyword);
    AddProperty(lexer, wxSTC_H_ATTRIBUTE, "Attribute", m_klass);
    AddProperty(lexer, wxSTC_H_ATTRIBUTEUNKNOWN, "Attribute Unknown", m_klass);
    AddProperty(lexer, wxSTC_H_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_H_DOUBLESTRING, "Double String", m_string);
    AddProperty(lexer, wxSTC_H_SINGLESTRING, "Single String", m_string);
    AddProperty(lexer, wxSTC_H_OTHER, "Other", m_editor);
    AddProperty(lexer, wxSTC_H_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_H_ENTITY, "Entity", m_number);
    AddProperty(lexer, wxSTC_H_TAGEND, "Tag End", m_keyword);
    AddProperty(lexer, wxSTC_H_XMLSTART, "XML Start", m_keyword);
    AddProperty(lexer, wxSTC_H_XMLEND, "XML End", m_keyword);
    AddProperty(lexer, wxSTC_H_CDATA, "CDATA", m_editor);
    FinalizeImport(lexer);
    return lexer;
}
