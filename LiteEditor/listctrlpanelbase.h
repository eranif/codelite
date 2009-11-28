///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __listctrlpanelbase__
#define __listctrlpanelbase__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ListCtrlPanelBase
///////////////////////////////////////////////////////////////////////////////
class ListCtrlPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxListCtrl* m_listCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemRightClicked( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		
		ListCtrlPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~ListCtrlPanelBase();
	
};

#endif //__listctrlpanelbase__
