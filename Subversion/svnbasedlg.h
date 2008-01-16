///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __svnbasedlg__
#define __svnbasedlg__

#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SvnBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SvnBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
	
	public:
		SvnBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Enter SVN commit comment:"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 497,369 ), int style = wxDEFAULT_DIALOG_STYLE);
	
};

#endif //__svnbasedlg__
