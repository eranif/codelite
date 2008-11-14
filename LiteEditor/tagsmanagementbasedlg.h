//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tagsmanagementbasedlg.h              
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
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tagsmanagementbasedlg__
#define __tagsmanagementbasedlg__

#include <wx/intl.h>

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TagsManagementBaseDlg
///////////////////////////////////////////////////////////////////////////////
class TagsManagementBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_listCtrlDatabases;
		wxButton* m_buttonAdd;
		wxButton* m_buttonRemove;
		wxButton* m_buttonSetActive;
		wxButton* m_buttonFixPaths;
		wxStaticText* m_staticTextFullPath;
		wxStaticText* m_staticText1;
		wxListCtrl* m_listCtrlPaths;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTagsDbActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnTagsDbDeSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnTagsDbSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnAddDatabase( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveDatabase( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveDatabaseUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnSetActive( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFixPaths( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		TagsManagementBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Manage Tags Databases"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TagsManagementBaseDlg();
	
};

#endif //__tagsmanagementbasedlg__
