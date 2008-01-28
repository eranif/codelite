///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __analyseroptionsbasedlg__
#define __analyseroptionsbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
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
		wxStaticText* m_staticText1;
		wxTextCtrl* m_containerName;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_containerOperator;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		AnalyserOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Reference Analyser Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 425,175 ), long style = wxDEFAULT_DIALOG_STYLE );
		~AnalyserOptionsBaseDlg();
	
};

#endif //__analyseroptionsbasedlg__
