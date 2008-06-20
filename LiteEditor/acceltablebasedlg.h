///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __acceltablebasedlg__
#define __acceltablebasedlg__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AccelTableBaseDlg
///////////////////////////////////////////////////////////////////////////////
class AccelTableBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_listCtrl1;
		wxStaticLine* m_staticline1;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_buttonDefault;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnColClicked( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeselected( wxListEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonDefaults( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		AccelTableBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Accelerator table:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 819,530 ), long style = wxDEFAULT_DIALOG_STYLE );
		~AccelTableBaseDlg();
	
};

#endif //__acceltablebasedlg__
