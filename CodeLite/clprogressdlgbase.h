//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clprogressdlgbase.h
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __clprogressdlgbase__
#define __clprogressdlgbase__

#if wxUSE_GUI
#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class clProgressDlgBase
///////////////////////////////////////////////////////////////////////////////
class clProgressDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticLabel;
		wxGauge* m_gauge;
	
	public:
		
		clProgressDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("CodeLite"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,-1 ), long style = wxSTAY_ON_TOP|wxSTATIC_BORDER ); 
		~clProgressDlgBase();
	
};
#endif // #if wxUSE_GUI
#endif //__clprogressdlgbase__
