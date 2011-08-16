//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : debugcoredump.cpp              
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

#include "manager.h"
#include "project.h"
#include "windowattrmanager.h"
#include "editor_config.h"
#include "globals.h"
#include "debuggermanager.h"
#include "debugcoredumpinfo.h"
#include "debugcoredump.h"
#include <wx/display.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>


DebugCoreDumpDlg::DebugCoreDumpDlg(wxWindow* parent) : DebugCoreDumpDlgBase(parent)
{
	// Prevent enormously-long strings from crowding the browse buttons off the screen
	wxDisplay display;
	int width = display.GetClientArea().GetWidth();
	wxSize size(width/4, -1);
	m_Core->SetInitialSize(size);
	m_ExeFilepath->SetInitialSize(size);
	m_WD->SetInitialSize(size);

	SetMaxSize(wxSize(width*2/3, -1));
	GetSizer()->Layout();
	GetSizer()->Fit(this);

	WindowAttrManager::Load(this, wxT("DebugCoreDumpDlgAttr"), NULL);

	Initialize();
	if (m_Core->IsEmpty()) {
		m_Core->SetFocus();
	} else {
		m_buttonDebug->SetFocus();
	}
}

void DebugCoreDumpDlg::Initialize()
{
	DebugCoreDumpInfo info;
	EditorConfigST::Get()->ReadObject(wxT("DebugCoreDumpDlg"), &info);
	
	m_choiceDebuggers->Append(DebuggerMgr::Get().GetAvailableDebuggers());
	if (m_choiceDebuggers->GetCount()){
		m_choiceDebuggers->SetSelection(0);
	}
	if (m_choiceDebuggers->GetCount() > (unsigned int)info.GetSelectedDbg()){
		m_choiceDebuggers->SetSelection(info.GetSelectedDbg());
	}

	m_Core->Append(info.GetCoreFilepaths());
	if (m_Core->GetCount() > 0) {
		m_Core->SetSelection(0);
	}

	m_WD->Append(info.GetWds());
	if (m_WD->GetCount() > 0) {
		m_WD->SetSelection(0);
	}	
	
	m_ExeFilepath->Append(info.GetExeFilepaths());
	if (m_ExeFilepath->GetCount() > 0) {
		m_ExeFilepath->SetSelection(0);
	} else {
		wxString activename = ManagerST::Get()->GetActiveProjectName();
		if (!activename.empty()) {
			wxString errMsg;
			ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(activename, errMsg);
			if (proj) {
				if (m_ExeFilepath->Append(proj->GetFileName().GetFullPath()) != wxNOT_FOUND) {
					m_ExeFilepath->SetSelection(0);
				}
				if (m_WD->Insert(proj->GetFileName().GetPath(), 0) != wxNOT_FOUND) {
					m_WD->SetSelection(0);
				}
			}
		}
	}
}

void DebugCoreDumpDlg::OnButtonBrowseCore(wxCommandEvent& event)
{
	wxUnusedVar(event);

	wxString path, ans;
	wxFileName fn(GetCore());
	if (fn.FileExists()) {
		// Use the serialised path as the wxFileSelector default path
		path = fn.GetPath();
	} else {
		// Otherwise use any working dir entry, which might just have been altered
		path = GetWorkingDirectory();
	}

	ans = wxFileSelector(_("Select core dump:"), path);
	if (!ans.empty()) {
		m_Core->Insert(ans, 0);
		m_Core->SetSelection(0);
	}
}

void DebugCoreDumpDlg::OnButtonBrowseExe(wxCommandEvent& event)
{
	wxUnusedVar(event);

	wxString path, ans;
	wxFileName fn(GetExe());
	if (fn.FileExists()) {
		// Use the serialised path as the wxFileSelector default path
		path = fn.GetPath();
	} else {
		// Otherwise use any working dir entry, which might just have been altered
		path = GetWorkingDirectory();
	}

	ans = wxFileSelector(_("Select file:"), path);
	if (!ans.empty()) {
		m_ExeFilepath->Insert(ans, 0);
		m_ExeFilepath->SetSelection(0);
	}
}

void DebugCoreDumpDlg::OnButtonBrowseWD(wxCommandEvent& event)
{
	wxUnusedVar(event);

	wxString ans, path(GetWorkingDirectory());
	if (!wxFileName::DirExists(path)){
		path = wxGetCwd();
	}

	ans = wxDirSelector(_("Select working directory:"), path);
	if (!ans.empty()) {
		m_WD->Insert(ans, 0);
		m_WD->SetSelection(0);
	}
}

void DebugCoreDumpDlg::OnButtonCancel(wxCommandEvent& event)
{
	wxUnusedVar(event);
	WindowAttrManager::Save(this, wxT("DebugCoreDumpDlgAttr"), NULL);
	EndModal(wxID_CANCEL);
}

void DebugCoreDumpDlg::OnButtonDebug(wxCommandEvent& event)
{
	wxUnusedVar(event);
	
	// save values
	const size_t MAX_NO_ITEMS = 10;
	DebugCoreDumpInfo info;
	info.SetCoreFilepaths( ReturnWithStringPrepended(m_Core->GetStrings(), GetCore(), MAX_NO_ITEMS) );
	info.SetExeFilepaths( ReturnWithStringPrepended(m_ExeFilepath->GetStrings(), GetExe(), MAX_NO_ITEMS) );
	info.SetWDs( ReturnWithStringPrepended(m_WD->GetStrings(), GetWorkingDirectory(), MAX_NO_ITEMS) );
	info.SetSelectedDbg(m_choiceDebuggers->GetSelection());

	EditorConfigST::Get()->WriteObject(wxT("DebugCoreDumpDlg"), &info);
	
	WindowAttrManager::Save(this, wxT("DebugCoreDumpDlgAttr"), NULL);
	EndModal(wxID_OK);
}

void DebugCoreDumpDlg::OnDebugBtnUpdateUI(wxUpdateUIEvent& event)
{
	// gdb needs both a core and an exe for useful debugging
	event.Enable(!GetCore().empty() && !GetExe().empty());
}
