#include "ThemeImporterLUA.hpp"

ThemeImporterLua::ThemeImporterLua()
{
    SetKeywords0(
        "if elseif else then end do while nil true false in for and or function local not repeat return until");
    SetFileExtensions("*.lua;*.glua;*.gluaw;");
    m_langName = "lua";
}

ThemeImporterLua::~ThemeImporterLua() {}

LexerConf::Ptr_t ThemeImporterLua::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_LUA);
    AddProperty(lexer, wxSTC_LUA_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_COMMENT, "Block comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_COMMENTLINE, "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_COMMENTDOC, "Comment doc", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_NUMBER, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_WORD, "Word (Set 0)", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_WORD2, "Word (Set 1)", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_WORD3, "Word (Set 2)", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_WORD4, "Word (Set 3)", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_WORD5, "Word (Set 4)", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_LITERALSTRING, "Literal string", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_PREPROCESSOR, "Preprocessor", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_LUA_LABEL, "Label", m_klass.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
