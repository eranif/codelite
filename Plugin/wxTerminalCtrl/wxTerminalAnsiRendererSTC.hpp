#ifndef WXTERMINALANSIRENDERERSTC_HPP
#define WXTERMINALANSIRENDERERSTC_HPP

#include "clResult.hpp"
#include "codelite_exports.h"
#include "wxTerminalAnsiRendererInterface.hpp"

#include <wx/colour.h>
#include <wx/stc/stc.h>
#include <wx/string.h>
#include <wx/textctrl.h>

class wxSTCStyleProvider;
class WXDLLIMPEXP_SDK wxTerminalAnsiRendererSTC : public wxTerminalAnsiRendererInterface
{
    wxStyledTextCtrl* m_ctrl = nullptr;
    wxSTCStyleProvider* m_stcStyleProvider = nullptr;
    int m_curstyle = 0;

protected:
    int GetInsertionPoint() const;
    void SetInsertionPoint();
    void InsertText(const wxString& str);

public:
    wxTerminalAnsiRendererSTC(wxStyledTextCtrl* ctrl);
    virtual ~wxTerminalAnsiRendererSTC();
    
    void Clear() override;
    /// Makes an audible noise
    void Bell() override;

    /// Moves the cursor left (but may "backwards wrap" if cursor is at start of line).
    void Backspace() override;

    /// Moves the cursor right to next multiple of 8.
    void Tab() override;

    /// Moves to next line, scrolls the display up if at bottom of the screen. Usually does not move horizontally,
    /// though programs should not rely on this.
    void LineFeed() override;

    /// Move a printer to top of next page. Usually does not move horizontally, though programs should not rely on this.
    /// Effect on video terminals varies. (^L)
    void FormFeed() override;

    /// Moves the cursor to column zero.
    void CarriageReturn() override;

    /// render text and move the cursor
    void AddString(wxStringView str) override;

    /// Move carent n steps in direction
    void MoveCaret(long n, wxDirection direction) override;

    /// move caret to X position
    void SetCaretX(long n) override;

    /// move caret to Y position
    void SetCaretY(long n) override;

    /// clear from caret left | right
    void ClearLine(size_t dir = wxRIGHT | wxLEFT) override;

    /// clear from caret up | down
    void ClearDisplay(size_t dir = wxUP | wxDOWN) override;

    /// Set the window title
    void SetWindowTitle(wxStringView window_title) override;

    /// reset the current attributes
    void ResetStyle() override;

    /// Set the current text colour
    void SetTextColour(const wxColour& col) override;

    /// Set the current text bg colour
    void SetTextBgColour(const wxColour& col) override;

    /// Set the current text font
    void SetTextFont(const wxFont& font) override;
    
    /// replace n chars with space (going right)
    /// if at edge, insert the space
    void EraseCharacter(int n) override;
};

#endif // WXTERMINALANSIRENDERERSTC_HPP
