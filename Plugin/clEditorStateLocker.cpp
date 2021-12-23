#include "clEditorStateLocker.h"

#include "bookmark_manager.h"

clEditorStateLocker::clEditorStateLocker(wxStyledTextCtrl* ctrl)
    : m_ctrl(ctrl)
{
    // store the first visible line
    m_firstVisibleLine = m_ctrl->GetFirstVisibleLine();
    m_position = m_ctrl->GetCurrentPos();

    // Store the bookmarks
    SerializeBookmarks();
    // Store breakpoints
    SerializeBreakpoints();
    // Store the folds
    SerializeFolds();
}

clEditorStateLocker::~clEditorStateLocker()
{
    // restore the position.
    if(m_position > m_ctrl->GetLastPosition()) {
        m_position = m_ctrl->GetLastPosition();
    }

    // If the caret is out of screen, scroll the editor to make it visible again
    int caretLine = m_ctrl->LineFromPosition(m_position);
    if(caretLine < m_ctrl->GetFirstVisibleLine() ||
       (caretLine > (m_ctrl->GetFirstVisibleLine() + m_ctrl->LinesOnScreen()))) {
        // center the caret line
        m_ctrl->SetFirstVisibleLine(caretLine - (m_ctrl->LinesOnScreen() / 2));
    }

    m_ctrl->ClearSelections();
    m_ctrl->SetCurrentPos(m_position);
    m_ctrl->SetSelectionStart(m_position);
    m_ctrl->SetSelectionEnd(m_position);
    m_ctrl->EnsureVisible(m_ctrl->LineFromPosition(m_position));

    ApplyBookmarks();
    ApplyBreakpoints();
    ApplyFolds();

    m_ctrl->SetFirstVisibleLine(
        m_firstVisibleLine); // We must do this _after_ ApplyFolds() or the display may scroll down
}

void clEditorStateLocker::ApplyBookmarks() { ApplyBookmarks(m_ctrl, m_bookmarks); }
void clEditorStateLocker::SerializeBookmarks() { SerializeBookmarks(m_ctrl, m_bookmarks); }
void clEditorStateLocker::ApplyFolds() { ApplyFolds(m_ctrl, m_folds); }
void clEditorStateLocker::SerializeFolds() { SerializeFolds(m_ctrl, m_folds); }

void clEditorStateLocker::ApplyBookmarks(wxStyledTextCtrl* ctrl, const wxArrayString& bookmarks)
{
    for(size_t i = 0; i < bookmarks.GetCount(); i++) {
        // Unless this is an old file, each bookmark will have been stored in the form: "linenumber:type"
        wxString lineno = bookmarks.Item(i).BeforeFirst(':');
        long bmt = smt_bookmark1;
        wxString type = bookmarks.Item(i).AfterFirst(':');
        if(!type.empty()) {
            type.ToCLong(&bmt);
        }
        long line = 0;
        if(lineno.ToCLong(&line)) {
            ctrl->MarkerAdd(line, bmt);
        }
    }
}

void clEditorStateLocker::SerializeBookmarks(wxStyledTextCtrl* ctrl, wxArrayString& bookmarks)
{
    for(int line = 0; (line = ctrl->MarkerNext(line, mmt_all_bookmarks)) >= 0; ++line) {
        for(int type = smt_FIRST_BMK_TYPE; type <= smt_LAST_BMK_TYPE; ++type) {
            int mask = (1 << type);
            if(ctrl->MarkerGet(line) & mask) {
                // We need to serialise both the line and BM type. To keep things simple in sessionmanager, just merge
                // their strings
                bookmarks.Add(wxString::Format("%d:%d", line, type));
            }
        }
    }
}

void clEditorStateLocker::ApplyFolds(wxStyledTextCtrl* ctrl, const clEditorStateLocker::VecInt_t& folds)
{
    for(size_t i = 0; i < folds.size(); ++i) {
        int line = folds.at(i);
        // 'line' was collapsed when serialised, so collapse it now. That assumes that the line-numbers haven't changed
        // in the meanwhile.
        // If we cared enough, we could have saved a fold-level too, and/or the function name +/- the line's
        // displacement within the function. But for now...
        if(ctrl->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) {
#if wxVERSION_NUMBER >= 3100
            if(ctrl->GetFoldExpanded(line)) {
                ctrl->ToggleFoldShowText(line, "...");
            }
#else
            if(ctrl->GetFoldExpanded(line)) { // For <wx3.1 check first, and only toggle if needed
                ctrl->ToggleFold(line);
            }
#endif
        }
    }
}

void clEditorStateLocker::SerializeFolds(wxStyledTextCtrl* ctrl, clEditorStateLocker::VecInt_t& folds)
{
    for(int line = 0; line < ctrl->GetLineCount(); ++line) {
        if((ctrl->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) && (ctrl->GetFoldExpanded(line) == false)) {
            folds.push_back(line);
        }
    }
}

void clEditorStateLocker::ApplyBreakpoints(wxStyledTextCtrl* ctrl, const wxArrayString& breapoints)
{
    for(size_t i = 0; i < breapoints.GetCount(); i++) {
        // Unless this is an old file, each bookmark will have been stored in the form: "linenumber:type"
        wxString lineno = breapoints.Item(i).BeforeFirst(':');
        long bmt = smt_bookmark1;
        wxString type = breapoints.Item(i).AfterFirst(':');
        if(!type.empty()) {
            type.ToCLong(&bmt);
        }
        long line = 0;
        if(lineno.ToCLong(&line)) {
            ctrl->MarkerAdd(line, bmt);
        }
    }
}

void clEditorStateLocker::SerializeBreakpoints(wxStyledTextCtrl* ctrl, wxArrayString& breapoints)
{
    for(int line = 0; (line = ctrl->MarkerNext(line, mmt_all_breakpoints)) >= 0; ++line) {
        for(int type = smt_FIRST_BP_TYPE; type <= smt_LAST_BP_TYPE; ++type) {
            int mask = (1 << type);
            if(ctrl->MarkerGet(line) & mask) {
                // We need to serialise both the line and BM type. To keep things simple in sessionmanager, just merge
                // their strings
                breapoints.Add(wxString::Format("%d:%d", line, type));
            }
        }
    }
}

void clEditorStateLocker::ApplyBreakpoints() { ApplyBreakpoints(m_ctrl, m_breakpoints); }

void clEditorStateLocker::SerializeBreakpoints() { SerializeBreakpoints(m_ctrl, m_breakpoints); }
