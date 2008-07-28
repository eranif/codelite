///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __memoryviewbase__
#define __memoryviewbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MemoryViewBase
///////////////////////////////////////////////////////////////////////////////
class MemoryViewBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlExpression;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlSize;
		wxButton* m_buttonEvaluate;
		wxTextCtrl* m_textCtrlMemory;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextEntered( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEvaluate( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEvaluateUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		MemoryViewBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 596,300 ), long style = wxTAB_TRAVERSAL );
		~MemoryViewBase();
	
};

#endif //__memoryviewbase__
