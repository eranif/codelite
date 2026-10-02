#ifndef FONTUTILS_HPP
#define FONTUTILS_HPP

#include "codelite_exports.h"

#include <wx/font.h>

namespace FontUtils
{
WXDLLIMPEXP_CL wxString GetFontInfo(const wxFont& font);
WXDLLIMPEXP_CL wxString GetFontInfo(const wxString& font_desc);

/// Return the default monospaced font: the bundled font if it was loaded (see SetBundledMonospacedFace), otherwise
/// a platform specific font
WXDLLIMPEXP_CL wxFont GetDefaultMonospacedFont();

/// Make `face_name` the default monospaced font. Call this only after the font was successfully loaded
/// (e.g. with wxFont::AddPrivateFont). Pass an empty string to go back to the platform font
WXDLLIMPEXP_CL void SetBundledMonospacedFace(const wxString& face_name);
} // namespace FontUtils

#endif // FONTUTILS_HPP
