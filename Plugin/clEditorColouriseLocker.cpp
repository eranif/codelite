#include "clEditorColouriseLocker.h"

namespace
{
int GetLastVisibleLine(wxStyledTextCtrl* ctrl)
{
    int max_lines = ctrl->LinesOnScreen();
    int first_line = ctrl->GetFirstVisibleLine();
    int end_line = first_line;
    int last_line = ctrl->GetLineCount();

    int lines_collected = 0;
    for(int i = first_line; i < last_line && lines_collected < max_lines; ++i) {
        if(ctrl->GetLineVisible(i)) {
            // the line is visible - collect it
            ++lines_collected;
            end_line = i;
        }
    }
    return end_line;
}
} // namespace

clEditorColouriseLocker::clEditorColouriseLocker(wxStyledTextCtrl* stc)
    : m_stc(stc)
    , m_firstPos(0)
    , m_lastPos(0)
{
    m_firstPos = m_stc->PositionFromLine(m_stc->GetFirstVisibleLine());
    int lastLine = GetLastVisibleLine(m_stc);
    m_lastPos = m_stc->GetLineEndPosition(lastLine);
}

clEditorColouriseLocker::~clEditorColouriseLocker()
{
    if(m_firstPos > m_lastPos) {
        m_stc->Colourise(m_firstPos, m_lastPos);
    }
}
