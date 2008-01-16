///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __edit_workspace_conf_dlg__
#define __edit_workspace_conf_dlg__

#include <wx/wx.h>

#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditWorkspaceConfDlg
///////////////////////////////////////////////////////////////////////////////
class EditWorkspaceConfDlg : public wxDialog 
{
private:
	void CustomInit();
	void FillList();
	void DoRename(const wxString &selItem);

protected:
	wxListBox* m_wspConfList;
	wxButton* m_buttonRename;
	wxButton* m_buttonDelete;
	wxButton* m_buttonClose;

protected:
	//event handlers
	void OnDelete(wxCommandEvent &event);
	void OnRename(wxCommandEvent &event);
	void OnListBoxDClick(wxCommandEvent &event);

public:
	EditWorkspaceConfDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Edit Workspace Configuration"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 362,261 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__edit_workspace_conf_dlg__
