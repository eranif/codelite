//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : overlaytool.cpp
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

#include "overlaytool.h"

#include "bitmap_loader.h"
#include "drawingutils.h"
#include "globals.h"
#include "imanager.h"
#include "wx/rawbmp.h"

#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

wxBitmap OverlayTool::ms_bmpOK;
wxBitmap OverlayTool::ms_bmpConflict;
wxBitmap OverlayTool::ms_bmpModified;

OverlayTool::OverlayTool()
{
    BitmapLoader* bl = clGetManager()->GetStdIcons();
    ms_bmpOK = bl->LoadBitmap("overlay/16/ok");
    ms_bmpModified = bl->LoadBitmap("overlay/16/modified");
    ms_bmpConflict = bl->LoadBitmap("overlay/16/conflicted");
}

OverlayTool::~OverlayTool() {}

wxBitmap OverlayTool::DoAddBitmap(const wxBitmap& bmp, const wxColour& colour) const
{
    wxMemoryDC dcMem;
    wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    wxBitmap bitmap(16, 16, 32);

    dcMem.SelectObject(bitmap);
    // Draw white background
    dcMem.SetPen(wxPen(col));
    dcMem.SetBrush(wxBrush(col));
    dcMem.DrawRectangle(wxPoint(0, 0), wxSize(16, 16));
    // Draw the base icon

    dcMem.DrawBitmap(bmp.ConvertToDisabled(), wxPoint(0, 0), true);

    wxColour c2 = DrawingUtils::LightColour(colour, 3.0);
    dcMem.GradientFillLinear(wxRect(wxPoint(0, 1), wxSize(2, 14)), colour, c2, wxSOUTH);

    //// Draw a small 2 pixel vertical line
    // wxBitmap coverBmp(16, 16, 32);
    //{
    //    wxAlphaPixelData pixData(coverBmp);
    //
    //    // Set the fill pixels
    //    int red   = colour.Red();
    //    int green = colour.Green();
    //    int blue  = colour.Blue();
    //    wxAlphaPixelData::Iterator p(pixData);
    //    for (int y=0; y<16; y++) {
    //        p.MoveTo(pixData, 0, y);
    //        for (int x=0; x<16; x++) {
    //            p.Red()   = red;
    //            p.Green() = green;
    //            p.Blue()  = blue;
    //            p.Alpha() = 90;
    //            ++p;
    //        }
    //    }
    //}
    //
    // dcMem.DrawBitmap(coverBmp, wxPoint(0, 0));
    dcMem.SelectObject(wxNullBitmap);

    return bitmap;
}

OverlayTool& OverlayTool::Get()
{
    static OverlayTool theTool;
    return theTool;
}

wxBitmap OverlayTool::CreateBitmap(const wxBitmap& orig, OverlayTool::BmpType type) const
{
    switch(type) {
    case Bmp_Conflict:
        return DoAddBitmap(orig, "RED");
    case Bmp_OK:
        return DoAddBitmap(orig, "MEDIUM FOREST GREEN");
    case Bmp_Modified:
        return DoAddBitmap(orig, "ORANGE RED");
    default:
        return orig;
    }
}
