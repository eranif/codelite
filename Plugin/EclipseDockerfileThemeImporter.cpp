#include "EclipseDockerfileThemeImporter.h"

EclipseDockerfileThemeImporter::EclipseDockerfileThemeImporter()
{
    SetKeywords0("echo cp rm ls chmod if then fi done touch for in do while switch else cd pwd  cat mkdir rmdir grep "
                 "awk print printf xargs find mv gzip tar bzip zip gunzip ADD ARG CMD COPY ENTRYPOINT ENV EXPOSE FROM "
                 "LABEL MAINTAINER ONBUILD RUN  STOPSIGNAL USER VOLUME WORKDIR");
    SetFileExtensions("dockerfile");
    m_langName = "dockerfile";
}

EclipseDockerfileThemeImporter::~EclipseDockerfileThemeImporter() {}

LexerConf::Ptr_t EclipseDockerfileThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_BASH);
    AddProperty(lexer, wxSTC_SH_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    wxString errorColour;
    if(lexer->IsDark()) {
        errorColour = "PINK";
    } else {
        errorColour = "RED";
    }

    AddProperty(lexer, wxSTC_SH_ERROR, "Error", errorColour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_COMMENTLINE, "Line comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_NUMBER, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_WORD, "Word (Set 0)", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_SCALAR, "Scalar", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_PARAM, "Param", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_BACKTICKS, "Backticks", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_HERE_Q, "Here Q", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SH_HERE_DELIM, "Here delim", m_klass.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
