//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_overlay_tool.h
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

#ifndef SVN_OVERLAY_TOOL_H
#define SVN_OVERLAY_TOOL_H

#include "precompiled_header.h"

class SvnOverlayTool {

	static wxBitmap ms_bmpOK;
	static wxBitmap ms_bmpConflict;
	static wxBitmap ms_bmpModified;
	static wxBitmap ms_bmpLocked;
	static wxBitmap ms_bmpUnversioned;
	static wxBitmap ms_bmpNew;
	static wxBitmap ms_bmpDeleted;
	
private:
	SvnOverlayTool();
    virtual ~SvnOverlayTool() = default;
	
protected:
	wxBitmap DoAddBitmap(const wxBitmap& bmp, const wxBitmap &overlayBmp);
	
public:
	static SvnOverlayTool& Get();

	wxBitmap OKIcon         (const wxBitmap& bmp);
	wxBitmap ConflictIcon   (const wxBitmap& bmp);
	wxBitmap ModifiedIcon   (const wxBitmap& bmp);
	wxBitmap NewIcon        (const wxBitmap& bmp);
	wxBitmap LockedIcon     (const wxBitmap& bmp);
	wxBitmap UnversionedIcon(const wxBitmap& bmp);
	wxBitmap DeletedIcon    (const wxBitmap& bmp);
	
};

#endif // SVN_OVERLAY_TOOL_H
