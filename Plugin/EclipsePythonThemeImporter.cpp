#include "EclipsePythonThemeImporter.h"

EclipsePythonThemeImporter::EclipsePythonThemeImporter()
{
    SetKeywords0("and assert break class continue def del elif else except exec finally for from global if import in "
                 "is lambda None not or pass print raise return try while yield");
    SetFileExtensions("*.py;waf;wscript;wscript_build");
    m_langName = "python";
}

EclipsePythonThemeImporter::~EclipsePythonThemeImporter() {}

LexerConf::Ptr_t EclipsePythonThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_PYTHON);
    AddProperty(lexer, wxSTC_P_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_WORD, "Word", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_TRIPLE, "Triple", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_TRIPLEDOUBLE, "Double Triple", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_CLASSNAME, "Class Name", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_DEFNAME, "Def Name", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_OPERATOR, "Operator", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_COMMENTBLOCK, "Comment block", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_WORD2, "Word 2", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_DECORATOR, "Decorator", m_klass.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
