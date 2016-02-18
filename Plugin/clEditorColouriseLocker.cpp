#include "clEditorColouriseLocker.h"

clEditorColouriseLocker::clEditorColouriseLocker(wxStyledTextCtrl* stc)
    : m_stc(stc)
    , m_firstPos(0)
    , m_lastPos(0)
{
    m_firstPos = m_stc->PositionFromLine(m_stc->GetFirstVisibleLine());
    int lastLine = m_stc->GetFirstVisibleLine() + m_stc->LinesOnScreen();
    m_lastPos = m_stc->GetLineEndPosition(lastLine);
}

clEditorColouriseLocker::~clEditorColouriseLocker()
{
    if(m_firstPos > m_lastPos) {
        m_stc->Colourise(m_firstPos, m_lastPos);
    }
}
