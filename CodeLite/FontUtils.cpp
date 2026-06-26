#include "FontUtils.hpp"

#include <unordered_map>
#include <unordered_set>

namespace FontUtils
{
#ifdef __WXMSW__
const wxString DEFAULT_FACE_NAME = "Consolas";
constexpr int DEFAULT_FONT_SIZE = 14;
#elif defined(__WXMAC__)
const wxString DEFAULT_FACE_NAME = "monaco";
constexpr int DEFAULT_FONT_SIZE = 16;
#else // GTK, FreeBSD etc
const wxString DEFAULT_FACE_NAME = "Monospace";
constexpr int DEFAULT_FONT_SIZE = 14;
#endif

#ifdef __WXMSW__
const std::unordered_set<wxString> words = {"SemiBold", "Semibold", "Extended", "Semi Bold", "Semi bold"};
#endif

std::unordered_map<wxString, wxString> fixed_fonts_cache;

wxString GetFontInfo(const wxFont& font) { return font.GetNativeFontInfoDesc(); }

wxString GetFontInfo(const wxString& font_desc)
{
    return font_desc;
//    // check the cache first
//    if (fixed_fonts_cache.count(font_desc) != 0) {
//        return fixed_fonts_cache[font_desc];
//    }
//    wxString desc = font_desc;
//
//    // update the cache
//    fixed_fonts_cache.insert({font_desc, desc});
//    // return the cached entry
//    return fixed_fonts_cache[font_desc];
}

wxFont GetDefaultMonospacedFont()
{
    wxFontInfo fontInfo = wxFontInfo(DEFAULT_FONT_SIZE).Family(wxFONTFAMILY_MODERN).FaceName(DEFAULT_FACE_NAME);
    wxFont font(fontInfo);
    return font;
}
} // namespace FontUtils
