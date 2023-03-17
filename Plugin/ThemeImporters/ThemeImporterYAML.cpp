#include "ThemeImporterYAML.hpp"

ThemeImporterYAML::ThemeImporterYAML()
{
    SetFileExtensions("*.yml;*.yaml");
    m_langName = "yaml";
}

ThemeImporterYAML::~ThemeImporterYAML() {}

LexerConf::Ptr_t ThemeImporterYAML::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_YAML);
    wxString errorColour;
    if(lexer->IsDark()) {
        errorColour = "PINK";
    } else {
        errorColour = "RED";
    }
    AddProperty(lexer, wxSTC_YAML_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_YAML_COMMENT, "Line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_YAML_IDENTIFIER, "Identifier", m_klass);
    AddProperty(lexer, wxSTC_YAML_KEYWORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_YAML_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_YAML_REFERENCE, "Reference", m_variable);
    AddProperty(lexer, wxSTC_YAML_DOCUMENT, "Document", m_editor);
    AddProperty(lexer, wxSTC_YAML_TEXT, "Text", m_string);
    AddProperty(lexer, wxSTC_YAML_ERROR, "Error", errorColour, m_editor.bg_colour);
    AddProperty(lexer, wxSTC_YAML_OPERATOR, "Operator", m_oper);
    FinalizeImport(lexer);
    return lexer;
}
