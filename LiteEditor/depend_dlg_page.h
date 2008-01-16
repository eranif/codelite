///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __depend_dlg_page__
#define __depend_dlg_page__

#include <wx/wx.h>

#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DependenciesPage
///////////////////////////////////////////////////////////////////////////////
class DependenciesPage : public wxPanel 
{
private:

protected:
	wxStaticText* m_staticText1;
	wxCheckListBox* m_checkListProjectList;
	wxStaticText* m_staticText2;
	wxListBox* m_listBoxBuildOrder;
	wxString m_projectName;

	void Init();
	void OnUpCommand(wxListBox *list);
	void OnDownCommand(wxListBox *list);

	virtual void OnCheckListItemToggled(wxCommandEvent &event);
	virtual void OnMoveUp(wxCommandEvent &event);
	virtual void OnMoveDown(wxCommandEvent &event);

public:
	DependenciesPage( wxWindow* parent, const wxString &projName, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	void Save();
	
};

#endif //__depend_dlg_page__
