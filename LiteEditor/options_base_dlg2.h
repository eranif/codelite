///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __options_base_dlg2__
#define __options_base_dlg2__

#include <wx/treebook.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OptionsBaseDlg2
///////////////////////////////////////////////////////////////////////////////
class OptionsBaseDlg2 : public wxDialog 
{
	private:
	
	protected:
		wxTreebook * m_treeBook;
		wxStaticLine* m_staticline1;
		wxButton* m_okButton;
		wxButton* m_cancelButton;
		wxButton* m_applyButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnActivate( wxActivateEvent& event ){ event.Skip(); }
		virtual void OnInitDialog( wxInitDialogEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonApply( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		OptionsBaseDlg2( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Editor Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
		~OptionsBaseDlg2();
	
};

#endif //__options_base_dlg2__
