///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cppchecksettingsdlgbase__
#define __cppchecksettingsdlgbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class CppCheckSettingsDialogBase
///////////////////////////////////////////////////////////////////////////////
class CppCheckSettingsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxCheckBox* m_cbOptionAll;
		wxCheckBox* m_cbOptionForce;
		wxCheckBox* m_cbOptionStyle;
		wxCheckBox* m_cbOptionUnusedFunctions;
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxListBox* m_listBoxExcludelist;
		wxButton* m_buttonAdd;
		wxButton* m_buttonRemove;
		wxButton* m_buttonClearList;
		wxStaticLine* m_staticline1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAddFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveFileUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnClearList( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearListUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnBtnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		CppCheckSettingsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("CppChecker settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 344,246 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~CppCheckSettingsDialogBase();
	
};

#endif //__cppchecksettingsdlgbase__
