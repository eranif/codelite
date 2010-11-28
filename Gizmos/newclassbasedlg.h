///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newclassbasedlg__
#define __newclassbasedlg__

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
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_ADD_INHERITANCE 1000
#define ID_DELETE_INHERITANCE 1001

///////////////////////////////////////////////////////////////////////////////
/// Class NewClassBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewClassBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textClassName;
		wxStaticText* m_staticText9;
		
		wxStaticText* m_staticTextNamespace;
		wxTextCtrl* m_textCtrlNamespace;
		wxButton* m_buttonBrowseNamespaces;
		
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrlBlockGuard;
		
		
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlGenFilePath;
		wxButton* m_buttonBrowseFolder;
		
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlVD;
		wxButton* m_buttonSelectVD;
		wxStaticText* m_staticText8;
		wxStaticLine* m_staticline6;
		wxCheckBox* m_checkBoxInline;
		wxCheckBox* m_checkBoxUseUnderscores;
		wxTextCtrl* m_textCtrlFileName;
		wxStaticText* m_staticText7;
		wxStaticLine* m_staticline4;
		wxListCtrl* m_listCtrl1;
		wxButton* m_buttonAddInheritance;
		wxButton* m_buttonDelInheritance;
		wxCheckBox* m_checkBoxCopyable;
		wxCheckBox* m_checkBoxImplVirtual;
		wxCheckBox* m_checkBoxSingleton;
		wxCheckBox* m_checkBoxImplPureVirtual;
		wxCheckBox* m_checkBoxVirtualDtor;
		wxStaticLine* m_staticline5;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseNamespace( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseFolder( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseVD( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckInline( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUseUnderscores( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemDeSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnButtonAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonDeleteUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCheckImpleAllVirtualFunctions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NewClassBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Class"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~NewClassBaseDlg();
	
};

#endif //__newclassbasedlg__
