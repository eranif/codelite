///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tagsparsersearchpathsbasedlg__
#define __tagsparsersearchpathsbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checklst.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TagsParserSearchPathsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class TagsParserSearchPathsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxCheckListBox* m_checkListPaths;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
	
	public:
		TagsParserSearchPathsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add Search Paths to Code Completion Parser"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TagsParserSearchPathsBaseDlg();
	
};

#endif //__tagsparsersearchpathsbasedlg__
