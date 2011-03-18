//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newtooldlg.cpp              
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

#include <wx/msgdlg.h>
#include "externaltooldlg.h"
#include "externaltoolsdata.h"
#include "newtooldlg.h"
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include "macrosdlg.h"
#include "workspace.h"

NewToolDlg::NewToolDlg( wxWindow* parent, IManager *mgr, ExternalToolData *data)
		: NewToolBase( parent )
		, m_mgr(mgr)
{
	// Don't use wxFB to load translated versions of these strings: the translations wouldn't be very sensible, and it might break things
	const wxString IdChoices[] = { wxT("external_tool_0"), wxT("external_tool_1"), wxT("external_tool_2"), wxT("external_tool_3"), wxT("external_tool_4"),
									wxT("external_tool_5"), wxT("external_tool_6"), wxT("external_tool_7"), wxT("external_tool_8"), wxT("external_tool_9") };
	wxArrayString choices(10, IdChoices);
	m_choiceId->Clear();
	m_choiceId->Append(choices);
	m_choiceId->SetFocus();
	if( data ) {
		m_textCtrlArguments->SetValue(data->m_args);
		m_choiceId->SetStringSelection(data->m_id);
		m_textCtrlPath->SetValue(data->m_path);
		m_textCtrlWd->SetValue(data->m_workingDirectory);
		m_textCtrlIcon16->SetValue(data->m_icon16);
		m_textCtrlIcon24->SetValue(data->m_icon24);
		m_textCtrlName->SetValue(data->m_name);
		m_checkBoxCaptureProcessOutput->SetValue(data->m_captureOutput);
		m_checkBoxSaveAllFilesBefore->SetValue(data->m_saveAllFiles);
	}
}

void NewToolDlg::OnButtonBrowsePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path = m_textCtrlPath->GetValue();
	wxString new_path = wxFileSelector(_("Select a program:"), path.c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlPath->SetValue(new_path);
	}
}

void NewToolDlg::OnButtonBrowseWD( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path(m_textCtrlWd->GetValue());
	wxString new_path = wxDirSelector(_("Select working directory:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrlWd->SetValue(new_path);
	}
}

void NewToolDlg::OnButtonHelp( wxCommandEvent& event )
{
	wxUnusedVar(event);

	wxString errMsg = wxT("");
	wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
	ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
	IEditor* editor = m_mgr->GetActiveEditor();

	MacrosDlg dlg(this, MacrosDlg::MacrosExternalTools, project, editor);
	dlg.ShowModal();
}

void NewToolDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int rc(wxID_OK);

	// load all the tools
	ExternalToolsData inData;
	m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);
	for(size_t i=0; i<inData.GetTools().size(); i++){
		if(GetToolId() == inData.GetTools().at(i).GetId()){
			int answer = wxMessageBox(wxString::Format(_("Continue updating tool ID '%s'"), GetToolId().c_str()), _("CodeLite"), wxYES_NO|wxCANCEL, this);
			if(answer != wxYES){
				rc = wxID_CANCEL;
			}
			break;
		}
	}

	EndModal(rc);
}

void NewToolDlg::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}
void NewToolDlg::OnButtonBrowseIcon16(wxCommandEvent& event)
{
	wxUnusedVar(event);
	wxString path = m_textCtrlIcon16->GetValue();
	wxString new_path = wxFileSelector(_("Select an icon:"), path.c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlIcon16->SetValue(new_path);
	}
}

void NewToolDlg::OnButtonBrowseIcon24(wxCommandEvent& event)
{
	wxUnusedVar(event);
	wxString path = m_textCtrlIcon24->GetValue();
	wxString new_path = wxFileSelector(_("Select an icon:"), path.c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlIcon24->SetValue(new_path);
	}
}

