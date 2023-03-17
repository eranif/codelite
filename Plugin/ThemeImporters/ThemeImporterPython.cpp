#include "ThemeImporterPython.hpp"

ThemeImporterPython::ThemeImporterPython()
{
    SetKeywords0(
        "and as assert break class continue def del elif else except exec finally for from global if import in "
        "is lambda not or pass print raise return try while with yield True False None pass self");
    SetFileExtensions("*.py;waf;wscript;wscript_build");
    SetLocalsWordSetIndex(1);
    SetLangName("python");
}

ThemeImporterPython::~ThemeImporterPython() {}

LexerConf::Ptr_t ThemeImporterPython::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_PYTHON);
    AddProperty(lexer, wxSTC_P_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Number", m_number);
    AddProperty(lexer, wxSTC_P_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_P_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_P_WORD, "Word", m_keyword); // keywords
    AddProperty(lexer, wxSTC_P_TRIPLE, "Triple", m_multiLineComment);
    AddProperty(lexer, wxSTC_P_TRIPLEDOUBLE, "Double Triple", m_multiLineComment);
    AddProperty(lexer, wxSTC_P_CLASSNAME, "Class Name", m_klass);
    AddProperty(lexer, wxSTC_P_DEFNAME, "Def Name", m_function);
    AddProperty(lexer, wxSTC_P_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_P_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_P_COMMENTBLOCK, "Comment block", m_multiLineComment);
    AddProperty(lexer, wxSTC_P_STRINGEOL, "String EOL", m_string);
    AddProperty(lexer, wxSTC_P_WORD2, "Locals", m_variable);
    AddProperty(lexer, wxSTC_P_DECORATOR, "Decorator", m_klass);
    FinalizeImport(lexer);
    return lexer;
}
