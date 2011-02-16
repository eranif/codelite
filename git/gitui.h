///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __gitui__
#define __gitui__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GitSettingsDlgBase
///////////////////////////////////////////////////////////////////////////////
class GitSettingsDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxColourPickerCtrl* m_colourTrackedFile;
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_colourDiffFile;
		wxStaticText* m_staticText4;
		wxFilePickerCtrl* m_pathGIT;
		wxStaticText* m_staticText5;
		wxFilePickerCtrl* m_pathGITK;
		
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
	
	public:
		
		GitSettingsDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Git settings..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~GitSettingsDlgBase();
	
};

#endif //__gitui__
