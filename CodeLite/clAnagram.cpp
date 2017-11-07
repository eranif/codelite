#include "clAnagram.h"
#include <algorithm>
#include <wx/wxcrt.h>

clAnagram::clAnagram(const wxString& needle)
    : m_needle(needle.Lower())
{
    std::for_each(m_needle.begin(), m_needle.end(), [&](wxChar ch) {
        if(m_charCounts.count(ch) == 0) {
            m_charCounts[ch] = 1;
        } else {
            m_charCounts[ch]++;
        }
    });
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
    if(m_needle.IsEmpty()) return true;
    size_t index = 0;
    size_t maxIndex = m_needle.size();
    for(size_t i = 0; i < haystack.size(); ++i) {
        wxChar ch = haystack[i];
        ch = wxTolower(ch);
        if(ch == m_needle[index]) { ++index; }
        if(maxIndex == index) return true;
    }
    return false;
}
