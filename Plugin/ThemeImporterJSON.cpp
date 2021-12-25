#include "ThemeImporterJSON.hpp"

#include <wx/stc/stc.h>

#if wxCHECK_VERSION(3, 1, 0)
ThemeImporterJson::ThemeImporterJson()
{
    // normal keywords
    SetKeywords0("true false null");

    // LD JSON
    SetKeywords1("type context id name");

    SetFileExtensions("*.wxcp;*.json");
    m_langName = "json";
}

ThemeImporterJson::~ThemeImporterJson() {}

LexerConf::Ptr_t ThemeImporterJson::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_JSON);
    AddProperty(lexer, wxSTC_JSON_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_JSON_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_JSON_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_JSON_STRINGEOL, "String EOL", m_string);
    AddProperty(lexer, wxSTC_JSON_PROPERTYNAME, "Property Name", m_variable);
    AddProperty(lexer, wxSTC_JSON_ESCAPESEQUENCE, "Escape Sequence", m_string);
    AddProperty(lexer, wxSTC_JSON_LINECOMMENT, "Line Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_JSON_BLOCKCOMMENT, "Block Comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_JSON_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_JSON_URI, "URI", m_klass);
    AddProperty(lexer, wxSTC_JSON_COMPACTIRI, "URI", m_function);
    AddProperty(lexer, wxSTC_JSON_KEYWORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_JSON_LDKEYWORD, "LD Keyword", m_keyword);
    AddProperty(lexer, wxSTC_JSON_ERROR, "Error", m_editor);
    FinalizeImport(lexer);
    return lexer;
}
#endif // wx > 3.1.0
