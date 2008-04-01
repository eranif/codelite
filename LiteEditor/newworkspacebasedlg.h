///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newworkspacebasedlg__
#define __newworkspacebasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewWorkspaceBase
///////////////////////////////////////////////////////////////////////////////
class NewWorkspaceBase : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panelWorkspace;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlWorkspaceName;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlWorkspacePath;
		wxButton* m_buttonWorkspaceDirPicker;
		wxStaticText* m_staticTextWorkspaceFileName;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonCreateWorkspace;
		wxButton* m_buttonCancelWorkspace;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnWorkspacePathUpdated( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnWorkspaceDirPicker( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCreate( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		NewWorkspaceBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("New Workspace"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 623,350 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NewWorkspaceBase();
	
};

#endif //__newworkspacebasedlg__
