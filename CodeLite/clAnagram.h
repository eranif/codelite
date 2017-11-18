#ifndef CLANAGRAM_H
#define CLANAGRAM_H

#include "codelite_exports.h"
#include "wxStringHash.h"
#include <wx/string.h>

enum class eAnagramFlag {
    kNone = 0,
    kIgnoreWhitespace = (1 << 0),
};

class WXDLLIMPEXP_CL clAnagram
{
    std::unordered_map<wxChar, int> m_charCounts;
    wxString m_needle;
    size_t m_flags;

protected:
    bool HasFlag(eAnagramFlag flag) const { return (m_flags & (size_t)flag); }

public:
    clAnagram()
        : m_flags(0)
    {
    }
    clAnagram(const wxString& needle, size_t flags = 0);
    virtual ~clAnagram();

    /**
     * @brief reset the anagram with a new pattern
     * See eAnagramFlag for possible flags
     */
    void Reset(const wxString& needle, size_t flags = 0);

    /**
     * @brief check haystack contains this anagram
     */
    bool Matches(const wxString& haystack) const;

    /**
     * @brief search for needle in haystack in order
     */
    bool MatchesInOrder(const wxString& haystack) const;

    /**
     * @brief return true if the needle is an empty string
     */
    bool IsEmpty() const;
};

#endif // CLANAGRAM_H
