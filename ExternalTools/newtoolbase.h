///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newtoolbase__
#define __newtoolbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewToolBase
///////////////////////////////////////////////////////////////////////////////
class NewToolBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText5;
		wxChoice* m_choiceId;
		
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrlName;
		
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlPath;
		wxButton* m_buttonBrowsePath;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlWd;
		wxButton* m_buttonBrowseWd;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlArguments;
		
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textCtrlIcon24;
		wxButton* m_buttonBrowseIcon24;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrlIcon16;
		wxButton* m_buttonBrowseIcon16;
		wxCheckBox* m_checkBoxSaveAllFilesBefore;
		wxCheckBox* m_checkBoxCaptureProcessOutput;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		wxButton* m_buttonHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnIdSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowsePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseWD( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseIcon24( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseIcon16( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonHelp( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NewToolBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("External Tool"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NewToolBase();
	
};

#endif //__newtoolbase__
