#include "ThemeImporterLUA.hpp"

ThemeImporterLua::ThemeImporterLua()
{
    // Keywords
    SetKeywords0(
        "if elseif else then end do while nil true false in for and or function local not repeat return until");

    m_classesIndex = 4;
    m_functionsIndex = 1;
    m_localsIndex = 5; // user1
    SetFileExtensions("*.lua;*.glua;*.gluaw;");
    m_langName = "lua";
}

ThemeImporterLua::~ThemeImporterLua() {}

LexerConf::Ptr_t ThemeImporterLua::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_LUA);
    AddProperty(lexer, wxSTC_LUA_DEFAULT, "Default", m_editor.fg_colour, m_editor.bg_colour);
    AddProperty(lexer, wxSTC_LUA_COMMENT, "Block comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_LUA_COMMENTLINE, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_LUA_COMMENTDOC, "Comment doc", m_multiLineComment);
    AddProperty(lexer, wxSTC_LUA_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_LUA_WORD, "Word (Set 0)", m_keyword);
    AddProperty(lexer, wxSTC_LUA_WORD2, "Word (Set 1)", m_variable);
    AddProperty(lexer, wxSTC_LUA_WORD3, "Classes", m_klass);
    AddProperty(lexer, wxSTC_LUA_WORD4, "Functions", m_function);
    AddProperty(lexer, wxSTC_LUA_WORD5, "Secondary keywords and identifiers", m_variable);
    AddProperty(lexer, wxSTC_LUA_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_LUA_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_LUA_LITERALSTRING, "Literal string", m_string);
    AddProperty(lexer, wxSTC_LUA_PREPROCESSOR, "Preprocessor", m_string);
    AddProperty(lexer, wxSTC_LUA_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_LUA_IDENTIFIER, "Identifier", m_editor.fg_colour, m_editor.bg_colour);
    AddProperty(lexer, wxSTC_LUA_STRINGEOL, "String EOL", m_string);
    AddProperty(lexer, wxSTC_LUA_LABEL, "Label", m_klass);
    FinalizeImport(lexer);
    return lexer;
}
