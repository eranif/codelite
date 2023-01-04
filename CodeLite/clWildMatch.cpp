#include "clWildMatch.hpp"

#include <algorithm>
#include <wx/filefn.h>
#include <wx/tokenzr.h>

namespace
{
void split_mask(const wxString& maskString, std::vector<_Mask>& includeMask, std::vector<_Mask>& excludeMask)
{
    wxArrayString masks = ::wxStringTokenize(maskString, ";,", wxTOKEN_STRTOK);
    for(wxString& mask : masks) {
        mask.Trim().Trim(false);
        // exclude mask starts with "!" or "-"
        if((mask[0] == '!') || (mask[0] == '-')) {
            mask.Remove(0, 1);
            excludeMask.push_back({ mask, ::wxIsWild(mask) });
        } else {
            includeMask.push_back({ mask, ::wxIsWild(mask) });
        }
    }
}

} // namespace
clFileExtensionMatcher::clFileExtensionMatcher(const wxString& mask)
    : m_mask(mask)
{
    std::vector<_Mask> dummy;
    wxArrayString masks = ::wxStringTokenize(m_mask, ";,", wxTOKEN_STRTOK);
    for(wxString& mask : masks) {
        mask.Replace("*", wxEmptyString);
        m_include_mask.push_back({ mask, false });
    }
    m_always_matches = false;
}

clFileExtensionMatcher::~clFileExtensionMatcher() {}

bool clFileExtensionMatcher::matches(const wxString& filename) const
{
    if(m_always_matches) {
        return true;
    }

    for(const _Mask& d : m_include_mask) {
        if(!d.is_wild && filename.EndsWith(d.pattern)) {
            return true;
        }
    }
    return false;
}

// clPathExcluder

clPathExcluder::clPathExcluder(const wxArrayString& patterns)
{
    m_exclude_mask.reserve(patterns.size());
    for(const auto& pattern : patterns) {
        m_exclude_mask.push_back({ pattern, ::wxIsWild(pattern) });
    }
}

clPathExcluder::clPathExcluder(const wxString& mask)
    : m_mask(mask)
{
    std::vector<_Mask> includeMask;
    split_mask(m_mask, includeMask, m_exclude_mask);
}

clPathExcluder::~clPathExcluder() {}

bool clPathExcluder::is_exclude_path(const wxString& str) const
{
    if(m_exclude_mask.empty()) {
        return false;
    }

    for(const auto& d : m_exclude_mask) {
        if((!d.is_wild && str.Contains(d.pattern)) ||    // use Contains match
           (d.is_wild && ::wxMatchWild(d.pattern, str))) // pattern matching
        {
            return true;
        }
    }
    return false;
}
