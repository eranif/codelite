//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : simpletablebase.h              
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
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __simpletablebase__
#define __simpletablebase__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_TOOLNEW 2500
#define ID_TOOLDELETE 2501
#define ID_TOOLDELETEALL 2502

///////////////////////////////////////////////////////////////////////////////
/// Class SimpleTableBase
///////////////////////////////////////////////////////////////////////////////
class SimpleTableBase : public wxPanel 
{
	private:
	
	protected:
		wxToolBar* m_toolBar2;
		wxListCtrl* m_listTable;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNewWatch( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNewWatchUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnDeleteWatch( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteWatchUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnDeleteAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteAllUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnListEditLabelBegin( wxListEvent& event ){ event.Skip(); }
		virtual void OnListEditLabelEnd( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemRightClick( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnListKeyDown( wxListEvent& event ){ event.Skip(); }
		
	
	public:
		SimpleTableBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	
};

#endif //__simpletablebase__
