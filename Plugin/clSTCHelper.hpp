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
};

#endif // CLSTCHELPER_HPP
