//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_overlay_tool.cpp
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

#include <wx/dcmemory.h>
#include "svn_overlay_tool.h"
#include <wx/settings.h>
#include "bitmap_loader.h"
#include "globals.h"
#include "imanager.h"

wxBitmap SvnOverlayTool::ms_bmpOK;
wxBitmap SvnOverlayTool::ms_bmpConflict;
wxBitmap SvnOverlayTool::ms_bmpModified;
wxBitmap SvnOverlayTool::ms_bmpLocked;
wxBitmap SvnOverlayTool::ms_bmpNew;
wxBitmap SvnOverlayTool::ms_bmpUnversioned;
wxBitmap SvnOverlayTool::ms_bmpDeleted;

SvnOverlayTool::SvnOverlayTool()
{
    BitmapLoader& bmpLoader = *clGetManager()->GetStdIcons();
    ms_bmpOK = bmpLoader.LoadBitmap(wxT("overlay/16/ok"));
    ms_bmpModified = bmpLoader.LoadBitmap(wxT("overlay/16/modified"));
    ms_bmpConflict = bmpLoader.LoadBitmap(wxT("overlay/16/conflicted"));
    ms_bmpLocked = bmpLoader.LoadBitmap(wxT("overlay/16/locked"));
    ms_bmpNew = bmpLoader.LoadBitmap(wxT("overlay/16/new"));
    ms_bmpUnversioned = bmpLoader.LoadBitmap(wxT("overlay/16/unversioned"));
    ms_bmpDeleted = bmpLoader.LoadBitmap(wxT("overlay/16/deleted"));
}

SvnOverlayTool::~SvnOverlayTool() {}

SvnOverlayTool& SvnOverlayTool::Get()
{
    static SvnOverlayTool theTool;
    return theTool;
}

wxBitmap SvnOverlayTool::DoAddBitmap(const wxBitmap& bmp, const wxBitmap& overlayBmp)
{
    wxMemoryDC dcMem;

    wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    wxBitmap bitmap(16, 16);
    dcMem.SelectObject(bitmap);
    dcMem.SetPen(wxPen(col));
    dcMem.SetBrush(wxBrush(col));
    dcMem.DrawRectangle(wxPoint(0, 0), wxSize(16, 16));
    dcMem.DrawBitmap(bmp, wxPoint(0, 0));

    dcMem.DrawBitmap(overlayBmp, wxPoint(0, 0));
    dcMem.SelectObject(wxNullBitmap);

    return bitmap;
}

wxBitmap SvnOverlayTool::ConflictIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpConflict); }

wxBitmap SvnOverlayTool::LockedIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpLocked); }

wxBitmap SvnOverlayTool::ModifiedIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpModified); }

wxBitmap SvnOverlayTool::NewIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpNew); }

wxBitmap SvnOverlayTool::OKIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpOK); }

wxBitmap SvnOverlayTool::UnversionedIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpUnversioned); }

wxBitmap SvnOverlayTool::DeletedIcon(const wxBitmap& bmp) { return DoAddBitmap(bmp, ms_bmpDeleted); }
