//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingsgeneralpagebase.h              
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

#ifndef __editorsettingsgeneralpagebase__
#define __editorsettingsgeneralpagebase__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsGeneralPageBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsGeneralPageBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_displayLineNumbers;
		wxCheckBox* m_checkBoxMatchBraces;
		wxCheckBox* m_showIndentationGuideLines;
		wxCheckBox* m_highlighyCaretLine;
		wxStaticText* m_staticText9;
		wxColourPickerCtrl* m_caretLineColourPicker;
		wxStaticText* m_staticText4;
		wxChoice* m_whitespaceStyle;
		
		wxCheckBox* m_indentsUsesTabs;
		wxStaticText* m_staticText3;
		wxSpinCtrl* m_indentWidth;
		wxStaticText* m_staticText31;
		wxSpinCtrl* m_tabWidth;
		wxRadioButton* m_radioBtnRMDisabled;
		wxRadioButton* m_radioBtnRMLine;
		wxRadioButton* m_radioBtnRMBackground;
		wxStaticText* m_staticText41;
		wxSpinCtrl* m_rightMarginColumn;
		wxStaticText* m_staticText5;
		wxColourPickerCtrl* m_rightMarginColour;
		wxCheckBox* m_checkBoxTrimLine;
		wxCheckBox* m_checkBoxAppendLF;
		wxRadioBox* m_radioBoxNavigationMethod;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHighlightCaretLine( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRightMarginIndicator( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		EditorSettingsGeneralPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsGeneralPageBase();
	
};

#endif //__editorsettingsgeneralpagebase__
