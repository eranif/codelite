#include "clAnagram.h"
#include <algorithm>
#include <wx/wxcrt.h>

#define IS_WHITESPACE(ch) (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')

clAnagram::clAnagram(const wxString& needle, size_t flags)
    : m_flags(flags)
{
    Reset(needle, m_flags);
}

clAnagram::~clAnagram() {}

bool clAnagram::Matches(const wxString& haystack) const
{
    std::unordered_map<wxChar, int> charsCount = m_charCounts;
    for(size_t i = 0; i < haystack.size(); ++i) {
        wxChar ch = haystack[i];
        ch = wxTolower(ch);
        if(charsCount.count(ch)) {
            int& counter = charsCount[ch];
            counter--;
            if(counter == 0) { charsCount.erase(ch); }
            if(charsCount.empty()) { return true; }
        }
    }
    return false;
}

bool clAnagram::MatchesInOrder(const wxString& haystack) const
{
    if(m_needle.IsEmpty()) { return true; }
    size_t index = 0;
    size_t maxIndex = m_needle.size();
    for(size_t i = 0; i < haystack.size(); ++i) {
        wxChar ch = haystack[i];
        ch = wxTolower(ch);
        if(ch == m_needle[index]) { ++index; }
        if(maxIndex == index) { return true; }
    }
    return false;
}

void clAnagram::Reset(const wxString& needle, size_t flags)
{
    m_flags = flags;
    m_needle.Clear();
    if(HasFlag(eAnagramFlag::kIgnoreWhitespace)) {
        for(size_t i = 0; i < needle.size(); ++i) {
            wxChar ch = needle[i];
            if(IS_WHITESPACE(ch)) { continue; }
            m_needle.Append(wxTolower(ch));
        }
    } else {
        m_needle = needle.Lower();
    }

    m_charCounts.clear();
    std::for_each(m_needle.begin(), m_needle.end(), [&](wxChar ch) {
        if(m_charCounts.count(ch) == 0) {
            m_charCounts[ch] = 1;
        } else {
            m_charCounts[ch]++;
        }
    });
}

bool clAnagram::IsEmpty() const { return m_needle.IsEmpty(); }
