///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __project_settings_base_dlg__
#define __project_settings_base_dlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/treebook.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/gbsizer.h>
#include <wx/statbox.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectSettingsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ProjectSettingsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panelSettings;
		wxBoxSizer* m_sizerSettings;
		wxChoice* m_choiceConfig;
		wxTreebook *m_treebook;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonHelp;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnConfigurationChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonApply( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonApplyUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectSettingsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ProjectSettingsBaseDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSGeneralPageBase
///////////////////////////////////////////////////////////////////////////////
class PSGeneralPageBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_generalPage;
		wxStaticText* m_staticText22;
		wxChoice* m_choiceProjectTypes;
		wxStaticText* m_staticText191;
		wxChoice* m_choiceCompilerType;
		wxStaticText* m_staticText231;
		wxChoice* m_choiceDebugger;
		wxStaticText* m_staticText15;
		wxTextCtrl* m_textOutputFilePicker;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrlItermediateDir;
		wxButton* m_buttonBrowseIM_WD;
		wxStaticText* m_staticText17;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_textCommand;
		wxButton* m_buttonBrowseProgram;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_textCommandArguments;
		wxCheckBox* m_checkBoxUseDebugArgs;
		wxStaticText* m_staticText44;
		wxTextCtrl* m_textCtrlDebugArgs;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrlCommandWD;
		wxButton* m_buttonBrowseCommandWD;
		wxStaticLine* m_staticline11;
		wxCheckBox* m_checkBoxPauseWhenExecEnds;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnProjectCustumBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseIntermediateDir( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseProgram( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUseDebugArgsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBrowseCommandWD( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxGridBagSizer* m_gbSizer1;
		
		PSGeneralPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSGeneralPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSCompilerPageBase
///////////////////////////////////////////////////////////////////////////////
class PSCompilerPageBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_compilerPage;
		wxCheckBox* m_checkCompilerNeeded;
		wxGridBagSizer* m_gbSizer2;
		wxStaticText* m_staticText331;
		wxChoice* m_choiceCmpUseWithGlobalSettings;
		wxStaticLine* m_staticline7;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCompilerOptions;
		wxButton* m_buttonCompilerOptions;
		wxStaticText* m_staticText45;
		wxTextCtrl* m_textCtrlCCompilerOptions;
		wxButton* m_button35;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textAdditionalSearchPath;
		wxButton* m_buttonAddSearchPath;
		wxStaticText* m_staticText171;
		wxTextCtrl* m_textPreprocessor;
		wxButton* m_buttonAddPreprocessor;
		wxTextCtrl* m_textCtrlPreCompiledHeader;
		wxButton* m_buttonBrowsePreCompiledHeader;
		wxCheckBox* m_checkBoxPCHInCommandLine;
		wxCheckBox* m_checkBoxSeparatePCHFlags;
		wxTextCtrl* m_textCtrlPCHCompilationFlags;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckCompilerNeeded( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectCustumBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCompiledNotNeededUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddCompilerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddCCompilerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddPreprocessor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowsePreCmpHeader( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEnablePCHFLagsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSCompilerPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSCompilerPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSLinkPageBase
///////////////////////////////////////////////////////////////////////////////
class PSLinkPageBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_linkerPage;
		wxCheckBox* m_checkLinkerNeeded;
		wxGridBagSizer* m_gbSizer3;
		wxStaticText* m_staticText3311;
		wxChoice* m_choiceLnkUseWithGlobalSettings;
		
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textLinkerOptions;
		wxButton* m_buttonLinkerOptions;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textLibraryPath;
		wxButton* m_buttonLibraryPath;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textLibraries;
		wxButton* m_buttonLibraries;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckLinkerNeeded( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectCustumBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnLinkerNotNeededUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddLinkerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibraryPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibrary( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSLinkPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSLinkPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSDebuggerPageBase
///////////////////////////////////////////////////////////////////////////////
class PSDebuggerPageBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panelDebugger;
		wxStaticText* m_staticText321;
		wxTextCtrl* m_textCtrlDebuggerPath;
		wxButton* m_buttonSelectDebugger;
		wxStaticLine* m_staticline121;
		wxCheckBox* m_checkBoxDbgRemote;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textCtrl1DbgHost;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_textCtrlDbgPort;
		wxStaticLine* m_staticline131;
		wxStaticText* m_staticText301;
		wxTextCtrl* m_textCtrlDbgCmds;
		wxStaticText* m_staticText311;
		wxTextCtrl* m_textCtrlDbgPostConnectCmds;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectDebuggerPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoteDebugUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSDebuggerPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSDebuggerPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSResourcesPageBase
///////////////////////////////////////////////////////////////////////////////
class PSResourcesPageBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_resourceCmpPage;
		wxCheckBox* m_checkResourceNeeded;
		wxGridBagSizer* m_gbSizer4;
		wxStaticText* m_staticText33111;
		wxChoice* m_choiceResUseWithGlobalSettings;
		wxStaticLine* m_staticline9;
		wxStaticText* m_staticText221;
		wxTextCtrl* m_textAddResCmpOptions;
		wxButton* m_buttonAddResCmpOptions;
		wxStaticText* m_staticText23;
		wxTextCtrl* m_textAddResCmpPath;
		wxButton* m_buttonAddResCmpPath;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnResourceCmpNeeded( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectCustumBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnrResourceCompilerNotNeededUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddOption( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddPath( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSResourcesPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSResourcesPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSEnvironmentBasePage
///////////////////////////////////////////////////////////////////////////////
class PSEnvironmentBasePage : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panelEnv;
		wxStaticText* m_staticText44;
		wxChoice* m_choiceEnv;
		wxStaticText* m_staticText45;
		wxChoice* m_choiceDbgEnv;
		wxStaticLine* m_staticline12;
		wxStaticText* m_staticText47;
		wxTextCtrl* m_textCtrlEnvvars;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSEnvironmentBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSEnvironmentBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSBuildEventsBasePage
///////////////////////////////////////////////////////////////////////////////
class PSBuildEventsBasePage : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_preBuildPage;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCtrlBuildEvents;
	
	public:
		
		PSBuildEventsBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSBuildEventsBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSCustomBuildBasePage
///////////////////////////////////////////////////////////////////////////////
class PSCustomBuildBasePage : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_customBuildPage;
		wxCheckBox* m_checkEnableCustomBuild;
		wxStaticLine* m_staticline12;
		wxStaticText* m_staticText33;
		wxTextCtrl* m_textCtrlCustomBuildWD;
		wxButton* m_buttonBrowseCustomBuildWD;
		wxListCtrl* m_listCtrlTargets;
		wxButton* m_buttonNewCustomTarget;
		wxButton* m_buttonEditCustomTarget;
		wxButton* m_buttonDeleteCustomTarget;
		wxStaticText* m_staticText29;
		wxStaticLine* m_staticline11;
		wxChoice* m_thirdPartyTool;
		wxStaticText* m_staticText30;
		wxTextCtrl* m_textCtrlMakefileGenerationCmd;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCustomBuildEnabled( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCustomBuildCBEnabledUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCustomBuildEnabledUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseCustomBuildWD( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnNewTarget( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditTarget( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditTargetUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnDeleteTarget( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteTargetUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnChoiceMakefileTool( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSCustomBuildBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSCustomBuildBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GlobalSettingsBasePanel
///////////////////////////////////////////////////////////////////////////////
class GlobalSettingsBasePanel : public wxPanel 
{
	private:
	
	protected:
		wxNotebook* m_notebook;
		wxPanel* m_compilerPage;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCompilerOptions;
		wxButton* m_buttonCompilerOptions;
		wxStaticText* m_staticText46;
		wxTextCtrl* m_textCtrlCCompileOptions;
		wxButton* m_buttonCCompileOptions;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textAdditionalSearchPath;
		wxButton* m_buttonAddSearchPath;
		wxStaticText* m_staticText171;
		wxTextCtrl* m_textPreprocessor;
		wxButton* m_buttonAddPreprocessor;
		wxPanel* m_linkerPage;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textLinkerOptions;
		wxButton* m_buttonLinkerOptions;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textLibraryPath;
		wxButton* m_buttonLibraryPath;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textLibraries;
		wxButton* m_buttonLibraries;
		wxPanel* m_resourceCmpPage;
		wxStaticText* m_staticText221;
		wxTextCtrl* m_textAddResCmpOptions;
		wxButton* m_buttonAddResCmpOptions;
		wxStaticText* m_staticText23;
		wxTextCtrl* m_textAddResCmpPath;
		wxButton* m_buttonAddResCmpPath;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddCompilerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddCCompilerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddPreprocessor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddLinkerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibraryPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibrary( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddOption( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddPath( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GlobalSettingsBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~GlobalSettingsBasePanel();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSCustomMakefileBasePage
///////////////////////////////////////////////////////////////////////////////
class PSCustomMakefileBasePage : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_customMakefileStep;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textDeps;
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textPreBuildRule;
		wxStaticText* m_staticText24;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnProjectCustumBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSCustomMakefileBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSCustomMakefileBasePage();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PSCompletionBase
///////////////////////////////////////////////////////////////////////////////
class PSCompletionBase : public wxPanel 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel14;
		wxStaticText* m_staticText47;
		wxTextCtrl* m_textCtrlSearchPaths;
		wxPanel* m_panel15;
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel16;
		wxStaticText* m_staticText48;
		wxTextCtrl* m_textCtrlCmpOptions;
		wxPanel* m_panel17;
		wxStaticText* m_staticText49;
		wxTextCtrl* m_textCtrlMacros;
		wxCheckBox* m_checkBoxC11;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PSCompletionBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~PSCompletionBase();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 0 );
		m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PSCompletionBase::m_splitter1OnIdle ), NULL, this );
		}
		
		void m_splitter2OnIdle( wxIdleEvent& )
		{
		m_splitter2->SetSashPosition( 0 );
		m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PSCompletionBase::m_splitter2OnIdle ), NULL, this );
		}
		
	
};

#endif //__project_settings_base_dlg__
