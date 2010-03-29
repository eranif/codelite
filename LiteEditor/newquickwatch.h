///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
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
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewQuickWatch
///////////////////////////////////////////////////////////////////////////////
class NewQuickWatch : public wxPanel 
{
	private:
	
	protected:
		wxTreeCtrl* m_treeCtrl;
		wxStaticLine* m_staticline1;
		wxCheckBox* m_checkBoxShowMoreFormats;
		wxPanel* m_panelExtra;
		wxStaticText* m_staticText1;
		wxStaticText* m_hexFormat;
		wxStaticText* m_staticText2;
		wxStaticText* m_binFormat;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnItemExpanded( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnExpandItem( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnItemMenu( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnShowHexAndBinFormat( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NewQuickWatch( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,200 ), long style = wxSIMPLE_BORDER|wxTAB_TRAVERSAL|wxWANTS_CHARS );
		~NewQuickWatch();
	
};

#endif //__newquickwatch__
