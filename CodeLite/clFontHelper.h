#ifndef CLFONTHELPER_H
#define CLFONTHELPER_H

#include "codelite_exports.h"
#include <wx/font.h>

class WXDLLIMPEXP_CL clFontHelper
{
public:
    /**
     * @brief serialize font into wxString
     */
    static wxString ToString(const wxFont& font);
    /**
     * @brief unserialize and construct a wxFont from
     * a string
     */
    static wxFont FromString(const wxString& str);
};

#endif // CLFONTHELPER_H
