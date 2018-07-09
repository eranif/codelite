//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clFontHelper.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLFONTHELPER_H
#define CLFONTHELPER_H

#include "codelite_exports.h"
#if wxUSE_GUI
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
#endif
#endif // CLFONTHELPER_H
