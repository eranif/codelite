///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tags_options_base_dlg__
#define __tags_options_base_dlg__

#include <wx/intl.h>

#include <wx/treebook.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/hyperlink.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/filepicker.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TagsOptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class TagsOptionsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxTreebook *m_treebook;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TagsOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tags Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TagsOptionsBaseDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CCDisplayAndBehaviorBase
///////////////////////////////////////////////////////////////////////////////
class CCDisplayAndBehaviorBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxMarkTagsFilesInBold;
		wxCheckBox* m_checkDisplayTypeInfo;
		wxCheckBox* m_checkDisplayFunctionTip;
		wxCheckBox* m_checkCppKeywordAssist;
		wxCheckBox* m_checkBoxretagWorkspaceOnStartup;
		wxCheckBox* m_checkDisableParseOnSave;
		wxCheckBox* m_checkBoxDeepUsingNamespaceResolving;
	
	public:
		
		CCDisplayAndBehaviorBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CCDisplayAndBehaviorBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CCColouringBasePage
///////////////////////////////////////////////////////////////////////////////
class CCColouringBasePage : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText72;
		wxSpinCtrl* m_spinCtrlMaxItemToColour;
		wxCheckBox* m_checkColourLocalVars;
		wxCheckBox* m_checkBoxColourMacroBlocks;
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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnColourWorkspaceUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CCColouringBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CCColouringBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CCTriggering
///////////////////////////////////////////////////////////////////////////////
class CCTriggering : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkWordAssist;
		
		wxStaticText* m_staticTextMinWordLen;
		wxSlider* m_sliderMinWordLen;
		wxCheckBox* m_checkAutoInsertSingleChoice;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAutoShowWordAssitUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CCTriggering( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CCTriggering();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CCIncludeFilesBasePage
///////////////////////////////////////////////////////////////////////////////
class CCIncludeFilesBasePage : public wxPanel 
{
	private:
	
	protected:
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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRemoveSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSearchPathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAllUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnItemActivatedExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddExcludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRemoveExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveExcludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearAllExcludePaths( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAllExcludePathsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CCIncludeFilesBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CCIncludeFilesBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CCAdvancedBasePage
///////////////////////////////////////////////////////////////////////////////
class CCAdvancedBasePage : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textFileSpec;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText92;
		wxTextCtrl* m_textCtrlFilesList;
		wxButton* m_buttonParse;
		wxNotebook* m_notebook2;
		wxPanel* m_panel4;
		wxTextCtrl* m_textPrep;
		wxStaticText* m_staticText9;
		wxHyperlinkCtrl* m_hyperlink1;
		wxPanel* m_panel5;
		wxStaticText* m_staticText91;
		wxTextCtrl* m_textTypes;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnParse( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileSelectedUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CCAdvancedBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CCAdvancedBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CCClangBasePage
///////////////////////////////////////////////////////////////////////////////
class CCClangBasePage : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxEnableClangCC;
		wxNotebook* m_notebook2;
		wxPanel* m_panel3;
		wxCheckBox* m_checkBoxClangFirst;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel8;
		wxTextCtrl* m_textCtrlClangSearchPaths;
		wxButton* m_buttonSuggest;
		wxPanel* m_panel6;
		wxSplitterWindow* m_splitter3;
		wxPanel* m_panel9;
		wxTextCtrl* m_textCtrlClangOptions;
		wxPanel* m_panel10;
		wxTextCtrl* m_textCtrlClangMacros;
		wxPanel* m_panel4;
		wxStaticText* m_staticText10;
		wxFilePickerCtrl* m_filePickerClang;
		wxStaticText* m_staticText101;
		wxButton* m_buttonClearCache;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClangCCEnabledUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnSuggestSearchPaths( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearClangCache( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearClangCacheUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CCClangBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CCClangBasePage();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 0 );
		m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CCClangBasePage::m_splitter1OnIdle ), NULL, this );
		}
		
		void m_splitter3OnIdle( wxIdleEvent& )
		{
		m_splitter3->SetSashPosition( 0 );
		m_splitter3->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CCClangBasePage::m_splitter3OnIdle ), NULL, this );
		}
		
	
};

#endif //__tags_options_base_dlg__
