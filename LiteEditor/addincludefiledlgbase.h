 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __addincludefiledlgbase__
#define __addincludefiledlgbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stc/stc.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AddIncludeFileDlgBase
///////////////////////////////////////////////////////////////////////////////
class AddIncludeFileDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlLineToAdd;
		wxButton* m_buttonClearCachedPaths;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlFullPath;
		wxStaticText* m_staticText6;
		wxStyledTextCtrl* m_textCtrlPreview;
		wxButton* m_buttonUp;
		wxButton* m_buttonDown;
		wxStaticLine* m_staticline5;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextUpdated( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearCachedPaths( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonUp( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonDown( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		AddIncludeFileDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add Include File:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 626,479 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~AddIncludeFileDlgBase();
	
};

#endif //__addincludefiledlgbase__
