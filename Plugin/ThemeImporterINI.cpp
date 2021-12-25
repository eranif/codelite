#include "ThemeImporterINI.hpp"

ThemeImporterINI::ThemeImporterINI() { SetFileExtensions("*.ini;*.properties;*.desktop;*.toml"); }

ThemeImporterINI::~ThemeImporterINI() {}

LexerConf::Ptr_t ThemeImporterINI::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "properties", wxSTC_LEX_PROPERTIES);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, wxSTC_PROPS_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_PROPS_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_PROPS_SECTION, "Section", m_klass);
    AddProperty(lexer, wxSTC_PROPS_ASSIGNMENT, "Assignment", m_editor);
    AddProperty(lexer, wxSTC_PROPS_DEFVAL, "Default Value", m_editor);
    AddProperty(lexer, wxSTC_PROPS_KEY, "Key", m_variable);
    FinalizeImport(lexer);
    return lexer;
}
