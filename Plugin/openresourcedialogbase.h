///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __openresourcedialogbase__
#define __openresourcedialogbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
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
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlResourceName;
		wxChoice* m_choiceResourceType;
		wxStaticText* m_staticText1;
		wxListView *m_listOptions;
		wxStaticText* m_fullText;
		wxStaticText* m_staticTextErrorMessage;
		wxCheckBox* m_checkBoxUsePartialMatching;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnType( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUsePartialMatching( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		OpenResourceDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open Resource"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~OpenResourceDialogBase();
	
};

#endif //__openresourcedialogbase__
