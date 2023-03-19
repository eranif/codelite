#include "ThemeImporterTCL.hpp"

#include <wx/stc/stc.h>

ThemeImporterTCL::ThemeImporterTCL()
{
    // Primary keywords
    SetKeywords0("break catch cd continue for foreach if proc unknown unset switch set return while");
    SetKeywords1(
        "append array close concat eof error eval exec exit expr file flush format gets glob global history incr info "
        "join lappend library lindex linsert list llength lrange lreplace lsearch lsort open pid puts pwd read regexp "
        "regsub rename scan seek source split string subst tclvars tell time trace uplevel upvar vwait assert "
        "assert_equal assert_match assert_error test");
    SetFileExtensions("*.tcl");
    SetFunctionsWordSetIndex(1);
    m_langName = "tcl";
}

ThemeImporterTCL::~ThemeImporterTCL() {}

LexerConf::Ptr_t ThemeImporterTCL::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_TCL);
    AddProperty(lexer, wxSTC_TCL_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_TCL_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_TCL_COMMENTLINE, "Comment line", m_singleLineComment);
    AddProperty(lexer, wxSTC_TCL_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_TCL_WORD_IN_QUOTE, "Word in quote", m_variable);
    AddProperty(lexer, wxSTC_TCL_IN_QUOTE, "In quote", m_string);
    AddProperty(lexer, wxSTC_TCL_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_TCL_BLOCK_COMMENT, "Block comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_TCL_COMMENT_BOX, "Comment box", m_multiLineComment);
    AddProperty(lexer, wxSTC_TCL_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_TCL_SUBSTITUTION, "Substitution", m_variable);
    AddProperty(lexer, wxSTC_TCL_SUB_BRACE, "Sub brace", m_singleLineComment);
    AddProperty(lexer, wxSTC_TCL_MODIFIER, "Modifier", m_oper);
    AddProperty(lexer, wxSTC_TCL_EXPAND, "Expand", m_variable);
    AddProperty(lexer, wxSTC_TCL_WORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_TCL_WORD2, "Function", m_function);
    AddProperty(lexer, wxSTC_TCL_WORD3, "Class", m_klass);
    AddProperty(lexer, wxSTC_TCL_WORD4, "Variable", m_variable);
    AddProperty(lexer, wxSTC_TCL_WORD5, "Word 5", m_editor);
    AddProperty(lexer, wxSTC_TCL_WORD6, "Word 6", m_editor);
    AddProperty(lexer, wxSTC_TCL_WORD7, "Word 7", m_editor);
    AddProperty(lexer, wxSTC_TCL_WORD8, "Word 8", m_editor);
    FinalizeImport(lexer);
    return lexer;
}
