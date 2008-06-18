//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : unittestsbasereport.h              
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
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __unittestsbasereport__
#define __unittestsbasereport__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "progressctrl.h"
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UnitTestsBasePage
///////////////////////////////////////////////////////////////////////////////
class UnitTestsBasePage : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText7;
		ProgressCtrl *m_progressPassed;
		wxStaticText* m_staticText8;
		ProgressCtrl *m_progressFailed;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticText10;
		wxStaticText* m_staticTextTotalTests;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticTextFailTestsNum;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticTextSuccessTestsNum;
		wxStaticLine* m_staticline4;
		wxStaticText* m_staticText16;
		wxListCtrl* m_listCtrlErrors;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		
	
	public:
		UnitTestsBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 969,472 ), long style = wxTAB_TRAVERSAL );
		~UnitTestsBasePage();
	
};

#endif //__unittestsbasereport__
