#include "EclipseBatchThemeImporter.h"

EclipseBatchThemeImporter::EclipseBatchThemeImporter()
{
    SetKeywords0("rem set if exist errorlevel for in do break call chcp cd chdir choice cls country ctty date del "
                 "erase dir echo exit goto loadfix loadhigh mkdir md move path pause prompt rename ren rmdir rd shift "
                 "time type ver verify vol com con lpt nul color copy defined else not start off");
    SetFileExtensions("*.bat;*.batch");
}

EclipseBatchThemeImporter::~EclipseBatchThemeImporter() {}
LexerConf::Ptr_t EclipseBatchThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "batch", wxSTC_LEX_BATCH);
    AddProperty(lexer, wxSTC_BAT_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_WORD, "Word", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_LABEL, "Label", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_HIDE, "Hide (@)", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_COMMAND, "Command", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_IDENTIFIER, "Identifier", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_BAT_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
