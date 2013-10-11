//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : ccboxmainpanel.h
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

#ifndef CCBOXMAINPANEL_H
#define CCBOXMAINPANEL_H

#include "precompiled_header.h"
#include <wx/panel.h>
#include <wx/window.h>

class CCBoxMainPanel : public wxPanel
{
public:
	CCBoxMainPanel(wxWindow *win);
	virtual ~CCBoxMainPanel();

	DECLARE_EVENT_TABLE();
	void OnPaint(wxPaintEvent &e);
	void OnErasebg(wxEraseEvent &e);
};

#endif // CCBOXMAINPANEL_H
