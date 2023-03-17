#include "ThemeImporterRuby.hpp"

ThemeImporterRuby::ThemeImporterRuby()
{
    SetKeywords0("__ENCODING__ __LINE__ __FILE__ BEGIN END alias and begin break case class def defined? do else elsif "
                 "end ensure false for if in module next nil not or redo rescue retry return self super then true "
                 "undef unless until when while yield");
    SetFileExtensions("*.rb;Rakefile");
    m_langName = "ruby";
}

ThemeImporterRuby::~ThemeImporterRuby() {}

LexerConf::Ptr_t ThemeImporterRuby::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_RUBY);
    AddProperty(lexer, wxSTC_RB_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_RB_COMMENTLINE, "Single Line Comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_RB_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_RB_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_RB_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_RB_WORD, "Keyword", m_keyword);                 // keywords
    AddProperty(lexer, wxSTC_RB_WORD_DEMOTED, "Keyword demoted", m_keyword); // keywords
    AddProperty(lexer, wxSTC_RB_DEFNAME, "Def Name", m_function);
    AddProperty(lexer, wxSTC_RB_CLASSNAME, "Class Name", m_klass);
    AddProperty(lexer, wxSTC_RB_MODULE_NAME, "Module Name", m_enum);
    AddProperty(lexer, wxSTC_RB_SYMBOL, "Symbol", m_field);
    AddProperty(lexer, wxSTC_RB_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_RB_CLASS_VAR, "Class Variable", m_variable);
    AddProperty(lexer, wxSTC_RB_INSTANCE_VAR, "Instance Variable", m_variable);
    AddProperty(lexer, wxSTC_RB_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_RB_HERE_DELIM, "HERE delim", m_string);
    AddProperty(lexer, wxSTC_RB_HERE_Q, "HERE Q", m_string);
    AddProperty(lexer, wxSTC_RB_HERE_QQ, "HERE QQ", m_string);
    AddProperty(lexer, wxSTC_RB_HERE_QX, "HERE QX", m_string);
    AddProperty(lexer, wxSTC_RB_STRING_Q, "String Q", m_string);
    AddProperty(lexer, wxSTC_RB_STRING_QQ, "String QQ", m_string);
    AddProperty(lexer, wxSTC_RB_STRING_QX, "String QX", m_string);
    AddProperty(lexer, wxSTC_RB_STRING_QR, "String QR", m_string);
    AddProperty(lexer, wxSTC_RB_STRING_QW, "String QW", m_string);
    FinalizeImport(lexer);
    return lexer;
}
