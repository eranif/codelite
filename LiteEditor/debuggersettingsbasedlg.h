///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __debuggersettingsbasedlg__
#define __debuggersettingsbasedlg__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerSettingsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class DebuggerSettingsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxFlatNotebook* m_notebook2;
		wxPanel* m_panel2;
		wxListCtrl* m_listCtrl1;
		wxButton* m_buttonNewType;
		wxButton* m_buttonEdit;
		wxButton* m_buttonDelete;
		wxPanel* m_panel1;
		wxFlatNotebook* m_book;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeselected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnNewShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DebuggerSettingsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Debugger Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 537,451 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__debuggersettingsbasedlg__
