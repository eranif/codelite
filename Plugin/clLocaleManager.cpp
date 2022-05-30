#include "clLocaleManager.hpp"

#include <thread>
#include <unordered_set>
#include <wx/fontmap.h>
#include <wx/intl.h>
#include <wx/wxprec.h>

using namespace std;
clLocaleManager::clLocaleManager() {}

clLocaleManager::~clLocaleManager() {}

void clLocaleManager::load()
{
    auto entry_point = [](clLocaleManager* manager) {
        wxArrayString cached_locales;
        wxArrayString cached_locales_CanonicalName;
        int system_default_locale = wxNOT_FOUND;

        unordered_set<wxString> visited;

        int system_lang = wxLocale::GetSystemLanguage();
        if(system_lang == wxLANGUAGE_UNKNOWN) {
            // Least-stupid fallback value
            system_lang = wxLANGUAGE_ENGLISH_US;
        }

        // this operation takes time to perform, cache the locales
        for(int n = 0, lang = wxLANGUAGE_UNKNOWN + 1; lang < wxLANGUAGE_USER_DEFINED; ++lang) {
            const wxLanguageInfo* info = wxLocale::GetLanguageInfo(lang);
            // Check there *is* a Canonical name, as empty strings return a valid locale :/
            if((info && !info->CanonicalName.IsEmpty()) && wxLocale::IsAvailable(lang)) {

                // Check we haven't already seen this item: we may find the system default twice
                if(visited.insert(info->CanonicalName).second) {
                    // Display the name as e.g. "en_GB: English (U.K.)"
                    cached_locales.Add(info->CanonicalName + ": " + info->Description);
                    cached_locales_CanonicalName.Add(info->CanonicalName);

                    if(lang == system_lang) {
                        // Use this as the selection if preffered_locale isn't found
                        system_default_locale = n;
                    }
                    ++n;
                }
            }
        }
        LocaleParams d;
        d.cached_locales.swap(cached_locales);
        d.cached_locales_CanonicalName.swap(cached_locales_CanonicalName);
        d.system_default_locale = system_default_locale;
        manager->CallAfter(&clLocaleManager::OnScanCompleted, d);
    };

#ifdef __WXGTK__
    thread thr(entry_point, this);
    thr.detach();
#else
    entry_point(this);
#endif
}

void clLocaleManager::OnScanCompleted(const LocaleParams& params)
{
    m_cached_locales = params.cached_locales;
    m_cached_locales_CanonicalName = params.cached_locales_CanonicalName;
    m_system_default_locale = params.system_default_locale;
}

clLocaleManager& clLocaleManager::get()
{
    static clLocaleManager locale_mgr;
    return locale_mgr;
}