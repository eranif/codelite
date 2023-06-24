#ifndef WXTERMINALANSIESCAPEHANDLER_HPP
#define WXTERMINALANSIESCAPEHANDLER_HPP

#include "clResult.hpp"
#include "codelite_exports.h"

#include <wx/string.h>

enum class wxHandleError {
    kNeedMoreData = 1,
    kProtocolError = 2,
    kNotFound = 3,
};

typedef clResult<wxStringView, wxHandleError> wxHandlResultStringView;

/// the interface for handling ANSI escape renderering
class WXDLLIMPEXP_SDK wxTerminalAnsiRendererInterface
{
public:
    wxTerminalAnsiRendererInterface() {}
    virtual ~wxTerminalAnsiRendererInterface() {}

    /// Makes an audible noise.
    virtual void Bell() {}
    /// Moves the cursor left (but may "backwards wrap" if cursor is at start of line).
    virtual void Backspace() {}
    /// Moves the cursor right to next multiple of 8.
    virtual void Tab() {}
    /// Moves to next line, scrolls the display up if at bottom of the screen. Usually does not move horizontally,
    /// though programs should not rely on this.
    virtual void LineFeed() {}
    /// Move a printer to top of next page. Usually does not move horizontally, though programs should not rely on this.
    /// Effect on video terminals varies. (^L)
    virtual void FormFeed() {}
    /// Moves the cursor to column zero.
    virtual void CarriageReturn() {}

    virtual void AddString(wxStringView str) { wxUnusedVar(str); }

    /// Move carent n steps in direction
    virtual void MoveCaret(long n, wxDirection direction)
    {
        wxUnusedVar(n);
        wxUnusedVar(direction);
    }
    virtual void SetCaretX(long n) { wxUnusedVar(n); }
    virtual void SetCaretY(long n) { wxUnusedVar(n); }
    /// clear from caret left | right
    virtual void ClearLine(size_t dir = wxRIGHT | wxLEFT) { wxUnusedVar(dir); }
    /// clear from caret up | down
    virtual void ClearDisplay(size_t dir = wxUP | wxDOWN) { wxUnusedVar(dir); }
    /// Set the window title
    virtual void SetWindowTitle(wxStringView window_title) { wxUnusedVar(window_title); }
};

class WXDLLIMPEXP_SDK wxTerminalAnsiEscapeHandler
{
private:
    wxHandlResultStringView handle_csi(wxStringView sv, wxTerminalAnsiRendererInterface* renderer);
    wxHandlResultStringView handle_osc(wxStringView sv, wxTerminalAnsiRendererInterface* renderer);
    // handle colour and style
    void handle_sgr(wxStringView sv, wxTerminalAnsiRendererInterface* renderer);
    wxHandlResultStringView loop_until_st(wxStringView sv);

public:
    wxTerminalAnsiEscapeHandler();
    virtual ~wxTerminalAnsiEscapeHandler();
    
    /**
     * @brief process buffer, return the number of chars consumed
     */
    size_t ProcessBuffer(wxStringView input, wxTerminalAnsiRendererInterface* renderer);
};

#endif // WXTERMINALANSIESCAPEHANDLER_HPP
