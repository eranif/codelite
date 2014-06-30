//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : copyrights_proj_sel_base_dlg.h
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

#ifndef __copyrights_proj_sel_base_dlg__
#define __copyrights_proj_sel_base_dlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checklst.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CopyrightsProjectSelBaseDlg
///////////////////////////////////////////////////////////////////////////////
class CopyrightsProjectSelBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxCheckListBox* m_checkListProjects;
		wxButton* m_buttonCheckAll;
		wxButton* m_buttonUnCheckAll;
		wxButton* m_buttonInsert;
		wxButton* m_button5;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnUnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		CopyrightsProjectSelBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Batch Insert Of Copyrights"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 704,418 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~CopyrightsProjectSelBaseDlg();
	
};

#endif //__copyrights_proj_sel_base_dlg__
