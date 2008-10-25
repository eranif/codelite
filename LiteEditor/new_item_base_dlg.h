///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __new_item_base_dlg__
#define __new_item_base_dlg__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewItemBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewItemBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_fileType;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_location;
		wxButton* m_browseBtn;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_fileName;
		wxStaticLine* m_staticline1;
		wxButton* m_okButton;
		wxButton* m_cancel;
	
	public:
		NewItemBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 512,462 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~NewItemBaseDlg();
	
};

#endif //__new_item_base_dlg__
