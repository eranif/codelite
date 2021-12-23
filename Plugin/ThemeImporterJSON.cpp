#include "ThemeImporterJSON.hpp"

#include <wx/stc/stc.h>

#if wxCHECK_VERSION(3, 1, 0)
ThemeImporterJson::ThemeImporterJson()
{
    SetKeywords0("true false null");
    SetFileExtensions("*.wxcp;*.json");
    m_langName = "json";
}

ThemeImporterJson::~ThemeImporterJson() {}

LexerConf::Ptr_t ThemeImporterJson::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_JSON);
    AddProperty(lexer, wxSTC_JSON_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_NUMBER, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_PROPERTYNAME, "Property Name", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_ESCAPESEQUENCE, "Escape Sequence", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_LINECOMMENT, "Line Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_BLOCKCOMMENT, "Block Comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_URI, "URI", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_COMPACTIRI, "URI", m_function.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_KEYWORD, "Keyword", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_LDKEYWORD, "LD Keyword", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_JSON_ERROR, "Error", m_foreground.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
#endif // wx > 3.1.0
