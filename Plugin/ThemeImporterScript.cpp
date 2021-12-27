#include "ThemeImporterScript.hpp"

ThemeImporterScript::ThemeImporterScript()
{
    SetKeywords0("echo cp rm ls chmod if then fi done touch for in do while switch else cd pwd  cat mkdir rmdir grep "
                 "awk print printf xargs find mv gzip tar bzip zip gunzip local return exit function");
    SetFileExtensions("*.bash;*.sh;.tcsh;.bashrc;*.ksh;configure");
    m_langName = "script";
}

ThemeImporterScript::~ThemeImporterScript() {}

LexerConf::Ptr_t ThemeImporterScript::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_BASH);
    AddProperty(lexer, wxSTC_SH_DEFAULT, "Default", m_editor);

    wxString errorColour;
    if(lexer->IsDark()) {
        errorColour = "PINK";
    } else {
        errorColour = "RED";
    }

    AddProperty(lexer, wxSTC_SH_ERROR, "Error", errorColour, m_editor.bg_colour);
    AddProperty(lexer, wxSTC_SH_COMMENTLINE, "Line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_SH_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_SH_WORD, "Word (Set 0)", m_keyword);
    AddProperty(lexer, wxSTC_SH_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_SH_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_SH_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_SH_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_SH_SCALAR, "Scalar", m_editor);
    AddProperty(lexer, wxSTC_SH_PARAM, "Param", m_variable);
    AddProperty(lexer, wxSTC_SH_BACKTICKS, "Backticks", m_string);
    AddProperty(lexer, wxSTC_SH_HERE_Q, "Here Q", m_string);
    AddProperty(lexer, wxSTC_SH_HERE_DELIM, "Here delim", m_klass);
    FinalizeImport(lexer);
    return lexer;
}
