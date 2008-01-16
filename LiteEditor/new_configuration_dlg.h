///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __new_configuration_dlg__
#define __new_configuration_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewConfigurationDlg
///////////////////////////////////////////////////////////////////////////////
class NewConfigurationDlg : public wxDialog 
{
private:
	wxString m_projectName;
	wxPanel* m_panel1;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textConfigurationName;
	wxStaticText* m_staticText2;
	wxChoice* m_choiceCopyConfigurations;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;

protected:
	void OnButtonOK(wxCommandEvent &event);

public:
	NewConfigurationDlg( wxWindow* parent, const wxString &projName, int id = wxID_ANY, wxString title = wxT("New Configuration"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 352,199 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__new_configuration_dlg__
