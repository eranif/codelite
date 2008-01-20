///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __buildsettingstabbase__
#define __buildsettingstabbase__

#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
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
	
	public:
		BuildTabSettingsBase( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 549,300 ), int style = wxTAB_TRAVERSAL );
	
};

#endif //__buildsettingstabbase__
