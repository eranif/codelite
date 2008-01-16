///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __close_all_dlg__
#define __close_all_dlg__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

enum {
	CLOSEALL_SAVEALL,
	CLOSEALL_DISCARDALL,
	CLOSEALL_ASKFOREACHFILE
};

///////////////////////////////////////////////////////////////////////////////
/// Class CloseAllDialog
///////////////////////////////////////////////////////////////////////////////
class CloseAllDialog : public wxDialog 
{
private:

protected:
	wxPanel* m_mainPanel;
	wxStaticText* m_staticMsg;
	wxButton* m_buttonSave;
	wxButton* m_buttonSaveAllFiles;
	wxButton* m_buttonDiscardChangesForAllFiles;

	void OnDiscardAllClicked(wxCommandEvent &event);
	void OnSaveAll(wxCommandEvent &event);
	void OnAskForEachFile(wxCommandEvent &event);

public:
	CloseAllDialog( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Close All"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 424,127 ), int style = wxDEFAULT_DIALOG_STYLE );
};

#endif //__close_all_dlg__
