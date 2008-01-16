///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __symbols_dialog_base__
#define __symbols_dialog_base__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SymbolsDialogBase
///////////////////////////////////////////////////////////////////////////////
class SymbolsDialogBase : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnItemActivated( wxListEvent& event ){ OnItemActivated( event ); }
		void _wxFB_OnItemSelected( wxListEvent& event ){ OnItemSelected( event ); }
		void _wxFB_OnButtonOK( wxCommandEvent& event ){ OnButtonOK( event ); }
		void _wxFB_OnButtonCancel( wxCommandEvent& event ){ OnButtonCancel( event ); }
		
	
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
		SymbolsDialogBase( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Resolve Ambiguity"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 600,300 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__symbols_dialog_base__
