///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ACCELTABLEBASEDLG_H__
#define __ACCELTABLEBASEDLG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AccelTableBaseDlg
///////////////////////////////////////////////////////////////////////////////
class AccelTableBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlFilter;
		wxListCtrl* m_listCtrl1;
		wxButton* m_buttonEdit;
		wxButton* m_buttonDefault;
		wxStaticLine* m_staticline1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnColClicked( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnEditButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonDefaults( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		AccelTableBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Keyboard Shortcuts:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 819,530 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~AccelTableBaseDlg();
	
};

#endif //__ACCELTABLEBASEDLG_H__
