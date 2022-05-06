#include "FontUtils.hpp"

#include <wxStringHash.h>

using namespace std;
namespace FontUtils
{
const unordered_set<wxString> words = { "SemiBold", "Semibold", "Extended" };
unordered_map<wxString, wxString> fixed_fonts_cache;

const wxString& GetFontInfo(const wxFont& font) { return GetFontInfo(font.GetNativeFontInfoDesc()); }

const wxString& GetFontInfo(const wxString& font_desc)
{
    // check the cache first
    if(fixed_fonts_cache.count(font_desc) != 0) {
        return fixed_fonts_cache[font_desc];
    }

    wxString desc = font_desc;
    for(const wxString& word : words) {
        desc.Replace(word, wxEmptyString);
    }

    // replace all double spaces with a single one
    while(desc.Replace("  ", " "))
        ;
    desc.Trim();

    // update the cache
    fixed_fonts_cache.insert({ font_desc, desc });
    // return the cached entry
    return fixed_fonts_cache[font_desc];
}
} // namespace FontUtils
