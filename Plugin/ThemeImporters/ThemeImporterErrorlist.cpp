#include "ThemeImporterErrorlist.hpp"

#include "drawingutils.h"

ThemeImporterErrorlist::ThemeImporterErrorlist()
{
    SetFileExtensions("*.log");
    m_langName = "errorlist";
}

ThemeImporterErrorlist::~ThemeImporterErrorlist() {}
LexerConf::Ptr_t ThemeImporterErrorlist::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, m_langName, wxSTC_LEX_ERRORLIST);
    bool is_dark = DrawingUtils::IsDark(m_editor.bg_colour);

    AddProperty(lexer, wxSTC_ERR_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_ERR_PYTHON, "Python error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_GCC, "GCC like error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_MS, "MSVC error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_CMD, "CMD error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_BORLAND, "Borland error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_PERL, "Perl error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_NET, ".NET error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_LUA, "LUA error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_CTAG, "CTags line", Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_DIFF_CHANGED, "Diff line changed", Yellow(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_DIFF_ADDITION, "Diff line added", Green(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_DIFF_DELETION, "Diff line deleted", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_DIFF_MESSAGE, "Diff line message", Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_PHP, "PHP error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ELF, "Essential Lahey Fortran error message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_IFC, "Intel Fortran Compiler error/warning message", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_IFORT, "Intel Fortran Compiler v8.0 error/warning message", Red(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ABSF, "Absoft Pro Fortran 90/95 v8.2 error and/or warning message", Red(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_TIDY, "HTML tidy style", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_JAVA_STACK, "Java stack", Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_VALUE, "Value", m_editor);
    AddProperty(lexer, wxSTC_ERR_GCC_INCLUDED_FROM, "GCC 'included from'", Grey(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ESCSEQ, "ANSI escape sequence", m_editor);
    AddProperty(lexer, wxSTC_ERR_ESCSEQ_UNKNOWN, "ANSI escape sequence unknown", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_GCC_EXCERPT, "GCC code excerpt and pointer to issue", Cyan(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_BASH, "Bash diagnostic line", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BLACK, "ANSI escape black", Black(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_RED, "ANSI escape red", Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_GREEN, "ANSI escape green", Green(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BROWN, "ANSI escape brown", Magenta(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BLUE, "ANSI escape blue", Blue(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_MAGENTA, "ANSI escape magenta", Magenta(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_CYAN, "ANSI escape cyan", Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_GRAY, "ANSI escape grey", Grey(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_YELLOW, "ANSI escape yellow", Yellow(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_DARK_GRAY, "ANSI escape dark grey", Grey(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BRIGHT_RED, "ANSI escape bright red", Red(is_dark, true), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BRIGHT_GREEN, "ANSI escape bright green", Green(is_dark, true), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BRIGHT_BLUE, "ANSI escape bright blue", Blue(is_dark, true), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BRIGHT_MAGENTA, "ANSI escape bright magenta", Magenta(is_dark, true),
                m_editor.bg_colour);
    AddProperty(lexer, wxSTC_ERR_ES_BRIGHT_CYAN, "ANSI escape bright cyan", Cyan(is_dark, true), m_editor.bg_colour);
    FinalizeImport(lexer);
    return lexer;
}
