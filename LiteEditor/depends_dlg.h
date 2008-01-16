///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __depends_dlg__
#define __depends_dlg__

#include <wx/wx.h>

#include <wx/choicebk.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DependenciesDlg
///////////////////////////////////////////////////////////////////////////////
class DependenciesDlg : public wxDialog 
{
private:

protected:
	wxChoicebook* m_book;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	wxString m_projectName;

	void Init();
	virtual void OnButtonOK(wxCommandEvent &event);
	virtual void OnButtonCancel(wxCommandEvent &event);

public:
	DependenciesDlg( wxWindow* parent, const wxString &projectName, int id = wxID_ANY, wxString title = wxT("Dependencies"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 532,361 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__depends_dlg__
