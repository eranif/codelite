#ifndef CLANAGRAM_H
#define CLANAGRAM_H

#include "codelite_exports.h"
#include "wxStringHash.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL clAnagram
{
    std::unordered_map<wxChar, int> m_charCounts;
    wxString m_needle;

public:
    clAnagram(const wxString& needle);
    virtual ~clAnagram();

    /**
     * @brief check haystack contains this anagram
     */
    bool Matches(const wxString& haystack) const;

    /**
     * @brief search for needle in haystack in order
     */
    bool MatchesInOrder(const wxString& haystack) const;
};

#endif // CLANAGRAM_H
