#include "clSTCHelper.hpp"

namespace
{
void DoSetCaretAt(wxStyledTextCtrl* ctrl, long pos)
{
    ctrl->SetCurrentPos(pos);
    ctrl->SetSelectionStart(pos);
    ctrl->SetSelectionEnd(pos);
    int line = ctrl->LineFromPosition(pos);
    if (line >= 0) {
        // This is needed to unfold the line if it were folded
        // The various other 'EnsureVisible' things don't do this
        ctrl->EnsureVisible(line);
    }
}
} // namespace

void clSTCHelper::ScrollRange(wxStyledTextCtrl* ctrl, int selection_start, int selection_end)
{
#if wxCHECK_VERSION(3, 1, 0)
    // ensure the selection is visible
    if (selection_end != selection_start) {
        ctrl->ScrollRange(selection_start, selection_end);
    }
    ctrl->EnsureCaretVisible(); // incase we are inside a folded area
#else
    // implement a wx30 version for ScrollRange()
    wxUnusedVar(selection_start);
    wxUnusedVar(selection_end);
    ctrl->EnsureCaretVisible(); // incase we are inside a folded area
#endif
}

void clSTCHelper::CenterLine(wxStyledTextCtrl* ctrl, int line, int col)
{
    int caret_pos = ctrl->PositionFromLine(line);
    if (col != wxNOT_FOUND) {
        // calculate the position
        caret_pos += col;
    }

    // move the caret to the requested line
    ctrl->EnsureVisible(line); // make sure the requested line is visible
    SetCaretAt(ctrl, caret_pos);

    ctrl->VerticalCentreCaret();
    ctrl->EnsureCaretVisible();
}

void clSTCHelper::SetCaretAt(wxStyledTextCtrl* ctrl, long pos)
{
    DoSetCaretAt(ctrl, pos);
    int line = ctrl->LineFromPosition(pos);

    // make sure that the line is visible (folded lines mainly)
    ctrl->EnsureVisible(line);

    // ensure caret is visible, but only if needed
    ScrollRange(ctrl, pos, pos);
}

void clSTCHelper::UpdateScrollbarWidth(wxStyledTextCtrl* ctrl, size_t char_width)
{
    // recalculate and set the length of horizontal scrollbar
    if (char_width == wxString::npos) {
        char_width = ctrl->TextWidth(0, "X");
    }

    int maxPixel = 0;
    int startLine = ctrl->GetFirstVisibleLine();
    int endLine = startLine + ctrl->LinesOnScreen();
    if (endLine >= (ctrl->GetLineCount() - 1)) {
        endLine--;
    }

    wxString text;
    for (int i = startLine; i <= endLine; i++) {
        int visibleLine = (int)ctrl->DocLineFromVisible(i); // get actual visible line, folding may offset lines
        wxString line_text = ctrl->GetLine(visibleLine);
        text = line_text.length() > text.length() ? line_text : text;
    }

    maxPixel = char_width * text.length();
    if (maxPixel == 0) {
        maxPixel++; // make sure maxPixel is valid
    }

    int currentLength = ctrl->GetScrollWidth(); // Get current scrollbar size
    if (currentLength != maxPixel) {
        // And if it is not the same, update it
        ctrl->SetScrollWidth(maxPixel);
    }
}

bool clSTCHelper::IsPositionInComment(wxStyledTextCtrl* ctrl, int pos)
{
    if (!ctrl) {
        return false;
    }

    // Get the position to check
    if (pos == -1) {
        pos = ctrl->GetCurrentPos();
    }

    // Get the style at the current position
    int style = ctrl->GetStyleAt(pos);

    // Get the current lexer
    int lexer = ctrl->GetLexer();

    // Check for comment styles based on lexer
    switch (lexer) {
    // C/C++, C#, Java, JavaScript, Go
    case wxSTC_LEX_CPP:
        return (style == wxSTC_C_COMMENT || style == wxSTC_C_COMMENTLINE || style == wxSTC_C_COMMENTDOC ||
                style == wxSTC_C_COMMENTLINEDOC || style == wxSTC_C_COMMENTDOCKEYWORD ||
                style == wxSTC_C_COMMENTDOCKEYWORDERROR);

    // Python
    case wxSTC_LEX_PYTHON:
        return (style == wxSTC_P_COMMENTLINE || style == wxSTC_P_COMMENTBLOCK);

    // HTML/XML
    case wxSTC_LEX_HTML:
    case wxSTC_LEX_XML:
        return (style == wxSTC_H_COMMENT || style == wxSTC_H_XCCOMMENT || style == wxSTC_H_SGML_COMMENT ||
                style == wxSTC_HJ_COMMENT || style == wxSTC_HJ_COMMENTLINE || style == wxSTC_HJ_COMMENTDOC ||
                style == wxSTC_HJA_COMMENT || style == wxSTC_HJA_COMMENTLINE || style == wxSTC_HJA_COMMENTDOC ||
                style == wxSTC_HB_COMMENTLINE || style == wxSTC_HBA_COMMENTLINE);

    // SQL
    case wxSTC_LEX_SQL:
        return (style == wxSTC_SQL_COMMENT || style == wxSTC_SQL_COMMENTLINE || style == wxSTC_SQL_COMMENTDOC ||
                style == wxSTC_SQL_COMMENTLINEDOC || style == wxSTC_SQL_COMMENTDOCKEYWORD ||
                style == wxSTC_SQL_COMMENTDOCKEYWORDERROR);

    // Perl
    case wxSTC_LEX_PERL:
        return (style == wxSTC_PL_COMMENTLINE);

    // Bash/Shell
    case wxSTC_LEX_BASH:
        return (style == wxSTC_SH_COMMENTLINE);

    // Lua
    case wxSTC_LEX_LUA:
        return (style == wxSTC_LUA_COMMENT || style == wxSTC_LUA_COMMENTLINE || style == wxSTC_LUA_COMMENTDOC);

    // Ruby
    case wxSTC_LEX_RUBY:
        return (style == wxSTC_RB_COMMENTLINE);

    // Pascal
    case wxSTC_LEX_PASCAL:
        return (style == wxSTC_PAS_COMMENT || style == wxSTC_PAS_COMMENT2 || style == wxSTC_PAS_COMMENTLINE);

    // Fortran
    case wxSTC_LEX_FORTRAN:
    case wxSTC_LEX_F77:
        return (style == wxSTC_F_COMMENT);

    // Batch files
    case wxSTC_LEX_BATCH:
        return (style == wxSTC_BAT_COMMENT);

    // Makefile
    case wxSTC_LEX_MAKEFILE:
        return (style == wxSTC_MAKE_COMMENT);

    // Properties/INI files
    case wxSTC_LEX_PROPERTIES:
        return (style == wxSTC_PROPS_COMMENT);

    // CSS
    case wxSTC_LEX_CSS:
        return (style == wxSTC_CSS_COMMENT);

    // PHP
    case wxSTC_LEX_PHPSCRIPT:
        return (style == wxSTC_HPHP_COMMENT || style == wxSTC_HPHP_COMMENTLINE);

    // VB/VBScript
    case wxSTC_LEX_VB:
    case wxSTC_LEX_VBSCRIPT:
        return (style == wxSTC_B_COMMENT);

    // TCL
    case wxSTC_LEX_TCL:
        return (style == wxSTC_TCL_COMMENT || style == wxSTC_TCL_COMMENTLINE || style == wxSTC_TCL_COMMENT_BOX ||
                style == wxSTC_TCL_BLOCK_COMMENT);

    // YAML
    case wxSTC_LEX_YAML:
        return (style == wxSTC_YAML_COMMENT);

    // JSON (technically no comments in standard JSON, but some editors support them)
    case wxSTC_LEX_JSON:
        return (style == wxSTC_JSON_LINECOMMENT || style == wxSTC_JSON_BLOCKCOMMENT);

    // Rust
    case wxSTC_LEX_RUST:
        return (style == wxSTC_RUST_COMMENTBLOCK || style == wxSTC_RUST_COMMENTLINE ||
                style == wxSTC_RUST_COMMENTBLOCKDOC || style == wxSTC_RUST_COMMENTLINEDOC);

    // Assembly
    case wxSTC_LEX_ASM:
        return (style == wxSTC_ASM_COMMENT || style == wxSTC_ASM_COMMENTBLOCK);

    // LISP/Scheme
    case wxSTC_LEX_LISP:
        return (style == wxSTC_LISP_COMMENT);

    // D
    case wxSTC_LEX_D:
        return (style == wxSTC_D_COMMENT || style == wxSTC_D_COMMENTLINE || style == wxSTC_D_COMMENTDOC ||
                style == wxSTC_D_COMMENTNESTED || style == wxSTC_D_COMMENTLINEDOC ||
                style == wxSTC_D_COMMENTDOCKEYWORD || style == wxSTC_D_COMMENTDOCKEYWORDERROR);

    // Erlang
    case wxSTC_LEX_ERLANG:
        return (style == wxSTC_ERLANG_COMMENT);

    // R
    case wxSTC_LEX_R:
        return (style == wxSTC_R_COMMENT);

    // Markdown
    case wxSTC_LEX_MARKDOWN:
        // Markdown doesn't have traditional comments, but code blocks might
        return false;

    // Default case - unknown lexer
    default:
        return false;
    }
}

void clSTCHelper::CopySettingsFrom(wxStyledTextCtrl* src, wxStyledTextCtrl* target)
{
    if (src == nullptr || target == nullptr) {
        return;
    }

    for (int style = 0; style < wxSTC_STYLE_MAX; ++style) {
        target->StyleSetForeground(style, src->StyleGetForeground(style));
        target->StyleSetBackground(style, src->StyleGetBackground(style));
    }
}
