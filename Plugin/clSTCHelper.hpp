#ifndef CLSTCHELPER_HPP
#define CLSTCHELPER_HPP

#include "codelite_exports.h"

#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clSTCHelper
{
public:
    /// Scroll the argument positions and the range between them into view giving
    /// priority to the `selection_start` position then the `selection_end` position.
    /// This may be used to make a search match visible.
    static void ScrollRange(wxStyledTextCtrl* ctrl, int selection_start, int selection_end);

    /// center the line + column in the editor
    static void CenterLine(wxStyledTextCtrl* ctrl, int line, int col = wxNOT_FOUND);

    /// Set the caret at the given `pos` and ensure its visible
    static void SetCaretAt(wxStyledTextCtrl* ctrl, long pos);

    /// Update the scrollbar width to match the visible view
    static void UpdateScrollbarWidth(wxStyledTextCtrl* ctrl, size_t char_width = wxString::npos);

    /**
     * @brief Determines if a given position in the text control is within a comment.
     *
     * This function checks whether the specified position (or current cursor position if not specified)
     * in a wxStyledTextCtrl is within a comment based on the active lexer. It supports multiple
     * programming languages including C/C++, Python, HTML/XML, SQL, Perl, Bash, Lua, Ruby, Pascal,
     * Fortran, Batch, Makefile, Properties, CSS, PHP, VB/VBScript, TCL, YAML, JSON, Rust, Assembly,
     * LISP, D, Erlang, R, and Markdown.
     *
     * @param ctrl Pointer to the wxStyledTextCtrl to check. If NULL, returns false.
     * @param pos The position in the text to check. If -1 (default), uses the current cursor position.
     * @return true if the position is within a comment according to the active lexer, false otherwise.
     */
    static bool IsPositionInComment(wxStyledTextCtrl* ctrl, int pos = wxSTC_INVALID_POSITION);
};

#endif // CLSTCHELPER_HPP
