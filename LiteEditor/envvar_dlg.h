///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __envvar_dlg__
#define __envvar_dlg__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarDlg
///////////////////////////////////////////////////////////////////////////////
class EnvVarDlg : public wxDialog 
{
private:

protected:
	wxPanel* m_panel1;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textValue;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_textName;
	wxStaticLine* m_staticline3;
	wxButton* m_buttonOK;
	wxButton* m_buttonCacnel;
public:
	EnvVarDlg( wxWindow* parent, wxString name = wxEmptyString, wxString value = wxEmptyString, int id = wxID_ANY, wxString title = wxT("Environment Variable"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 320,152 ), int style = wxDEFAULT_DIALOG_STYLE);

	wxString GetName() const { return m_textName->GetValue(); }
	wxString GetValue() const { return m_textValue->GetValue(); }
};

#endif //__envvar_dlg__
