#ifndef WXTERMINALANSIRENDERERINTERFACE_HPP
#define WXTERMINALANSIRENDERERINTERFACE_HPP

#include "clResult.hpp"
#include "codelite_exports.h"

#include <wx/colour.h>
#include <wx/string.h>
#include <wx/textctrl.h>

/// the interface for handling ANSI escape renderering
class WXDLLIMPEXP_SDK wxTerminalAnsiRendererInterface
{
protected:
    wxTextAttr m_defaultAttr;
    wxTextAttr m_curAttr;
    wxPoint m_pos = { 0, 0 };
    bool m_useDarkThemeColours = false;
    wxString m_windowTitle;

public:
    wxTerminalAnsiRendererInterface();
    virtual ~wxTerminalAnsiRendererInterface();

    virtual void Clear()
    {
        m_curAttr = m_defaultAttr;
        m_pos = { 0, 0 };
    }

    /// Makes an audible noise.
    virtual void Bell();

    /// Moves the cursor left (but may "backwards wrap" if cursor is at start of line).
    virtual void Backspace();

    /// Moves the cursor right to next multiple of 8.
    virtual void Tab();

    /// Moves to next line, scrolls the display up if at bottom of the screen. Usually does not move horizontally,
    /// though programs should not rely on this.
    virtual void LineFeed();

    /// Move a printer to top of next page. Usually does not move horizontally, though programs should not rely on this.
    /// Effect on video terminals varies. (^L)
    virtual void FormFeed();

    /// Moves the cursor to column zero.
    virtual void CarriageReturn();

    /// render text and move the cursor
    virtual void AddString(wxStringView str);

    /// Move carent n steps in direction
    virtual void MoveCaret(long n, wxDirection direction);
    /// move caret to X position
    virtual void SetCaretX(long n);

    /// move caret to Y position
    virtual void SetCaretY(long n);

    /// clear from caret left | right
    virtual void ClearLine(size_t dir = wxRIGHT | wxLEFT);

    /// replace n chars with space (going right)
    /// if at edge, insert the space
    virtual void EraseCharacter(int n);

    /// clear from caret up | down
    virtual void ClearDisplay(size_t dir = wxUP | wxDOWN);

    /// Set the window title
    virtual void SetWindowTitle(wxStringView window_title);

    /// reset the current attributes
    virtual void ResetStyle();

    /// Set the current text colour
    virtual void SetTextColour(const wxColour& col);

    /// Set the current text bg colour
    virtual void SetTextBgColour(const wxColour& col);

    /// Set the current text font
    virtual void SetTextFont(const wxFont& font);

    /// Initialise the default text attributes
    virtual void SetDefaultAttributes(const wxTextAttr& attr);

    bool IsUseDarkThemeColours() const { return m_useDarkThemeColours; }
    void SetUseDarkThemeColours(bool b) { m_useDarkThemeColours = b; }
    const wxString& GetWindowTitle() const { return m_windowTitle; }
};

#endif // WXTERMINALANSIRENDERERINTERFACE_HPP
