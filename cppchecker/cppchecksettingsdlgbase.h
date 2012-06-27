///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  3 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CPPCHECKSETTINGSDLGBASE_H__
#define __CPPCHECKSETTINGSDLGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class CppCheckSettingsDialogBase
///////////////////////////////////////////////////////////////////////////////
class CppCheckSettingsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_ChecksPanel;
		wxStaticText* m_staticText5;
		wxCheckListBox* m_checkListExtraWarnings;
		wxButton* m_buttonAllChecks;
		wxButton* m_buttonClearChecks;
		wxCheckBox* m_cbOptionForce;
		wxPanel* m_ExcludePanel;
		wxStaticText* m_staticText1;
		wxListBox* m_listBoxExcludelist;
		wxButton* m_buttonAdd;
		wxButton* m_buttonRemove;
		wxButton* m_buttonClearList;
		wxPanel* m_SupressPanel;
		wxStaticText* m_staticText11;
		wxCheckListBox* m_checkListSuppress;
		wxButton* m_buttonAddSuppression;
		wxButton* m_buttonRemoveSuppression;
		wxButton* m_buttonSuppressAll;
		wxButton* m_buttonClearSuppressions;
		wxCheckBox* m_checkBoxSerialise;
		wxStaticLine* m_staticline1;
		wxButton* m_BtmOK;
		wxButton* m_BtnCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChecksTickAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChecksTickAllUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnChecksUntickAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChecksUntickAllUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnAddFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveFileUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearListUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnAddSuppression( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSuppression( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSuppressionUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnSuppressTickAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSuppressTickAllUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnSuppressUntickAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSuppressUntickAllUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CppCheckSettingsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("CppChecker settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~CppCheckSettingsDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CppCheckAddSuppressionDialogBase
///////////////////////////////////////////////////////////////////////////////
class CppCheckAddSuppressionDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText12;
		wxTextCtrl* m_txtDescription;
		wxStaticText* m_staticText13;
		wxTextCtrl* m_txtKey;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOKButtonUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CppCheckAddSuppressionDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("CppChecker add warning suppression"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~CppCheckAddSuppressionDialogBase();
	
};

#endif //__CPPCHECKSETTINGSDLGBASE_H__
