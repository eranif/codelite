//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : quickdebugdlg.cpp              
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

#include "windowattrmanager.h"
#include "quickdebuginfo.h"
#include <wx/tokenzr.h>
#include "editor_config.h"
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include "debuggermanager.h"
#include "quickdebugdlg.h"

QuickDebugDlg::QuickDebugDlg( wxWindow* parent )
:
QuickDebugBase( parent )
{
	Initialize();
	m_buttonDebug->SetFocus();
	GetSizer()->SetMinSize(550, wxNOT_FOUND);
	GetSizer()->Fit(this);
	
	WindowAttrManager::Load(this, wxT("QuickDebugDlgAttr"), NULL);
}

void QuickDebugDlg::Initialize()
{
	QuickDebugInfo info;
	EditorConfigST::Get()->ReadObject(wxT("QuickDebugDlg"), &info);
	
	m_choiceDebuggers->Append(DebuggerMgr::Get().GetAvailableDebuggers());
	if(m_choiceDebuggers->GetCount()){
		m_choiceDebuggers->SetSelection(0);
	}
	if(m_choiceDebuggers->GetCount() > (unsigned int)info.GetSelectedDbg()){
		m_choiceDebuggers->SetSelection(info.GetSelectedDbg());
	}
	
	m_textCtrlExePath->SetValue(info.GetExePath());
	m_textCtrlArgs->SetValue(info.GetArguments());
	
	wxString startupCmds;
	for(size_t i=0; i<info.GetStartCmds().GetCount(); i++){
		startupCmds << info.GetStartCmds().Item(i) << wxT("\n");
	}
	m_textCtrlCmds->SetValue(startupCmds);
	m_textCtrl5->SetValue(info.GetWd());
}

void QuickDebugDlg::OnButtonBrowseExe( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path(GetExe());
	if(wxFileName::FileExists(path)){
		m_textCtrlExePath->SetValue(wxFileSelector(_("Select file:"), path.c_str()));
	}else{
		m_textCtrlExePath->SetValue(wxFileSelector(_("Select file:")));
	}
}

void QuickDebugDlg::OnButtonDebug( wxCommandEvent& event )
{
	wxUnusedVar(event);
	
	// save values
	QuickDebugInfo info;
	info.SetSelectedDbg(m_choiceDebuggers->GetSelection());
	info.SetExePath(m_textCtrlExePath->GetValue());
	info.SetWd(m_textCtrl5->GetValue());
	info.SetStartCmds(GetStartupCmds());
	info.SetArguments(m_textCtrlArgs->GetValue());
	EditorConfigST::Get()->WriteObject(wxT("QuickDebugDlg"), &info);
	
	WindowAttrManager::Save(this, wxT("QuickDebugDlgAttr"), NULL);
	EndModal(wxID_OK);
}

void QuickDebugDlg::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	WindowAttrManager::Save(this, wxT("QuickDebugDlgAttr"), NULL);
	EndModal(wxID_CANCEL);
}

wxString QuickDebugDlg::GetArguments()
{
	return m_textCtrlArgs->GetValue();
}

wxString QuickDebugDlg::GetDebuggerName()
{
	return m_choiceDebuggers->GetStringSelection();
}

wxString QuickDebugDlg::GetExe()
{
	return m_textCtrlExePath->GetValue();
}

wxArrayString QuickDebugDlg::GetStartupCmds()
{
	wxString cmds = m_textCtrlCmds->GetValue();
	cmds.Trim().Trim(false);
	
	return wxStringTokenize(cmds, wxT("\n\r"), wxTOKEN_STRTOK);
}

wxString QuickDebugDlg::GetWorkingDirectory()
{
	return m_textCtrl5->GetValue();
}

void QuickDebugDlg::OnButtonBrowseWD(wxCommandEvent& event)
{
	wxUnusedVar(event);
	wxString path(GetWorkingDirectory());
	if(wxFileName::DirExists(path)){
		m_textCtrl5->SetValue(wxDirSelector(_("Select working directory:"), path));
	}else{
		m_textCtrl5->SetValue(wxDirSelector(_("Select working directory:")));
	}
}
