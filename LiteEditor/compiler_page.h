//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : compiler_page.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __compiler_page__
#define __compiler_page__

#include <wx/wx.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CompilerPage
///////////////////////////////////////////////////////////////////////////////
class CompilerPage : public wxScrolledWindow 
{
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText161;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textErrorPattern;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textErrorFileIndex;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textErrorLineNumber;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText17;
		wxStaticText* m_staticText51;
		wxTextCtrl* m_textWarnPattern;
		wxStaticText* m_staticText61;
		wxTextCtrl* m_textWarnFileIndex;
		wxStaticText* m_staticText71;
		wxTextCtrl* m_textWarnLineNumber;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText18;
		wxStaticText* m_staticText141;
		wxTextCtrl* m_textCtrlGlobalIncludePath;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrlGlobalLibPath;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText19;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textObjectExtension;
		wxPanel* m_panel2;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCompilerName;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textLinkerName;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textSOLinker;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textArchiveTool;
		wxStaticText* m_staticText14;
		wxTextCtrl* m_textResourceCmp;
		wxPanel* m_panel3;
		wxStaticText* m_staticText8;
		wxListCtrl* m_listSwitches;

	wxString m_selSwitchName ;
	wxString m_selSwitchValue;
	wxString m_cmpname;

	// Virtual event handlers, overide them in your derived class
	virtual void OnItemActivated( wxListEvent& event );
	virtual void OnItemSelected( wxListEvent& event );

	void EditSwitch();
	void InitSwitches();
	void CustomInitialize();
	void ConnectEvents();	
	void AddSwitch(const wxString &name, const wxString &value, bool choose);
	

public:
	CompilerPage( wxWindow* parent, wxString name, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 1, 1), int style = wxTAB_TRAVERSAL );
	void Save();

};

#endif //__compiler_page__
