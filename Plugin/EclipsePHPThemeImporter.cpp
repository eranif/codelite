#include "EclipsePHPThemeImporter.h"
#include <wx/stc/stc.h>
#include "macros.h"

EclipsePHPThemeImporter::EclipsePHPThemeImporter()
{
    SetKeywords4("implements abstract and array as bool boolean break case cfunction class const continue declare "
                 "default die directory do double echo else elseif empty enddeclare endfor endforeach endif endswitch "
                 "endwhile eval exit extends false float for foreach function global if include include_once int "
                 "integer isset list new null object old_function or parent print real require require_once resource "
                 "return static stdclass string switch true unset use var while xor __class__ __file__ __function__ "
                 "__line__ __sleep __namespace__ __trait__ __wakeup public protected private trait interface namespace "
                 "final throw extends try catch self yield");
    
    // Javascript keywords
    SetKeywords1("break do instanceof  typeof case else new var catch finally return void continue for switch while "
                 "debugger function this with default if throw delete in try abstract  export interface  static "
                 "boolean  extends "
                 "long super"
                 "byte final native synchronized char float package throws class goto private transient const "
                 "implements  protected "
                 " volatile double import  public enum "
                 "int short null true false");
    SetFileExtensions("*.php;*.php3;*.phtml;*.inc;*.php5");
}

EclipsePHPThemeImporter::~EclipsePHPThemeImporter() {}

LexerConf::Ptr_t EclipsePHPThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "php", 4);
    CHECK_PTR_RET_NULL(lexer);

    // PHP can also be found in a file which contains HTML and JavaScript
    // so we define them all here

    // HTML
    AddProperty(lexer, wxSTC_H_DEFAULT, "HTML Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_TAG, "HTML Tag", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_TAGUNKNOWN, "Unknown HTML Tag", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_ATTRIBUTE, "HTML Attribute", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_ATTRIBUTEUNKNOWN, "Unknown HTML Attribute", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_NUMBER, "HTML Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_DOUBLESTRING, "HTML Double String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_SINGLESTRING, "HTML Single String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_OTHER, "HTML Other", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_COMMENT, "HTML Comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_H_ENTITY, "HTML Entity", m_foreground.colour, m_background.colour);

    // Embedded JavaScript
    AddProperty(lexer, wxSTC_HJ_START, "JS Start", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_DEFAULT, "JS Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_COMMENT, "JS Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_COMMENTLINE, "JS MultiLine Comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_COMMENTDOC, "JS Doc Comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_NUMBER, "JS Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_WORD, "JS Word", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_KEYWORD, "JS Keyword", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_DOUBLESTRING, "JS Double String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_SINGLESTRING, "JS Single String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_SYMBOLS, "JS Symbols", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_STRINGEOL, "JS String EOL", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HJ_REGEX, "JS Regex", m_klass.colour, m_background.colour);

    // PHP
    AddProperty(lexer, wxSTC_HPHP_COMPLEX_VARIABLE, "PHP Complex Variable", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_DEFAULT, "PHP Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_HSTRING, "PHP Heredoc String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_SIMPLESTRING, "PHP String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_WORD, "PHP Word", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_NUMBER, "PHP Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_VARIABLE, "PHP Variable", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_COMMENTLINE, "PHP Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_COMMENT, "PHP Multiline Comment", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_HPHP_HSTRING_VARIABLE, "PHP Heredoc Variable", m_variable.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
