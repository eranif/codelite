///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __quickfindbarbase__
#define __quickfindbarbase__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class QuickFindBarBase
///////////////////////////////////////////////////////////////////////////////
class QuickFindBarBase : public wxPanel 
{
	private:
	
	protected:
		wxButton* m_buttonClose;
		wxTextCtrl* m_textCtrlFindWhat;
		wxButton* m_buttonNext;
		wxButton* m_buttonPrevious;
		wxCheckBox* m_checkBoxMatchCase;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonClose( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnText( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonNext( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonPrevious( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		QuickFindBarBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~QuickFindBarBase();
	
};

#endif //__quickfindbarbase__
