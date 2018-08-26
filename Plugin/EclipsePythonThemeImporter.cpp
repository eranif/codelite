#include "EclipsePythonThemeImporter.h"

EclipsePythonThemeImporter::EclipsePythonThemeImporter()
{
    SetKeywords0("and as assert break class continue def del elif else except exec finally for from global if import in "
                 "is lambda not or pass print raise return try while with yield");
    SetKeywords1("bool bytes bytearray dict float frozenset int list long set str tuple type unicode");
    SetKeywords2("__import__ abs all any ascii bin callable chr classmethod compile complex delattr dir divmod "
                 "enumerate eval filter format getattr globals hasattr hash help hex id input isinstance issubclass "
                 "iter len locals map max memoryview min next object oct open ord pow property range raw_input repr "
                 "reversed round setattr slice sorted staticmethod sum super vars zip");
    SetKeywords3("True False None");
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
