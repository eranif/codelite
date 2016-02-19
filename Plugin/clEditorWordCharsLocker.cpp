#include "clEditorWordCharsLocker.h"
#include <algorithm>

clEditorWordCharsLocker::clEditorWordCharsLocker(wxStyledTextCtrl* stc, const wxString& wordChars)
    : m_stc(stc)
    , m_wordChars(wordChars)
{
    std::for_each(m_wordChars.begin(), m_wordChars.end(), [&](const wxChar& ch) { m_chars.insert((int)ch); });
}

clEditorWordCharsLocker::~clEditorWordCharsLocker() {}

wxString clEditorWordCharsLocker::GetWordAtPos(int pos, int& startPos, int& endPos) const
{
    startPos = wxNOT_FOUND;
    endPos = wxNOT_FOUND;
    if(pos > m_stc->GetLastPosition() || pos < 0) return "";

    if(pos > 0) {
        startPos = pos;
        int tmppos = startPos - 1;
        while(tmppos >= 0) {
            char ch = m_stc->GetCharAt(tmppos);
            if(m_chars.count((int)ch)) {
                startPos = tmppos;
                --tmppos;
                continue;
            }
            break;
        }
    } else {
        startPos = 0;
    }

    endPos = pos;
    int lastPos = m_stc->GetLastPosition();
    while(endPos < lastPos) {
        char ch = m_stc->GetCharAt(endPos);
        if(m_chars.count((int)ch)) {
            ++endPos;
            continue;
        }
        break;
    }

    return m_stc->GetTextRange(startPos, endPos);
}
