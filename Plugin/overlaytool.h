//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : overlaytool.h
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

#ifndef __overlaytool__
#define __overlaytool__

#include "codelite_exports.h"

#include <wx/bitmap.h>

/**
 * @class OverlayTool a simple tool which allows creating new images with overlay icons
 * @author eran
 * @date 08/31/09
 * @brief
 */
class WXDLLIMPEXP_SDK OverlayTool
{

    static wxBitmap ms_bmpOK;
    static wxBitmap ms_bmpConflict;
    static wxBitmap ms_bmpModified;

public:
    enum BmpType {
        Bmp_NoChange = -1,
        Bmp_OK = 0,
        Bmp_Modified = 1,
        Bmp_Conflict = 2,
    };

private:
    OverlayTool();
    virtual ~OverlayTool();

    wxBitmap DoAddBitmap(const wxBitmap& bmp, const wxColour& colour) const;

public:
    static OverlayTool& Get();

    wxBitmap CreateBitmap(const wxBitmap& orig, OverlayTool::BmpType type) const;
};

#endif // __overlaytool__
