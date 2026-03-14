#include "ThemeImporterPHP.hpp"

#include "macros.h"

#include <wx/stc/stc.h>

ThemeImporterPHP::ThemeImporterPHP()
{
    SetKeywords0("implements abstract and array as bool boolean break case cfunction class const continue declare "
                 "default die directory do double echo else elseif empty enddeclare endfor endforeach endif endswitch "
                 "endwhile eval exit extends false float for foreach function global if include include_once int "
                 "integer isset list new null object old_function or parent print real require require_once resource "
                 "return static stdclass string switch true unset use var while xor __class__ __file__ __function__ "
                 "__line__ __sleep __namespace__ __trait__ __wakeup public protected private trait interface namespace "
                 "final throw extends try catch self yield");

    // Secondary keywords and identifiers
    SetFunctionsWordSetIndex(1);
    SetClassWordSetIndex(3);
    SetOthersWordSetIndex(4);
    SetLocalsWordSetIndex(LexerConf::WS_VARIABLES, true);

    // Special task markers
    // will be styled with SCE_C_TASKMARKER
    SetKeywords5("TODO FIXME BUG ATTN");
    SetLangName("php");
    SetFileExtensions("*.php;*.php3;*.phtml;*.inc;*.php5");
}

LexerConf::Ptr_t ThemeImporterPHP::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    // Convert to CodeLite's XML properties
    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_C_COMMENT, "Multi line comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTLINE, "Single line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTDOC, "Doc comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_C_WORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_C_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_C_STRINGRAW, "String Raw", m_string);
    AddProperty(lexer, wxSTC_C_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_C_UUID, "Uuid", m_number);
    AddProperty(lexer, wxSTC_C_PREPROCESSOR, "Preprocessor", m_editor);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_C_STRINGEOL, "Open String", m_string);
    AddProperty(lexer, wxSTC_C_COMMENTLINEDOC, "Doc single line comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORD, "Doc keyword", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORDERROR, "Doc keyword error", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_WORD2, "Function", m_function);
    AddProperty(lexer, wxSTC_C_GLOBALCLASS, "Class", m_klass);
    AddPropertySubstyle(lexer, LexerConf::WS_VARIABLES, "Variable", m_variable);

    // the base for all our substyles
    lexer->SetSubstyleBase(wxSTC_C_IDENTIFIER);

    FinalizeImport(lexer);
    return lexer;
}
