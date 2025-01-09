#pragma once

#include "codelite_exports.h"

#include <vector>
#include <wx/string.h>

/// Utility class for comparing between version string in the format of X.Y.Z
class WXDLLIMPEXP_CL clVersionString
{
public:
    clVersionString(const wxString& version_string);
    ~clVersionString();

    /// Return `1` if this version is greater than `other`. Return `-1` if `other` is greater than this
    /// Return `0` if they are equal
    int Compare(const wxString& other) const;

    const wxString& GetVersionString() const { return m_versionString; }

private:
    long number_at(size_t index) const;
    std::vector<long> m_numbers;
    wxString m_versionString;
};
