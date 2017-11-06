#include "clAnagram.h"
#include <algorithm>
#include <wx/wxcrt.h>

clAnagram::clAnagram(const wxString& needle)
{
    std::for_each(needle.begin(), needle.end(), [&](wxChar ch) {
        ch = wxTolower(ch);
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
