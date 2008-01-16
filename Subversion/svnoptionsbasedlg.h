///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __svnoptionsbasedlg__
#define __svnoptionsbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "filepicker.h"
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SvnOptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SvnOptionsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
		wxStaticText* m_staticText1;
		FilePicker *m_filePicker;
		wxStaticLine* m_staticline;
		wxStaticText* m_staticText3;
		wxSpinCtrl* m_spinCtrl1;
		wxCheckBox* m_checkBoxUpdateOnSave;
		wxCheckBox* m_checkBoxAutoAddNewFiles;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdateOnSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAutoAddNewFiles( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SvnOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Subversion Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 505,212 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__svnoptionsbasedlg__
