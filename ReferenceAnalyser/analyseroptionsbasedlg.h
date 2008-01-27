///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __analyseroptionsbasedlg__
#define __analyseroptionsbasedlg__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AnalyserOptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class AnalyserOptionsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
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
		AnalyserOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Reference Analyser Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 505,212 ), long style = wxDEFAULT_DIALOG_STYLE );
		~AnalyserOptionsBaseDlg();
	
};

#endif //__analyseroptionsbasedlg__
