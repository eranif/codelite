///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cscopetabbase__
#define __cscopetabbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/treectrl.h>
#include <wx/gauge.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CscopeTabBase
///////////////////////////////////////////////////////////////////////////////
class CscopeTabBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_statusMessage;
		wxButton* m_buttonClear;
		wxStaticLine* m_staticline2;
		wxTreeCtrl* m_treeCtrlResults;
		wxStaticLine* m_staticline1;
		wxGauge* m_gauge;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClearResults( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnLeftDClick( wxMouseEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		CscopeTabBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	
};

#endif //__cscopetabbase__
