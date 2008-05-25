//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : project_settings_base_dlg.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
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
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/gbsizer.h>
#include <wx/checklst.h>
#include "dirpicker.h"
#include <wx/statbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

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
		wxNotebook* m_notebook3;
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
		DirPicker *m_intermediateDirPicker;
		wxStaticText* m_staticText17;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_textCommand;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_textCommandArguments;
		wxStaticText* m_staticText20;
		DirPicker *m_workingDirPicker;
		wxCheckBox* m_checkBoxPauseWhenExecEnds;
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
		wxStaticText* m_staticText27;
		DirPicker *m_customBuildDirPicker;
		wxStaticText* m_staticText181;
		wxTextCtrl* m_textBuildCommand;
		wxStaticText* m_staticText291;
		wxTextCtrl* m_textCtrl1SingleFileCommand;
		wxStaticText* m_staticText192;
		wxTextCtrl* m_textCleanCommand;
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
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCmdEvtVModified( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnChoiceMakefileTool( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		ProjectSettingsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Project Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 782,502 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProjectSettingsBaseDlg();
	
};

#endif //__project_settings_base_dlg__
