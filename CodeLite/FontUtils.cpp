#include "FontUtils.hpp"

#include "wxStringHash.h"

namespace FontUtils
{
#ifdef __WXMSW__
const wxString DEFAULT_FACE_NAME = "Consolas";
constexpr int DEFAULT_FONT_SIZE = 14;
#elif defined(__WXMAC__)
const wxString DEFAULT_FACE_NAME = "monaco";
constexpr int DEFAULT_FONT_SIZE = 14;
#else // GTK, FreeBSD etc
const wxString DEFAULT_FACE_NAME = "Monospace";
constexpr int DEFAULT_FONT_SIZE = 14;
#endif

#ifdef __WXMSW__
const std::unordered_set<wxString> words = { "SemiBold", "Semibold", "Extended", "Semi Bold", "Semi bold" };
#endif

std::unordered_map<wxString, wxString> fixed_fonts_cache;

const wxString& GetFontInfo(const wxFont& font) { return GetFontInfo(font.GetNativeFontInfoDesc()); }

const wxString& GetFontInfo(const wxString& font_desc)
{
    // check the cache first
    if(fixed_fonts_cache.count(font_desc) != 0) {
        return fixed_fonts_cache[font_desc];
    }

    wxString desc = font_desc;
#ifdef __WXMSW__
    // on MSW, we need to manipulate the info by remiving
    // "Semi Bold" (on all its variants) from teh font's info
    for(const wxString& word : words) {
        desc.Replace(word, wxEmptyString);
    }

    // replace all double spaces with a single one
    while(desc.Replace("  ", " "))
        ;
    desc.Trim();
#endif

    // update the cache
    fixed_fonts_cache.insert({ font_desc, desc });
    // return the cached entry
    return fixed_fonts_cache[font_desc];
}

wxFont GetDefaultMonospacedFont()
{
    wxFontInfo fontInfo = wxFontInfo(DEFAULT_FONT_SIZE).Family(wxFONTFAMILY_MODERN).FaceName(DEFAULT_FACE_NAME);
    wxFont font(fontInfo);
    return font;
}
} // namespace FontUtils
