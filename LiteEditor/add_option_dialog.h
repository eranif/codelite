///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __add_option_dialog__
#define __add_option_dialog__

#include <wx/wx.h>

#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MyDialog2
///////////////////////////////////////////////////////////////////////////////
class AddOptionDlg : public wxDialog 
{
private:

protected:
	wxTextCtrl* m_text;
	wxStaticLine* m_staticline9;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;

public:
	AddOptionDlg( wxWindow* parent, wxString value = wxEmptyString, int id = wxID_ANY, wxString title = wxT("Edit"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 481,299 ), int style = wxDEFAULT_DIALOG_STYLE );
	wxString GetValue() const;
};

#endif //__add_option_dialog__
