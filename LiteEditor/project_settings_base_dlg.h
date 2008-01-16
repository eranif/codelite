///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __project_settings_base_dlg__
#define __project_settings_base_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/checklst.h>
#include <wx/treebook.h>
#include "dirpicker.h"

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
		wxTreebook* m_notebook3;
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
		DirPicker* m_intermediateDirPicker;
		wxStaticText* m_staticText17;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_textCommand;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_textCommandArguments;
		wxStaticText* m_staticText20;
		DirPicker* m_workingDirPicker;
		wxPanel* m_compilerPage;
		wxCheckBox* m_checkCompilerNeeded;
		wxStaticLine* m_staticline7;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCompilerOptions;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textAdditionalSearchPath;
		wxButton* m_buttonAddSearchPath;
		wxStaticText* m_staticText171;
		wxTextCtrl* m_textPreprocessor;
		wxButton* m_buttonAddPreprocessor;
		wxButton* m_buttonCompilerOptions;
		wxPanel* m_linkerPage;
		wxCheckBox* m_checkLinkerNeeded;
		wxStaticLine* m_staticline8;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textLinkerOptions;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textLibraryPath;
		wxButton* m_buttonLibraries;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textLibraries;
		wxButton* m_buttonLibraryPath;
		wxButton* m_buttonLinkerOptions;
		wxPanel* m_resourceCmpPage;
		wxCheckBox* m_checkResourceNeeded;
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
		wxStaticLine* m_staticline82;
		wxStaticText* m_staticText181;
		wxTextCtrl* m_textBuildCommand;
		wxStaticText* m_staticText192;
		wxTextCtrl* m_textCleanCommand;
		wxStaticText* m_staticText201;
		wxPanel* m_customMakefileStep;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textDeps;
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textPreBuildRule;
		wxStaticText* m_staticText24;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
	
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ){ event.Skip(); }

	public:
		ProjectSettingsBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Project Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 782,502 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__project_settings_base_dlg__
