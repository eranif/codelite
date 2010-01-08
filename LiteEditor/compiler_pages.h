///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __compiler_pages__
#define __compiler_pages__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/valtext.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CompilerPatternsBase
///////////////////////////////////////////////////////////////////////////////
class CompilerPatternsBase : public wxPanel 
{
	private:
	
	protected:
		wxListCtrl* m_listErrPatterns;
		wxButton* m_btnAddErrPattern;
		wxButton* m_btnDelErrPattern;
		wxButton* m_btnUpdateErrPattern;
		wxListCtrl* m_listWarnPatterns;
		wxButton* m_btnAddWarnPattern;
		wxButton* m_btnDelWarnPattern;
		wxButton* m_btnUpdateWarnPattern;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnErrItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnBtnAddErrPattern( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBtnDelErrPattern( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnErrorPatternSelectedUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnUpdateErrPattern( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWarnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnBtnAddWarnPattern( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBtnDelWarnPattern( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWarningPatternSelectedUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnUpdateWarnPattern( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompilerPatternsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~CompilerPatternsBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerToolsBase
///////////////////////////////////////////////////////////////////////////////
class CompilerToolsBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCompilerName;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textCtrlCCompilerName;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textLinkerName;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textSOLinker;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textArchiveTool;
		wxStaticText* m_staticText14;
		wxTextCtrl* m_textResourceCmp;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrlPathVariable;
	
	public:
		
		CompilerToolsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CompilerToolsBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerSwitchesBase
///////////////////////////////////////////////////////////////////////////////
class CompilerSwitchesBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText8;
		wxListCtrl* m_listSwitches;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompilerSwitchesBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 438,428 ), long style = wxTAB_TRAVERSAL );
		~CompilerSwitchesBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerFileTypesBase
///////////////////////////////////////////////////////////////////////////////
class CompilerFileTypesBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText23;
		wxListCtrl* m_listCtrlFileTypes;
		wxButton* m_buttonNewFileType;
		wxButton* m_buttonDeleteFileType;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileTypeActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnFileTypeDeSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnFileTypeSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnNewFileType( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteFileType( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompilerFileTypesBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~CompilerFileTypesBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerAdvanceBase
///////////////////////////////////////////////////////////////////////////////
class CompilerAdvanceBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxGenerateDependenciesFiles;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText18;
		wxStaticText* m_staticText141;
		wxTextCtrl* m_textCtrlGlobalIncludePath;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrlGlobalLibPath;
		wxStaticLine* m_staticline31;
		wxStaticText* m_staticText19;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textObjectExtension;
		wxStaticText* m_staticText24;
		wxTextCtrl* m_textDependExtension;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textPreprocessExtension;
	
	public:
		
		CompilerAdvanceBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~CompilerAdvanceBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerCompilerOptionsBase
///////////////////////////////////////////////////////////////////////////////
class CompilerCompilerOptionsBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText23;
		wxListCtrl* m_listCompilerOptions;
		wxButton* m_buttonCompilerOption;
		wxButton* m_buttonDeleteCompilerOption;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCompilerOptionActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnCompilerOptionDeSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnCompilerOptionSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnNewCompilerOption( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteCompilerOption( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompilerCompilerOptionsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~CompilerCompilerOptionsBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerLinkerOptionsBase
///////////////////////////////////////////////////////////////////////////////
class CompilerLinkerOptionsBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText23;
		wxListCtrl* m_listLinkerOptions;
		wxButton* m_buttonLinkerOption;
		wxButton* m_buttonDeleteLinkerOption;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLinkerOptionActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnLinkerOptionDeSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnLinkerOptionSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnNewLinkerOption( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteLinkerOption( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompilerLinkerOptionsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~CompilerLinkerOptionsBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerOptionDlgBase
///////////////////////////////////////////////////////////////////////////////
class CompilerOptionDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textCtrl18;
		wxStaticText* m_staticText27;
		wxTextCtrl* m_textCtrl19;
		wxStaticLine* m_staticline4;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
	
	public:
		wxString m_sName; 
		wxString m_sHelp; 
		
		CompilerOptionDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 256,140 ), long style = wxDEFAULT_DIALOG_STYLE );
		~CompilerOptionDlgBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompilerPatternDlgBase
///////////////////////////////////////////////////////////////////////////////
class CompilerPatternDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textPattern;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textFileIndex;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textLineNumber;
		wxStaticLine* m_staticline5;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSubmit( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxString m_pattern; 
		wxString m_fileIdx; 
		wxString m_lineIdx; 
		
		CompilerPatternDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 348,160 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~CompilerPatternDlgBase();
	
};

#endif //__compiler_pages__
