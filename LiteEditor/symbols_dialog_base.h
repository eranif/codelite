///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __symbols_dialog_base__
#define __symbols_dialog_base__

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
/// Class SymbolsDialogBase
///////////////////////////////////////////////////////////////////////////////
class SymbolsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_results;
		wxStaticLine* statLine;
		wxButton* m_okButton;
		wxButton* m_cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SymbolsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Resolve Ambiguity"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 851,428 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SymbolsDialogBase();
	
};

#endif //__symbols_dialog_base__
