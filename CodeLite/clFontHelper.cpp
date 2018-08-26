#include "clFontHelper.h"
#if wxUSE_GUI
#include <wx/tokenzr.h>

wxFont clFontHelper::FromString(const wxString& str)
{
    wxArrayString parts = ::wxStringTokenize(str, ";", wxTOKEN_STRTOK);
    if(parts.size() != 5) return wxNullFont;

    long iPointSize, iFamily, iWeight, iStyle;
    wxString facename = parts.Item(0);
    parts.Item(1).ToCLong(&iPointSize);
    parts.Item(2).ToCLong(&iFamily);
    parts.Item(3).ToCLong(&iWeight);
    parts.Item(4).ToCLong(&iStyle);

    bool bold = (iWeight == wxFONTWEIGHT_BOLD);
    bool italic = (iStyle == wxFONTSTYLE_ITALIC);
    wxFont font(wxFontInfo(iPointSize).Bold(bold).Italic(italic).FaceName(facename).Family((wxFontFamily)iFamily));
    return font;
}

wxString clFontHelper::ToString(const wxFont& font)
{
    if(!font.IsOk()) {
        return "";
    }
    wxString str;
    str << font.GetFaceName() << ";" << font.GetPointSize() << ";" << (int)font.GetFamily() << ";"
        << (int)font.GetWeight() << ";" << (int)font.GetStyle();
    return str;
}
#endif
