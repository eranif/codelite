///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  8 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __yestoallbasedlg__
#define __yestoallbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class YesToAllBaseDlg
///////////////////////////////////////////////////////////////////////////////
class YesToAllBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
		wxStaticText* m_staticMsg;
		wxCheckBox* m_checkBoxAll;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1Yes;
		wxButton* m_sdbSizer1No;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNoClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnYesClicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		YesToAllBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Are you sure?"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~YesToAllBaseDlg();
	
};

#endif //__yestoallbasedlg__
