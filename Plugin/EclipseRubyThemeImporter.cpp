#include "EclipseRubyThemeImporter.hpp"

EclipseRubyThemeImporter::EclipseRubyThemeImporter()
{
    SetKeywords0("__ENCODING__ __LINE__ __FILE__ BEGIN END alias and begin break case class def defined? do else elsif "
                 "end ensure false for if in module next nil not or redo rescue retry return self super then true "
                 "undef unless until when while yield");
    SetFileExtensions("*.rb;Rakefile");
    m_langName = "ruby";
}

EclipseRubyThemeImporter::~EclipseRubyThemeImporter() {}

LexerConf::Ptr_t EclipseRubyThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_RUBY);
    AddProperty(lexer, wxSTC_RB_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_COMMENTLINE, "Single Line Comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_NUMBER, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_WORD, "Word", m_keyword.colour, m_background.colour); // keywords
    AddProperty(lexer, wxSTC_RB_DEFNAME, "Def Name", m_function.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_CLASSNAME, "Class Name", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_MODULE_NAME, "Module Name", m_enum.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_SYMBOL, "Symbol", m_field.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_CLASS_VAR, "Class Variable", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_INSTANCE_VAR, "Instance Variable", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_HERE_DELIM, "HERE delim", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_HERE_Q, "HERE Q", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_HERE_QQ, "HERE QQ", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_HERE_QX, "HERE QX", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_STRING_Q, "String Q", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_STRING_QQ, "String QQ", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_STRING_QX, "String QX", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_STRING_QR, "String QR", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RB_STRING_QW, "String QW", m_string.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
