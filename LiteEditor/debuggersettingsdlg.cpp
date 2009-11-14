//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggersettingsdlg.cpp
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
 #include "debuggersettingsdlg.h"
#include "windowattrmanager.h"
#include "editor_config.h"
#include "debuggermanager.h"
#include "macros.h"
#include "manager.h"
#include "globals.h"
#include "dbgcommanddlg.h"
#include "debuggerconfigtool.h"

//-------------------------------------------------------------------
DebuggerPage::DebuggerPage(wxWindow *parent, wxString title)
		: DebuggerPageBase(parent)
		, m_title(title)
{
	DebuggerInformation info;
	if(DebuggerMgr::Get().GetDebuggerInformation(m_title, info)){
		m_textCtrDbgPath->SetValue(info.path);
		m_checkBoxEnableLog->SetValue(info.enableDebugLog);
		m_checkBoxEnablePendingBreakpoints->SetValue(info.enablePendingBreakpoints);
		m_checkBreakAtWinMain->SetValue(info.breakAtWinMain);
		m_checkShowTerminal->SetValue(info.showTerminal);
		m_checkUseRelativePaths->SetValue(info.useRelativeFilePaths);
		m_catchThrow->SetValue(info.catchThrow);
		m_spinCtrlNumElements->SetValue(info.maxDisplayStringSize);
		m_showTooltips->SetValue(info.showTooltips);
		m_textCtrlStartupCommands->SetValue( info.startupCommands );

		m_checkBoxExpandLocals->SetValue(info.resolveLocals);
#ifdef __WXMSW__
		m_checkBoxDebugAssert->SetValue(info.debugAsserts);
#endif
	}

#ifndef __WXMSW__
	m_checkBoxDebugAssert->SetValue(false);
	m_checkBoxDebugAssert->Enable(false);
#endif

}

DebuggerPage::~DebuggerPage()
{
}

void DebuggerPage::OnBrowse(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxString newfilepath, filepath(m_textCtrDbgPath->GetValue());
	if ((!filepath.IsEmpty()) && wxFileName::FileExists(filepath)) {
		newfilepath = wxFileSelector(wxT("Select file:"), filepath.c_str());
	} else {
		newfilepath = wxFileSelector(wxT("Select file:"));
	}

	if (!newfilepath.IsEmpty()) {
		m_textCtrDbgPath->SetValue(newfilepath);
	}
}

void DebuggerPage::OnDebugAssert(wxCommandEvent& e)
{
	if ( e.IsChecked() ) {
		m_checkBoxEnablePendingBreakpoints->SetValue(true);
	}
}

//-------------------------------------------------------------------

DebuggerSettingsDlg::DebuggerSettingsDlg( wxWindow* parent )
		:DebuggerSettingsBaseDlg( parent )
		, m_selectedItem(wxNOT_FOUND)
{
	//fill the notebook with the available debuggers
	Initialize();
	ConnectButton(m_buttonOK, DebuggerSettingsDlg::OnOk);

	// center the dialog
	Centre();

	m_listCtrl1->SetFocus();
	GetSizer()->Fit(this);
	WindowAttrManager::Load(this, wxT("DbgSettingsDlg"), NULL);
}

void DebuggerSettingsDlg::Initialize()
{
	DebuggerMgr &mgr = DebuggerMgr::Get();
	wxArrayString debuggers = mgr.GetAvailableDebuggers();
	for (size_t i=0; i<debuggers.GetCount(); i++) {
		//create page per-debugger
		m_book->AddPage(new DebuggerPage(m_book, debuggers.Item(i)), debuggers.Item(i), true);
	}

	m_listCtrl1->InsertColumn(0, wxT("Type"));
	m_listCtrl1->InsertColumn(1, wxT("Expression"));
	m_listCtrl1->InsertColumn(2, wxT("Debugger Command"));

	//add items from the saved items
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &m_data);

	//Populate the list with the items from the configuration file
	std::vector<DebuggerCmdData> cmds = m_data.GetCmds();
	for (size_t i=0; i<cmds.size(); i++) {
		DebuggerCmdData cmd = cmds.at(i);

		long item = AppendListCtrlRow(m_listCtrl1);
		SetColumnText(m_listCtrl1, item, 0, cmd.GetName());
		SetColumnText(m_listCtrl1, item, 1, cmd.GetCommand());
		SetColumnText(m_listCtrl1, item, 2, cmd.GetDbgCommand());
	}
	m_listCtrl1->SetColumnWidth(0, 100);
	m_listCtrl1->SetColumnWidth(1, 200);
}

void DebuggerSettingsDlg::OnOk(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//go over the debuggers and set the debugger path
	for (size_t i=0; i<(size_t)m_book->GetPageCount(); i++) {
		DebuggerPage *page =  (DebuggerPage *)m_book->GetPage(i);

		//find the debugger
		DebuggerInformation info;
		DebuggerMgr::Get().GetDebuggerInformation(page->m_title, info);

		//populate the information and save it
		info.enableDebugLog           = page->m_checkBoxEnableLog->GetValue();
		info.enablePendingBreakpoints = page->m_checkBoxEnablePendingBreakpoints->GetValue();
		info.path                     = page->m_textCtrDbgPath->GetValue();
		info.name                     = page->m_title;
		info.breakAtWinMain           = page->m_checkBreakAtWinMain->IsChecked();
		info.showTerminal             = page->m_checkShowTerminal->IsChecked();
		info.consoleCommand           = EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand();
		info.useRelativeFilePaths     = page->m_checkUseRelativePaths->IsChecked();
		info.catchThrow               = page->m_catchThrow->IsChecked();
		info.showTooltips             = page->m_showTooltips->IsChecked();
		info.startupCommands          = page->m_textCtrlStartupCommands->GetValue();
		info.maxDisplayStringSize     = page->m_spinCtrlNumElements->GetValue();
		info.resolveLocals            = page->m_checkBoxExpandLocals->IsChecked();
#ifdef __WXMSW__
		info.debugAsserts             = page->m_checkBoxDebugAssert->IsChecked();
#endif
		DebuggerMgr::Get().SetDebuggerInformation(page->m_title, info);
	}

	//copy the commands the serialized object m_data
	int count = m_listCtrl1->GetItemCount();
	std::vector<DebuggerCmdData> cmdArr;
	for(int i=0; i<count; i++){
		DebuggerCmdData cmd;
		cmd.SetName      ( GetColumnText(m_listCtrl1, i, 0) );
		cmd.SetCommand   ( GetColumnText(m_listCtrl1, i, 1) );
		cmd.SetDbgCommand( GetColumnText(m_listCtrl1, i, 2) );
		cmdArr.push_back(cmd);
	}
	m_data.SetCmds(cmdArr);

	//save the debugger commands
	DebuggerConfigTool::Get()->WriteObject(wxT("DebuggerCommands"), &m_data);
	EndModal(wxID_OK);
}

void DebuggerSettingsDlg::OnButtonCancel(wxCommandEvent &e)
{
	wxUnusedVar(e);
	EndModal(wxID_CANCEL);
}

void DebuggerSettingsDlg::OnNewShortcut(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DbgCommandDlg *dlg = new DbgCommandDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
		//add new command to the table
		wxString name       = dlg->GetName();
		wxString expression = dlg->GetExpression();
		wxString dbgCmd     = dlg->GetDbgCommand();
		long item;
		wxListItem info;

		//make sure that the expression does not exist
		int count = m_listCtrl1->GetItemCount();
		for(int i=0; i<count; i++){
			wxString existingName = GetColumnText(m_listCtrl1, i, 0);
			if(name == existingName){
				dlg->Destroy();
				wxMessageBox(_("Debugger type with the same name already exist"), wxT("CodeLite"), wxOK | wxICON_INFORMATION);
				return;
			}
		}

		// Set the item display name
		info.SetColumn(0);
		item = m_listCtrl1->InsertItem(info);

		SetColumnText(m_listCtrl1, item, 0, name       );
		SetColumnText(m_listCtrl1, item, 1, expression );
		SetColumnText(m_listCtrl1, item, 2, dbgCmd);

		m_listCtrl1->SetColumnWidth(0, 100);
		m_listCtrl1->SetColumnWidth(1, 200);
		m_listCtrl1->SetColumnWidth(1, 200);
	}
	dlg->Destroy();
}

void DebuggerSettingsDlg::OnItemSelected(wxListEvent &e)
{
	m_selectedItem = e.m_itemIndex;
}

void DebuggerSettingsDlg::OnItemDeselected(wxListEvent &e)
{

	wxUnusedVar(e);
	m_selectedItem = wxNOT_FOUND;
}

void DebuggerSettingsDlg::OnEditShortcut(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DoEditItem();
}

void DebuggerSettingsDlg::OnDeleteShortcut(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DoDeleteItem();
}

void DebuggerSettingsDlg::OnItemActivated(wxListEvent &e)
{
	m_selectedItem = e.m_itemIndex;
	DoEditItem();
}

void DebuggerSettingsDlg::DoEditItem()
{
	//Edit the selection
	if (m_selectedItem == wxNOT_FOUND) {
		return;
	}

	//popup edit dialog
	DbgCommandDlg dlg(this);

	wxString name  = GetColumnText(m_listCtrl1, m_selectedItem, 0);
	wxString expr  = GetColumnText(m_listCtrl1, m_selectedItem, 1);
	wxString dbgCmd= GetColumnText(m_listCtrl1, m_selectedItem, 2);

	dlg.SetName(name);
	dlg.SetExpression(expr);
	dlg.SetDbgCommand(dbgCmd);

	if (dlg.ShowModal() == wxID_OK) {
		SetColumnText(m_listCtrl1, m_selectedItem, 0, dlg.GetName());
		SetColumnText(m_listCtrl1, m_selectedItem, 1, dlg.GetExpression());
		SetColumnText(m_listCtrl1, m_selectedItem, 2, dlg.GetDbgCommand());
	}
}

void DebuggerSettingsDlg::DoDeleteItem()
{
	if (m_selectedItem == wxNOT_FOUND) {
		return;
	}
	m_listCtrl1->DeleteItem(m_selectedItem);
	m_selectedItem = wxNOT_FOUND;
}

DebuggerSettingsDlg::~DebuggerSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("DbgSettingsDlg"), NULL);
}
