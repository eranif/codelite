///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __svnlogbasedlg__
#define __svnlogbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include "filepicker.h"
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SvnLogBaseDialog
///////////////////////////////////////////////////////////////////////////////
class SvnLogBaseDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlFromRevision;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlToRevision;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText5;
		FilePicker *m_filePicker;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
	
	public:
		SvnLogBaseDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Subversion Change Log:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SvnLogBaseDialog();
	
};

#endif //__svnlogbasedlg__
