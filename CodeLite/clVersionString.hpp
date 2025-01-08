#pragma once

#include "codelite_exports.h"

#include <vector>
#include <wx/string.h>

/// Utility class converting between version string -> number
/// Example: 1.2.3 -> 1*10^2 + 2*10^3 + 3*10^0 -> 123
class WXDLLIMPEXP_CL clVersionString
{

public:
    clVersionString(const wxString& version_string);
    ~clVersionString();

    /// Return `1` if this version is greater than `other`. Return `-1` if `other` is greater than this
    /// Return `0` if they are equal
    int Compare(const wxString& other) const;

private:
    long number_at(size_t index) const;
    std::vector<long> m_numbers;
};
