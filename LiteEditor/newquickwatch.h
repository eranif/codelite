///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newquickwatch__
#define __newquickwatch__

#include <wx/intl.h>

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewQuickWatch
///////////////////////////////////////////////////////////////////////////////
class NewQuickWatch : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
		wxTreeCtrl* m_treeCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ){ event.Skip(); }
		virtual void OnMouseMove( wxMouseEvent& event ){ event.Skip(); }
		virtual void OnEditLabelStart( wxTreeEvent& event ){ event.Skip(); }
		virtual void OnEditLabelEnd( wxTreeEvent& event ){ event.Skip(); }
		virtual void OnItemExpanded( wxTreeEvent& event ){ event.Skip(); }
		virtual void OnExpandItem( wxTreeEvent& event ){ event.Skip(); }
		virtual void OnItemMenu( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		NewQuickWatch( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Display Variable"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,150 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~NewQuickWatch();
	
};

#endif //__newquickwatch__
