//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : externaltoolsbasedlg.h
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

#ifndef __externaltoolsbasedlg__
#define __externaltoolsbasedlg__

#include <wx/intl.h>

#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ExternalToolBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ExternalToolBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_listCtrlTools;
		wxButton* m_buttonNewTool;
		wxButton* m_buttonEdit;
		wxButton* m_buttonDelete;
		wxStaticLine* m_staticline3;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnButtonNew( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonNewUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonEdit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonEditUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonDelete( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonDeleteUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		
		ExternalToolBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("External Tools"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~ExternalToolBaseDlg();
	
};

#endif //__externaltoolsbasedlg__
