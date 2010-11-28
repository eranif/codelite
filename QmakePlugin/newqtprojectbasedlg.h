///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newqtprojectbasedlg__
#define __newqtprojectbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewQtProjBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewQtProjBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlProjName;
		
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrl;
		wxButton* m_buttonSelectProjectPath;
		wxStaticText* m_staticText13;
		wxChoice* m_choiceProjKind;
		
		wxStaticText* m_staticText12;
		wxChoice* m_choiceQmake;
		wxButton* m_buttonSelectConfig;
		wxCheckBox* m_checkBoxUseSepDirectory;
		wxButton* m_buttonCancel;
		wxButton* m_buttonOk;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowseProjectPath( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNewQmakeSettings( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		NewQtProjBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Qmake project"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~NewQtProjBaseDlg();
	
};

#endif //__newqtprojectbasedlg__
