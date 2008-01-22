///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cscopetabbase__
#define __cscopetabbase__

#include <wx/treectrl.h>
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
		virtual void OnItemActivated( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		CscopeTabBase( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	
};

#endif //__cscopetabbase__
