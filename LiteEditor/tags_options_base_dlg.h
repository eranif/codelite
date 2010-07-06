///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tags_options_base_dlg__
#define __tags_options_base_dlg__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TagsOptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class TagsOptionsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_mainBook;
		wxPanel* m_generalPage;
		wxCheckBox* m_checkBoxMarkTagsFilesInBold;
		wxCheckBox* m_checkDisableParseOnSave;
		wxCheckBox* m_checkBoxretagWorkspaceOnStartup;
		wxCheckBox* m_checkDisplayTypeInfo;
		wxCheckBox* m_checkDisplayFunctionTip;
		wxCheckBox* m_checkCppKeywordAssist;
		wxCheckBox* m_checkBoxDeepUsingNamespaceResolving;
		
		wxStaticText* m_staticText72;
		wxSpinCtrl* m_spinCtrlMaxItemToColour;
		wxCheckBox* m_checkColourLocalVars;
		wxCheckBox* m_checkColourProjTags;
		wxCheckBox* m_checkBoxClass;
		wxCheckBox* m_checkBoxStruct;
		wxCheckBox* m_checkBoxFunction;
		wxCheckBox* m_checkBoxEnum;
		wxCheckBox* m_checkBoxEnumerator;
		wxCheckBox* m_checkBoxUnion;
		wxCheckBox* m_checkBoxPrototype;
		wxCheckBox* m_checkBoxTypedef;
		wxCheckBox* m_checkBoxMacro;
		wxCheckBox* m_checkBoxNamespace;
		wxCheckBox* m_checkBoxMember;
		wxCheckBox* m_checkBoxVariable;
		wxPanel* m_panelParser;
		wxStaticText* m_staticText7;
		wxListBox* m_listBoxSearchPaths;
		wxButton* m_buttonAddSearchPath;
		wxButton* m_buttonRemovePath;
		wxButton* m_buttonClearAll;
		wxStaticText* m_staticText71;
		wxListBox* m_listBoxSearchPaths1;
		wxButton* m_buttonAddExcludePath;
		wxButton* m_buttonRemoveExcludePath;
		wxButton* m_buttonClearAllExcludPath;
		wxPanel* m_ctagsPage;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textFileSpec;
		wxStaticText* m_staticText5;
		wxComboBox* m_comboBoxLang;
		wxCheckBox* m_checkFilesWithoutExt;
		wxStaticLine* m_staticline2;
		wxNotebook* m_notebook2;
		wxPanel* m_panel4;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textPrep;
		wxPanel* m_panel5;
		wxStaticText* m_staticText91;
		wxTextCtrl* m_textTypes;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnColourWorkspaceUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRemoveSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSearchPathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAllUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnAddExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddExcludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRemoveExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveExcludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearAllExcludePaths( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAllExcludePathsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TagsOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Tags Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TagsOptionsBaseDlg();
	
};

#endif //__tags_options_base_dlg__
