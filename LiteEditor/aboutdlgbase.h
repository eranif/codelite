//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : aboutdlgbase.h              
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

#ifndef __aboutdlgbase__
#define __aboutdlgbase__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/html/htmlwin.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AboutDlgBase
///////////////////////////////////////////////////////////////////////////////
class AboutDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxStaticBitmap* m_bitmap;
		wxPanel* m_panel4;
		wxHtmlWindow* m_htmlWin3;
		wxStaticText* m_staticTextHomePage;
		wxHyperlinkCtrl* m_hyperlink1;
		wxStaticText* m_staticText2;
		wxHyperlinkCtrl* m_hyperlink2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticTextInformation;
		wxButton* m_buttonOk;
	
	public:
		AboutDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About CodeLite"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~AboutDlgBase();
	
};

#endif //__aboutdlgbase__
