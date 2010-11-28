 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __movefuncimplbasedlg__
#define __movefuncimplbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "filepicker.h"
#include <wx/wxscintilla.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MoveFuncImplBaseDlg
///////////////////////////////////////////////////////////////////////////////
class MoveFuncImplBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText4;
		FilePicker *m_filePicker;
		wxStaticText* m_staticText5;
		wxScintilla* m_preview;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOK;
		wxButton* m_buttoncancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		MoveFuncImplBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Move Function Implementation Preview"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 746,422 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~MoveFuncImplBaseDlg();
	
};

#endif //__movefuncimplbasedlg__
