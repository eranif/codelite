#include "clEditorWordCharsLocker.h"

#include <algorithm>

clEditorXmlHelper::clEditorXmlHelper(wxStyledTextCtrl* stc)
    : m_stc(stc)
{
    static const wxString wordChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-";
    std::for_each(wordChars.begin(), wordChars.end(), [&](const wxChar& ch) { m_chars.insert((int)ch); });
}

clEditorXmlHelper::~clEditorXmlHelper() {}

wxString clEditorXmlHelper::GetXmlTagAt(int pos, int& startPos, int& endPos) const
{
    startPos = wxNOT_FOUND;
    endPos = wxNOT_FOUND;
    if(pos > m_stc->GetLastPosition() || pos < 0)
        return "";

    // going backward
    startPos = pos;
    int tmppos = startPos - 1;
    while(tmppos >= 0) {
        char ch = GetCharAt(tmppos);
        if(m_chars.count((int)ch)) {
            startPos = tmppos;
            --tmppos;
            continue;
        }
        break;
    }

    if((GetCharAt(startPos - 1) == (int)'/') && (GetCharAt(startPos - 2) == (int)'<')) {
        // a closing XML tag
        startPos -= 2;
    } else if(GetCharAt(startPos - 1) == (int)'<') {
        // an opening tag
        startPos -= 1;
    } else {
        // Not an XML tag
        return "";
    }

    endPos = pos;
    int lastPos = m_stc->GetLastPosition();
    while(endPos < lastPos) {
        char ch = GetCharAt(endPos);
        if(m_chars.count((int)ch)) {
            ++endPos;
            continue;
        }
        break;
    }

    // Did we get anything?
    if((GetCharAt(endPos) == (int)'>')) {
        // a closing XML tag
        endPos += 1;
    } else if((GetCharAt(endPos) != (int)' ') && (GetCharAt(endPos) != (int)'\t')) {
        // a tag can end in only 2 ways:
        // with '>' or with space | tab, this tag does not end with either
        return "";
    }
    return m_stc->GetTextRange(startPos, endPos);
}

int clEditorXmlHelper::GetCharAt(int pos) const
{
    if(pos < 0 || pos > m_stc->GetLastPosition())
        return 0;
    return m_stc->GetCharAt(pos);
}
