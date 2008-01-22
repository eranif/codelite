///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cscopetabbase__
#define __cscopetabbase__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CscopeTabBase
///////////////////////////////////////////////////////////////////////////////
class CscopeTabBase : public wxPanel 
{
	private:
	
	protected:
		wxTreeCtrl* m_treeCtrlResults;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLeftDClick( wxMouseEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		CscopeTabBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	
};

#endif //__cscopetabbase__
