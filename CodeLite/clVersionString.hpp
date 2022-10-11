#ifndef CLVERSIONSTRING_HPP
#define CLVERSIONSTRING_HPP

#include "codelite_exports.h"

#include <wx/string.h>

/// Utility class converting between version string -> number
/// Example: 1.2.3 -> 1*10^2 + 2*10^3 + 3*10^0 -> 123
class WXDLLIMPEXP_CL clVersionString
{
    wxString m_version_string;
    size_t m_number = 0;

public:
    clVersionString(const wxString& version_string);
    ~clVersionString();

    /// return the version as a number
    size_t to_number() const { return m_number; }
};

#endif // CLVERSIONSTRING_HPP
