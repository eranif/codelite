#include "ThemeImporterDockerfile.hpp"

ThemeImporterDockerfile::ThemeImporterDockerfile()
{
    SetKeywords0("echo cp rm ls chmod if then fi done touch for in do while switch else cd pwd  cat mkdir rmdir grep "
                 "awk print printf xargs find mv gzip tar bzip zip gunzip ADD ARG CMD COPY ENTRYPOINT ENV EXPOSE FROM "
                 "LABEL MAINTAINER ONBUILD RUN  STOPSIGNAL USER VOLUME WORKDIR");
    SetFileExtensions("dockerfile");
    m_langName = "dockerfile";
}

ThemeImporterDockerfile::~ThemeImporterDockerfile() {}

LexerConf::Ptr_t ThemeImporterDockerfile::Import(const wxFileName& theme_file)
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
    AddProperty(lexer, wxSTC_SH_HERE_Q, "Here Q", m_klass);
    AddProperty(lexer, wxSTC_SH_HERE_DELIM, "Here delim", m_klass);
    FinalizeImport(lexer);
    return lexer;
}
