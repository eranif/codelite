//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : NArrow.h
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

#ifndef NARROW_H
#define NARROW_H

#include <wx/wx.h>
#include <wx/wxsf/SolidArrow.h> // Base class: wxSFArrowBase

/*! \brief Class representing arrow and used in ERD diagram. Connection type - N */
class NArrow : public wxSFSolidArrow {

public:
	XS_DECLARE_CLONABLE_CLASS(NArrow);
	
	
	NArrow(void);
	NArrow(wxSFShapeBase* parent);
	NArrow(const NArrow& obj);
	virtual ~NArrow();

public:
	virtual void Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc);
};

#endif // NARROW_H
