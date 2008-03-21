///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __advanced_settings__
#define __advanced_settings__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include "wx/treebook.h"
#include <wx/panel.h>
#include "build_page.h"

///////////////////////////////////////////////////////////////////////////
class BuildTabSetting;

///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedDlg
///////////////////////////////////////////////////////////////////////////////
class AdvancedDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE();
	
protected:
	wxTreebook* m_notebook; 
	wxPanel* m_compilersPage;
	wxStaticText* m_staticText1;
	wxButton* m_buttonNewCompiler;
	wxStaticLine* m_staticline2;
	wxNotebook* m_compilersNotebook; 
	wxStaticLine* m_staticline10;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	BuildPage *m_buildPage;
	BuildTabSetting *m_buildSettings;
	wxMenu *m_rightclickMenu;
	
	void OnButtonNewClicked(wxCommandEvent &);
	void OnButtonOKClicked(wxCommandEvent &);
	void OnDeleteCompiler(wxCommandEvent &);
	void OnMouseRightUp(wxMouseEvent &e);
	void LoadCompilers();
	
public:
	AdvancedDlg( wxWindow* parent, size_t selected_page, int id = wxID_ANY, wxString title = wxT("Build Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(800, 600), int style = wxDEFAULT_DIALOG_STYLE );
	~AdvancedDlg();
};

#endif //__advanced_settings__
