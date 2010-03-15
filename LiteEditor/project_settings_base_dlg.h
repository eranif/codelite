///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __project_settings_base_dlg__
#define __project_settings_base_dlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/checklst.h>
#include <wx/listctrl.h>
#include <wx/statbox.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectSettingsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ProjectSettingsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText21;
		wxChoice* m_choiceConfigurationType;
		wxButton* m_buttonConfigManager;
		wxStaticLine* m_staticline81;
		wxPanel* m_panelSettings;
		wxBoxSizer* m_sizerSettings;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonHelp;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnConfigurationTypeSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonConfigurationManager( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonApply( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectSettingsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Project Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 734,502 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ProjectSettingsBaseDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectConfigurationBasePanel
///////////////////////////////////////////////////////////////////////////////
class ProjectConfigurationBasePanel : public wxPanel 
{
	private:
	
	protected:
		wxNotebook* m_notebook;
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
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_textCommand;
		wxButton* m_buttonBrowseProgram;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_textCommandArguments;
		
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrlCommandWD;
		wxButton* m_buttonBrowseCommandWD;
		wxCheckBox* m_checkBoxPauseWhenExecEnds;
		wxPanel* m_compilerPage;
		wxCheckBox* m_checkCompilerNeeded;
		wxStaticText* m_staticText331;
		wxChoice* m_choiceCmpUseWithGlobalSettings;
		wxStaticLine* m_staticline7;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCompilerOptions;
		wxButton* m_buttonCompilerOptions;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textAdditionalSearchPath;
		wxButton* m_buttonAddSearchPath;
		wxStaticText* m_staticText171;
		wxTextCtrl* m_textPreprocessor;
		wxButton* m_buttonAddPreprocessor;
		wxStaticLine* m_staticline14;
		wxStaticText* m_staticTextPreCompiledHeader;
		wxTextCtrl* m_textCtrlPreCompiledHeader;
		wxButton* m_buttonBrowsePreCompiledHeader;
		wxPanel* m_linkerPage;
		wxCheckBox* m_checkLinkerNeeded;
		wxStaticText* m_staticText3311;
		wxChoice* m_choiceLnkUseWithGlobalSettings;
		wxStaticLine* m_staticline8;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textLinkerOptions;
		wxButton* m_buttonLinkerOptions;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textLibraryPath;
		wxButton* m_buttonLibraryPath;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textLibraries;
		wxButton* m_buttonLibraries;
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
		wxPanel* m_resourceCmpPage;
		wxCheckBox* m_checkResourceNeeded;
		wxStaticText* m_staticText33111;
		wxChoice* m_choiceResUseWithGlobalSettings;
		wxStaticLine* m_staticline9;
		wxStaticText* m_staticText221;
		wxTextCtrl* m_textAddResCmpOptions;
		wxButton* m_buttonAddResCmpOptions;
		wxStaticText* m_staticText23;
		wxTextCtrl* m_textAddResCmpPath;
		wxButton* m_buttonAddResCmpPath;
		wxPanel* m_preBuildPage;
		wxStaticText* m_staticText11;
		wxStaticLine* m_staticline2;
		wxCheckListBox* m_checkListPreBuildCommands;
		wxButton* m_buttonNewPreBuildCmd;
		wxButton* m_buttonDeletePreBuildCmd;
		wxButton* m_buttonEditPreBuildCmd;
		wxButton* m_buttonUpPreBuildCmd;
		wxButton* m_buttonDownPreBuildCmd;
		wxPanel* m_postBuildPage;
		wxStaticText* m_staticText111;
		wxStaticLine* m_staticline21;
		wxCheckListBox* m_checkListPostBuildCommands;
		wxButton* m_buttonNewPostBuildCmd;
		wxButton* m_buttonDeletePostBuildCmd;
		wxButton* m_buttonEditPostBuildCmd;
		wxButton* m_buttonUpPostBuildCmd;
		wxButton* m_buttonDownPostBuildCmd;
		wxPanel* m_customBuildPage;
		wxCheckBox* m_checkEnableCustomBuild;
		wxStaticLine* m_staticline12;
		wxStaticText* m_staticText33;
		wxTextCtrl* m_textCtrlCustomBuildWD;
		wxButton* m_buttonBrowseCustomBuildWD;
		wxListCtrl* m_listCtrlTargets;
		wxStaticLine* m_staticline13;
		wxButton* m_buttonNewCustomTarget;
		wxButton* m_buttonEditCustomTarget;
		wxButton* m_buttonDeleteCustomTarget;
		wxStaticText* m_staticText29;
		wxStaticLine* m_staticline11;
		wxChoice* m_thirdPartyTool;
		wxStaticText* m_staticText30;
		wxTextCtrl* m_textCtrlMakefileGenerationCmd;
		wxPanel* m_customMakefileStep;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textDeps;
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textPreBuildRule;
		wxStaticText* m_staticText24;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseIntermediateDir( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseProgram( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseCommandWD( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckCompilerNeeded( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddCompilerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddPreprocessor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowsePreCmpHeader( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckLinkerNeeded( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddLinkerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibraryPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibrary( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectDebuggerPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDebuggingRemoteTarget( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpNeeded( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddOption( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewPreBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeletePreBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPreBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpPreBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDownPreBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPostBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewPostBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeletePostBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpPostBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDownPostBuildCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCustomBuildEnabled( wxCommandEvent& event ) { event.Skip(); }
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
		
		ProjectConfigurationBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 596,424 ), long style = wxTAB_TRAVERSAL );
		~ProjectConfigurationBasePanel();
	
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
		virtual void OnAddSearchPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddPreprocessor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonAddLinkerOptions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibraryPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddLibrary( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddOption( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResourceCmpAddPath( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GlobalSettingsBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~GlobalSettingsBasePanel();
	
};

#endif //__project_settings_base_dlg__
