//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : new_item_base_dlg.h
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __new_item_base_dlg__
#define __new_item_base_dlg__

#include <wx/intl.h>

#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewItemBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewItemBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_fileType;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_fileName;
		
		wxStaticText* m_staticText3;
		wxTextCtrl* m_location;
		wxButton* m_browseBtn;
		wxStaticLine* m_staticline1;
		wxButton* m_okButton;
		wxButton* m_cancel;
	
	public:
		
		NewItemBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 512,462 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~NewItemBaseDlg();
	
};

#endif //__new_item_base_dlg__
