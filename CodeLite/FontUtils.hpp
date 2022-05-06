#ifndef FONTUTILS_HPP
#define FONTUTILS_HPP

#include <codelite_exports.h>
#include <wx/font.h>

namespace FontUtils
{
WXDLLIMPEXP_CL const wxString& GetFontInfo(const wxFont& font);
WXDLLIMPEXP_CL const wxString& GetFontInfo(const wxString& font_desc);
}
#endif // FONTUTILS_HPP
