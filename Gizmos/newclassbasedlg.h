///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newclassbasedlg__
#define __newclassbasedlg__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include "dirpicker.h"
#include <wx/checkbox.h>
#include <wx/listctrl.h>
#include <wx/button.h>
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
		wxPanel* m_mainPanel;
		wxStaticBitmap* m_bmp;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textClassName;
		wxStaticText* m_staticText2;
		DirPicker *m_dirPicker;
		wxStaticLine* m_staticline6;
		wxCheckBox* m_checkBoxEnterFileName;
		wxTextCtrl* m_textCtrlFileName;
		wxStaticLine* m_staticline4;
		wxStaticText* m_staticText9;
		wxListCtrl* m_listCtrl1;
		wxButton* m_buttonAddInheritance;
		wxButton* m_buttonDelInheritance;
		wxCheckBox* m_checkBoxCopyable;
		wxCheckBox* m_checkBoxImplVirtual;
		wxCheckBox* m_checkBox6;
		wxCheckBox* m_checkBoxImplPureVirtual;
		wxCheckBox* m_checkBoxVirtualDtor;
		wxStaticLine* m_staticline5;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextEnter( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckEnterFileNameManually( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnListItemDeSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnButtonAdd( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonDelete( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonDeleteUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnCheckImpleAllVirtualFunctions( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		NewClassBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("New Class"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 690,631 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__newclassbasedlg__
