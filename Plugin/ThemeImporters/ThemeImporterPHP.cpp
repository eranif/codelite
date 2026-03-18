#include "ThemeImporterPHP.hpp"

#include "macros.h"

#include <wx/stc/stc.h>

ThemeImporterPHP::ThemeImporterPHP()
{
    SetKeywords0("abstract and array as break case catch class clone const continue declare default "
                 "die do echo else elseif empty enddeclare endfor endforeach endif endswitch endwhile "
                 "enum eval exit extends final finally fn for foreach function global goto if implements "
                 "include include_once instanceof insteadof interface isset list match namespace new or "
                 "print private protected public readonly require require_once return static switch throw "
                 "trait try unset use var while xor yield yield from true false null "
                 "__halt_compiler __class__ __dir__ __file__ __function__ __line__ __method__ "
                 "__namespace__ __trait__ parent self static mixed never iterable object resource "
                 "int float string bool callable void");

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
