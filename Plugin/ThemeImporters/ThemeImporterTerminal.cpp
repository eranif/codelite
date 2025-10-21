#include "ThemeImporterTerminal.hpp"

#include "clColours.h"
#include "drawingutils.h"

#include <ExtraLexers.h>

#if wxCHECK_VERSION(3, 3, 0)
#define HAS_ILEXER 1
#else
#define HAS_ILEXER 0
#endif

ThemeImporterTerminal::ThemeImporterTerminal()
{
    SetFileExtensions("*.log");
    m_langName = "terminal";
}

LexerConf::Ptr_t ThemeImporterTerminal::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, m_langName, wxSTC_LEX_TERMINAL);
    bool is_dark = DrawingUtils::IsDark(m_editor.bg_colour);

    AddProperty(lexer, wxSTC_TERMINAL_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_TERMINAL_PYTHON, "Python error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_GCC, "GCC like error message", clColours::Red(is_dark), m_editor.bg_colour);

    AddProperty(
        lexer, wxSTC_TERMINAL_GCC_WARNING, "GCC like warning message", clColours::Yellow(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_GCC_NOTE, "GCC like note message", clColours::Cyan(is_dark), m_editor.bg_colour);

    AddProperty(lexer, wxSTC_TERMINAL_MS, "MSVC error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_CMD, "CMD error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_BORLAND, "Borland error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_PERL, "Perl error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_NET, ".NET error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_LUA, "LUA error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_CTAG, "CTags line", clColours::Cyan(is_dark), m_editor.bg_colour);
    AddProperty(
        lexer, wxSTC_TERMINAL_DIFF_CHANGED, "Diff line changed", clColours::Yellow(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_DIFF_ADDITION, "Diff line added", clColours::Green(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_DIFF_DELETION, "Diff line deleted", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_DIFF_MESSAGE, "Diff line message", clColours::Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_PHP, "PHP error message", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ELF,
                "Essential Lahey Fortran error message",
                clColours::Red(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_IFC,
                "Intel Fortran Compiler error/warning message",
                clColours::Red(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_IFORT,
                "Intel Fortran Compiler v8.0 error/warning message",
                clColours::Red(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ABSF,
                "Absoft Pro Fortran 90/95 v8.2 error and/or warning message",
                clColours::Red(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_TIDY, "HTML tidy style", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_JAVA_STACK, "Java stack", clColours::Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_VALUE, "Value", m_editor);
    AddProperty(
        lexer, wxSTC_TERMINAL_GCC_INCLUDED_FROM, "GCC 'included from'", clColours::Grey(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ESCSEQ, "ANSI escape sequence", m_editor);
    AddProperty(lexer,
                wxSTC_TERMINAL_ESCSEQ_UNKNOWN,
                "ANSI escape sequence unknown",
                clColours::Red(is_dark),
                m_editor.bg_colour);

    AddProperty(lexer,
                wxSTC_TERMINAL_GCC_EXCERPT,
                "GCC code excerpt and pointer to issue",
                clColours::Cyan(is_dark),
                m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_BASH, "Bash diagnostic line", clColours::Red(is_dark), m_editor.bg_colour);

    AddProperty(lexer, wxSTC_TERMINAL_ES_BLACK, "ANSI escape black", clColours::Black(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_RED, "ANSI escape red", clColours::Red(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_GREEN, "ANSI escape green", clColours::Green(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_BROWN, "ANSI escape brown", clColours::Yellow(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_BLUE, "ANSI escape blue", clColours::Blue(is_dark), m_editor.bg_colour);
    AddProperty(
        lexer, wxSTC_TERMINAL_ES_MAGENTA, "ANSI escape magenta", clColours::Magenta(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_CYAN, "ANSI escape cyan", clColours::Cyan(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_GRAY, "ANSI escape grey", clColours::Grey(is_dark), m_editor.bg_colour);
    AddProperty(lexer, wxSTC_TERMINAL_ES_YELLOW, "ANSI escape yellow", clColours::Yellow(is_dark), m_editor.bg_colour);
    AddProperty(
        lexer, wxSTC_TERMINAL_ES_DARK_GRAY, "ANSI escape dark grey", clColours::Grey(is_dark), m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ES_BRIGHT_RED,
                "ANSI escape bright red",
                clColours::Red(is_dark, true),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ES_BRIGHT_GREEN,
                "ANSI escape bright green",
                clColours::Green(is_dark, true),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ES_BRIGHT_BLUE,
                "ANSI escape bright blue",
                clColours::Blue(is_dark, true),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ES_BRIGHT_MAGENTA,
                "ANSI escape bright magenta",
                clColours::Magenta(is_dark, true),
                m_editor.bg_colour);
    AddProperty(lexer,
                wxSTC_TERMINAL_ES_BRIGHT_CYAN,
                "ANSI escape bright cyan",
                clColours::Cyan(is_dark, true),
                m_editor.bg_colour);
    FinalizeImport(lexer);
    return lexer;
}
