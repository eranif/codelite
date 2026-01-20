#include "clAnagram.h"

#include <wx/wxcrt.h>

#define IS_WHITESPACE(ch) (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')

clAnagram::clAnagram(const wxString& needle, size_t flags)
    : m_flags(flags)
{
    Reset(needle, m_flags);
}

bool clAnagram::MatchesInOrder(const wxString& haystack) const
{
    if(m_needle.IsEmpty()) { return true; }
    size_t index = 0;
    size_t maxIndex = m_needle.size();
    for (wxChar ch : haystack) {
        ch = wxTolower(ch);
        if (ch == m_needle[index]) {
            ++index;
        }
        if (maxIndex == index) {
            return true;
        }
    }
    return false;
}

void clAnagram::Reset(const wxString& needle, size_t flags)
{
    m_flags = flags;
    m_needle.Clear();
    if (HasFlag(eAnagramFlag::kIgnoreWhitespace)) {
        for (wxChar ch : needle) {
            if (IS_WHITESPACE(ch)) {
                continue;
            }
            m_needle.Append(wxTolower(ch));
        }
    } else {
        m_needle = needle.Lower();
    }

    m_charCounts.clear();
    for (wxChar ch : m_needle) {
        if(m_charCounts.count(ch) == 0) {
            m_charCounts[ch] = 1;
        } else {
            m_charCounts[ch]++;
        }
    }
}

bool clAnagram::IsEmpty() const { return m_needle.IsEmpty(); }
