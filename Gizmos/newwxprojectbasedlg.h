///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newwxprojectbasedlg__
#define __newwxprojectbasedlg__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include "dirpicker.h"
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewWxProjectBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewWxProjectBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_bitmapPanel;
		wxStaticBitmap* m_bitmap1;
		wxStaticLine* m_staticline4;
		wxFlatNotebook* m_flatNotebook1; 
		wxPanel* m_panel2;
		wxPanel* m_panelBasicInfo;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlName;
		wxStaticText* m_staticText2;
		DirPicker *m_dirPicker;
		wxStaticText* m_staticText3;
		wxChoice* m_choiceApplicationType;
		wxStaticText* m_staticText4;
		wxStaticLine* m_staticline2;
		wxCheckBox* m_checkBoxUnicode;
		wxCheckBox* m_checkBoxMWindows;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChoiceChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		NewWxProjectBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("New wxWidgets Project"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 528,390 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__newwxprojectbasedlg__
