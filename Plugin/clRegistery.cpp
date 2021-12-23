#include "clRegistery.hpp"

#include <vector>

#ifdef __WXMSW__
#include "wx/msw/registry.h"
static wxSharedPtr<wxRegKey> TryCreateRegKey(wxRegKey::StdKey stdKey, wxRegKey::WOW64ViewMode mode, const wxString& key)
{
    wxSharedPtr<wxRegKey> regKey(new wxRegKey(stdKey, key, mode));
    if(!regKey->Exists()) {
        return wxSharedPtr<wxRegKey>(nullptr);
    }
    return regKey;
}

static wxSharedPtr<wxRegKey> CreateRegKey(const wxString& key)
{
    std::vector<wxRegKey::StdKey> K = { wxRegKey::HKCU, wxRegKey::HKLM, wxRegKey::HKLM };
    std::vector<wxRegKey::WOW64ViewMode> M = { wxRegKey::WOW64ViewMode_64, wxRegKey::WOW64ViewMode_32 };
    for(auto stdkey : K) {
        for(auto viewMode : M) {
            wxSharedPtr<wxRegKey> regKey = TryCreateRegKey(stdkey, viewMode, key);
            if(regKey) {
                return regKey;
            }
        }
    }
    return wxSharedPtr<wxRegKey>(nullptr);
}
#endif

clRegistery::clRegistery(const wxString& key)
#ifdef __WXMSW__
    : m_key(key)
#endif
{
#ifdef __WXMSW__
    m_regKey = CreateRegKey(m_key);
#endif
}

clRegistery::~clRegistery() {}

wxString clRegistery::ReadValueString(const wxString& valueName)
{
    wxUnusedVar(valueName);
#ifdef __WXMSW__
    if(!m_regKey) {
        return "";
    }
    wxString value;
    m_regKey->QueryValue(valueName, value);
    return value;
#endif
    return wxEmptyString;
}

wxString clRegistery::GetFirstChild()
{
    wxString childPath;
#ifdef __WXMSW__
    if(!m_regKey) {
        return "";
    }
    wxString keyname;
    if(!m_regKey->GetFirstKey(keyname, m_index)) {
        return "";
    }
    childPath << m_key << "\\" << keyname;
#endif
    return childPath;
}
