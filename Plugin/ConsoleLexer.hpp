#pragma once

#include "codelite_exports.h"

#include <memory>
#include <wx/stc/stc.h>

#define CONSOLE_LEXER_DEFAULT 0
#define CONSOLE_LEXER_PYTHON 1
#define CONSOLE_LEXER_GCC_ERROR 2
#define CONSOLE_LEXER_GCC_INCLUDED_FROM 22
#define CONSOLE_LEXER_GCC_WARNING 56
#define CONSOLE_LEXER_GCC_EXCERPT 25
#define CONSOLE_LEXER_GCC_NOTE 57
#define CONSOLE_LEXER_ESCSEQ 23
#define CONSOLE_LEXER_ESCSEQ_UNKNOWN 24
#define CONSOLE_LEXER_ANSI_BLACK 40
#define CONSOLE_LEXER_ANSI_RED 41
#define CONSOLE_LEXER_ANSI_GREEN 42
#define CONSOLE_LEXER_ANSI_BROWN 43
#define CONSOLE_LEXER_ANSI_BLUE 44
#define CONSOLE_LEXER_ANSI_MAGENTA 45
#define CONSOLE_LEXER_ANSI_CYAN 46
#define CONSOLE_LEXER_ANSI_GRAY 47
#define CONSOLE_LEXER_ANSI_DARK_GRAY 48
#define CONSOLE_LEXER_ANSI_BRIGHT_RED 49
#define CONSOLE_LEXER_ANSI_BRIGHT_GREEN 50
#define CONSOLE_LEXER_ANSI_YELLOW 51
#define CONSOLE_LEXER_ANSI_BRIGHT_BLUE 52
#define CONSOLE_LEXER_ANSI_BRIGHT_MAGENTA 53
#define CONSOLE_LEXER_ANSI_BRIGHT_CYAN 54
#define CONSOLE_LEXER_ANSI_WHITE 55

class WXDLLIMPEXP_SDK ConsoleLexer : public wxEvtHandler
{
    using Ptr_t = std::unique_ptr<ConsoleLexer>;

public:
    ConsoleLexer(wxStyledTextCtrl* stc);
    virtual ~ConsoleLexer();

    /**
     * @brief initialize the control styles and colours
     */
    void SetStyles();

protected:
    void OnStyleNeeded(wxStyledTextEvent& e);

    wxStyledTextCtrl* m_stc = nullptr;
};

class WXDLLIMPEXP_SDK ConsoleLexerClientData : public wxClientData
{
public:
    ConsoleLexerClientData(wxStyledTextCtrl* ctrl)
        : m_lexer(ctrl)
    {
    }
    virtual ~ConsoleLexerClientData() {}

    ConsoleLexer& GetLexer() { return m_lexer; }

private:
    ConsoleLexer m_lexer;
};
