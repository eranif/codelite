#ifndef CLREGISTERY_HPP
#define CLREGISTERY_HPP

#include <codelite_exports.h>
#include <wx/string.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include <wx/sharedptr.h>
#endif

class WXDLLIMPEXP_SDK clRegistery
{
#ifdef __WXMSW__
    wxString m_key;
    wxSharedPtr<wxRegKey> m_regKey;
    long m_index = 0;
#endif

public:
    clRegistery(const wxString& key);
    virtual ~clRegistery();

    wxString ReadValueString(const wxString& valueName);
    /**
     * @brief return the full path of the first key child
     * @return empty string on error or full path of child key
     */
    wxString GetFirstChild();
};

#endif // CLREGISTERY_HPP
