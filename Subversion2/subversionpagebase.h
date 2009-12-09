///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __subversionpagebase__
#define __subversionpagebase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SubversionPageBase
///////////////////////////////////////////////////////////////////////////////
class SubversionPageBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrlRootDir;
		wxButton* m_buttonChangeRootDir;
		wxTreeCtrl* m_treeCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeRootDir( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTreeMenu( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		SubversionPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~SubversionPageBase();
	
};

#endif //__subversionpagebase__
