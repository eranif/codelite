//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : lexer_page.h              
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
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lexer_page__
#define __lexer_page__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/button.h>
#include "lexer_configuration.h"
#include <wx/clrpicker.h>
#include <wx/fontpicker.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LexerPage
///////////////////////////////////////////////////////////////////////////////
class LexerPage : public wxPanel 
{
	LexerConfPtr m_lexer;
	std::list<StyleProperty> m_propertyList;
	int m_selection;

	DECLARE_EVENT_TABLE()

protected:
	wxListBox* m_properties;
	wxFontPickerCtrl* m_fontPicker;
	wxFontPickerCtrl* m_globalFontPicker;
	wxColourPickerCtrl* m_globalBgColourPicker;
	wxColourPickerCtrl* m_colourPicker;
	wxColourPickerCtrl* m_bgColourPicker;
	wxTextCtrl *m_fileSpec;

	// Virtual event handlers, overide them in your derived class
	virtual void OnItemSelected( wxCommandEvent& event );
	virtual void OnFontChanged(wxFontPickerEvent &event);
	virtual void OnColourChanged(wxColourPickerEvent &event);
	virtual void OnEditKeyWordsButton0(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton1(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton2(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton3(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton4(wxCommandEvent &event);
	
	void EditKeyWords(int set);
	
public:
	LexerPage( wxWindow* parent, LexerConfPtr lexer, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 285,300 ), int style = wxTAB_TRAVERSAL );
	void SaveSettings();
};

#endif //__lexer_page__
