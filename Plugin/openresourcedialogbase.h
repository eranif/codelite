///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __OPENRESOURCEDIALOGBASE_H__
#define __OPENRESOURCEDIALOGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OpenResourceDialogBase
///////////////////////////////////////////////////////////////////////////////
class OpenResourceDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrlResourceName;
		wxStaticText* m_staticText1;
		wxListView *m_listOptions;
		wxStaticText* m_fullText;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		OpenResourceDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open Resource"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,300 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~OpenResourceDialogBase();
	
};

#endif //__OPENRESOURCEDIALOGBASE_H__
