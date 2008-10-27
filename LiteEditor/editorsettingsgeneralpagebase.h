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
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
#include <wx/radiobut.h>
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
		wxCheckBox* m_indentsUsesTabs;
		wxCheckBox* m_showIndentationGuideLines;
		wxCheckBox* m_highlighyCaretLine;
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_caretLineColourPicker;
		wxStaticText* m_staticText3;
		wxSpinCtrl* m_spinCtrlTabWidth;
		wxStaticText* m_staticText4;
		wxChoice* m_whitespaceStyle;
		wxRadioBox* m_radioBoxNavigationMethod;
		wxCheckBox* m_checkBoxTrimLine;
		wxCheckBox* m_checkBoxAppendLF;
		wxRadioButton* m_radioBtnRMDisabled;
		wxRadioButton* m_radioBtnRMLine;
		wxRadioButton* m_radioBtnRMBackground;
		wxStaticText* m_staticText41;
		wxSpinCtrl* m_rightMarginColumn;
		wxStaticText* m_staticText5;
		wxColourPickerCtrl* m_rightMarginColour;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRightMarginIndicator( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		EditorSettingsGeneralPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 650,552 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsGeneralPageBase();
	
};

#endif //__editorsettingsgeneralpagebase__
