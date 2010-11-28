///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __virtualdirectoryselectorbase__
#define __virtualdirectoryselectorbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include "codelite_exports.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class VirtualDirectorySelectorBase
///////////////////////////////////////////////////////////////////////////////
class WXDLLIMPEXP_SDK VirtualDirectorySelectorBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTreeCtrl* m_treeCtrl;
		wxStaticText* m_staticTextPreview;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxTreeEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOkUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		VirtualDirectorySelectorBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Virtual Directory Selector"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~VirtualDirectorySelectorBase();
	
};

#endif //__virtualdirectoryselectorbase__
