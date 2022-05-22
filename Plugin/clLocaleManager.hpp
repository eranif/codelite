#ifndef CLLOCALEMANAGER_HPP
#define CLLOCALEMANAGER_HPP

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/event.h>

struct LocaleParams {
    wxArrayString cached_locales;
    wxArrayString cached_locales_CanonicalName;
    int system_default_locale = wxNOT_FOUND;
};

class WXDLLIMPEXP_SDK clLocaleManager : public wxEvtHandler
{
    wxArrayString m_cached_locales;
    wxArrayString m_cached_locales_CanonicalName;
    int m_system_default_locale = wxNOT_FOUND;

private:
    clLocaleManager();
    ~clLocaleManager();

    void OnScanCompleted(const LocaleParams& params);

public:
    static clLocaleManager& get();
    void load();

    void SetCachedLocales(const wxArrayString& cached_locales) { this->m_cached_locales = cached_locales; }
    void SetCachedLocalesCanonicalName(const wxArrayString& cached_locales_CanonicalName)
    {
        this->m_cached_locales_CanonicalName = cached_locales_CanonicalName;
    }
    void SetSystemDefaultLocale(int system_default_locale) { this->m_system_default_locale = system_default_locale; }
    const wxArrayString& GetCachedLocales() const { return m_cached_locales; }
    const wxArrayString& GetCachedLocalesCanonicalName() const { return m_cached_locales_CanonicalName; }
    int GetSystemDefaultLocale() const { return m_system_default_locale; }
};

#endif // CLLOCALEMANAGER_HPP
