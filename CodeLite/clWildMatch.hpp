#ifndef CLWILDMATCH_HPP
#define CLWILDMATCH_HPP

#include "codelite_exports.h"

#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

struct _Mask {
    wxString pattern;
    bool is_wild = false;

    _Mask(const wxString& pat, bool b)
        : pattern(pat)
        , is_wild(b)
    {
    }
};

using namespace std;
class WXDLLIMPEXP_CL clFileExtensionMatcher
{
    wxString m_mask;
    vector<_Mask> m_include_mask;
    bool m_always_matches = false;

public:
    clFileExtensionMatcher(const wxString& mask);
    ~clFileExtensionMatcher();

    bool matches(const wxString& filename) const;
};

class WXDLLIMPEXP_CL clPathExcluder
{
    wxString m_mask;
    vector<_Mask> m_exclude_mask;

public:
    clPathExcluder(const wxString& mask);
    ~clPathExcluder();
    bool is_exclude_path(const wxString& path) const;
};

#endif // CLWILDMATCH_HPP
