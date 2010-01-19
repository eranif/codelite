///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __quickfindbarbase__
#define __quickfindbarbase__

#include <wx/intl.h>

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class QuickFindBarBase
///////////////////////////////////////////////////////////////////////////////
class QuickFindBarBase : public wxPanel 
{
	private:
	
	protected:
		wxBitmapButton* m_closeButton;
		wxStaticText* m_staticTextFind;
		wxTextCtrl* m_findWhat;
		wxButton* m_buttonFindNext;
		wxButton* m_buttonFindPrevious;
		
		wxStaticText* m_replaceStaticText;
		wxTextCtrl* m_replaceWith;
		wxButton* m_replaceButton;
		
		wxStaticLine* m_staticline1;
		wxCheckBox* m_checkBoxCase;
		wxCheckBox* m_checkBoxWord;
		wxCheckBox* m_checkBoxRegex;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHide( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnReplace( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBoxCase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBoxWord( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBoxRegex( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxBoxSizer* optionsSizer;
		
		QuickFindBarBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~QuickFindBarBase();
	
};

#endif //__quickfindbarbase__
