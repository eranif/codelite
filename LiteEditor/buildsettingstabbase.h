///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __buildsettingstabbase__
#define __buildsettingstabbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BuildTabSettingsBase
///////////////////////////////////////////////////////////////////////////////
class BuildTabSettingsBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText1;
		wxColourPickerCtrl* m_colourPickerError;
		wxColourPickerCtrl* m_colourPickerErrorFg;
		wxCheckBox* m_checkBoxBoldErrFont;
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_colourPickerWarnings;
		wxColourPickerCtrl* m_colourPickerWarningsFg;
		wxCheckBox* m_checkBoxBoldWarnFont;
		wxCheckBox* m_checkBoxSkipeWarnings;
		wxCheckBox* m_checkBoxAutoHide;
		wxCheckBox* m_checkBoxAutoShow;
		wxCheckBox* m_checkBoxErrorsFirstLine;
		wxRadioBox* m_radioBoxShowBuildTab;
		wxCheckBox* m_checkBoxDisplayAnnotations;
		wxCheckBox* m_checkBoxDisplayMarkers;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		BuildTabSettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 720,550 ), long style = wxTAB_TRAVERSAL );
		~BuildTabSettingsBase();
	
};

#endif //__buildsettingstabbase__
