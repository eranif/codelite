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
#include "editor_config.h"
#include "debuggermanager.h"
#include "macros.h"
#include "manager.h"
#include "globals.h"
#include "dbgcommanddlg.h"
#include "debuggerconfigtool.h"

//-------------------------------------------------------------------
DebuggerPage::DebuggerPage(wxWindow *parent, wxString title)
		: wxPanel(parent)
		, m_title(title)
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );

	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	sz->Add(fgSizer2, 0, wxEXPAND|wxALL);

	wxStaticText *m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Debugger Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filePicker = new FilePicker( this );
	fgSizer2->Add( m_filePicker, 0, wxALL|wxEXPAND, 5 );

	m_checkBoxEnableLog = new wxCheckBox( this, wxID_ANY, wxT("Enable full debugger logging"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_checkBoxEnableLog, 0, wxEXPAND|wxALL, 5);

	m_checkBoxEnablePendingBreakpoints = new wxCheckBox(this, wxID_ANY, wxT("Enable pending breakpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_checkBoxEnablePendingBreakpoints, 0, wxEXPAND|wxALL, 5);

	m_checkBreakAtWinMain = new wxCheckBox(this, wxID_ANY, wxT("Automatically set breakpoint at main"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_checkBreakAtWinMain, 0, wxEXPAND|wxALL, 5);

	m_checkResolveStarThis = new wxCheckBox(this, wxID_ANY, wxT("Resolve '*this' in the 'Locals' view"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_checkResolveStarThis, 0, wxEXPAND|wxALL, 5);

	m_checkShowTerminal = new wxCheckBox(this, wxID_ANY, wxT("Show debugger terminal"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_checkShowTerminal, 0, wxEXPAND|wxALL, 5);

	m_checkUseRelativePaths = new wxCheckBox(this, wxID_ANY, wxT("Use file name only for breakpoints (NO full paths)"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_checkUseRelativePaths, 0, wxEXPAND|wxALL, 5);

	m_catchThrow = new wxCheckBox(this, wxID_ANY, wxT("Break when C++ execption is thrown"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_catchThrow, 0, wxEXPAND|wxALL, 5);

	m_showTooltips = new wxCheckBox(this, wxID_ANY, wxT("While debugging, show debugger tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add(m_showTooltips, 0, wxEXPAND|wxALL, 5);

	sz->Layout();

	DebuggerInformation info;
	if(DebuggerMgr::Get().GetDebuggerInformation(m_title, info)){
		m_filePicker->SetPath(info.path);
		m_checkBoxEnableLog->SetValue(info.enableDebugLog);
		m_checkBoxEnablePendingBreakpoints->SetValue(info.enablePendingBreakpoints);
		m_checkBreakAtWinMain->SetValue(info.breakAtWinMain);
		m_checkResolveStarThis->SetValue(info.resolveThis);
		m_checkShowTerminal->SetValue(info.showTerminal);
		m_checkUseRelativePaths->SetValue(info.useRelativeFilePaths);
		m_catchThrow->SetValue(info.catchThrow);
		m_showTooltips->SetValue(info.showTooltips);
	}
}

DebuggerPage::~DebuggerPage()
{
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
	m_listCtrl1->InsertColumn(1, wxT("Command"));
	m_listCtrl1->InsertColumn(2, wxT("Sub Menu?"));

	//add items from the saved items
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &m_data);

	//Populate the list with the items from the configuration file
	std::vector<DebuggerCmdData> cmds = m_data.GetCmds();
	for (size_t i=0; i<cmds.size(); i++) {
		DebuggerCmdData cmd = cmds.at(i);
		wxString subMenu(wxT("No"));

		if (cmd.GetIsSubMenu()) {
			subMenu = wxT("Yes");
		}

		long item = AppendListCtrlRow(m_listCtrl1);
		SetColumnText(m_listCtrl1, item, 0, cmd.GetName());
		SetColumnText(m_listCtrl1, item, 1, cmd.GetCommand());
		SetColumnText(m_listCtrl1, item, 2, subMenu);
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
		info.path                     = page->m_filePicker->GetPath();
		info.name                     = page->m_title;
		info.breakAtWinMain           = page->m_checkBreakAtWinMain->IsChecked();
		info.resolveThis              = page->m_checkResolveStarThis->IsChecked();
		info.showTerminal             = page->m_checkShowTerminal->IsChecked();
		info.consoleCommand           = EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand();
		info.useRelativeFilePaths     = page->m_checkUseRelativePaths->IsChecked();
		info.catchThrow               = page->m_catchThrow->IsChecked();
		info.showTooltips             = page->m_showTooltips->IsChecked();

		DebuggerMgr::Get().SetDebuggerInformation(page->m_title, info);
	}

	//copy the commands the serialized object m_data
	int count = m_listCtrl1->GetItemCount();
	std::vector<DebuggerCmdData> cmdArr;
	for(int i=0; i<count; i++){
		DebuggerCmdData cmd;
		cmd.SetName( GetColumnText(m_listCtrl1, i, 0) );
		cmd.SetCommand( GetColumnText(m_listCtrl1, i, 1) );

		cmd.SetIsSubMenu( false );
		if(GetColumnText(m_listCtrl1, i, 2) == wxT("Yes")){
			cmd.SetIsSubMenu( true );
		}

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
		wxString name = dlg->GetName();
		wxString cmd  = dlg->GetCommand();
		wxString subMenu;
		if (dlg->IsSubMenu()) {
			subMenu = wxT("Yes");
		} else {
			subMenu = wxT("No");
		}

		long item;
		wxListItem info;

		//make sure that the expression does not exist
		int count = m_listCtrl1->GetItemCount();
		for(int i=0; i<count; i++){
			wxString existingName = GetColumnText(m_listCtrl1, i, 0);
			if(name == existingName){
				dlg->Destroy();
				wxString msg;
				wxMessageBox(_("Debugger shortcut with the same name already exist"), wxT("CodeLite"), wxOK | wxICON_INFORMATION);
				return;
			}
		}

		// Set the item display name
		info.SetColumn(0);
		item = m_listCtrl1->InsertItem(info);

		SetColumnText(m_listCtrl1, item, 0, name);
		SetColumnText(m_listCtrl1, item, 1, cmd);
		SetColumnText(m_listCtrl1, item, 2, subMenu);

		m_listCtrl1->SetColumnWidth(0, 100);
		m_listCtrl1->SetColumnWidth(1, 400);
		m_listCtrl1->SetColumnWidth(2, 100);
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
	DbgCommandDlg *dlg = new DbgCommandDlg(this);
	wxString name = GetColumnText(m_listCtrl1, m_selectedItem, 0);
	wxString cmd  = GetColumnText(m_listCtrl1, m_selectedItem, 1);
	wxString sm   = GetColumnText(m_listCtrl1, m_selectedItem, 2);

	bool subMenu(false);
	if (sm == wxT("Yes")) {
		subMenu = true;
	}

	dlg->SetName(name);
	dlg->SetCommand(cmd);
	dlg->SetAsSubMenu(subMenu);

	if (dlg->ShowModal() == wxID_OK) {
		//update the item
		sm = wxT("No");
		if (dlg->IsSubMenu()) {
			sm = wxT("Yes");
		}

		SetColumnText(m_listCtrl1, m_selectedItem, 0, dlg->GetName());
		SetColumnText(m_listCtrl1, m_selectedItem, 1, dlg->GetCommand());
		SetColumnText(m_listCtrl1, m_selectedItem, 2, sm);
	}

	dlg->Destroy();
}

void DebuggerSettingsDlg::DoDeleteItem()
{
	if (m_selectedItem == wxNOT_FOUND) {
		return;
	}
	m_listCtrl1->DeleteItem(m_selectedItem);
	m_selectedItem = wxNOT_FOUND;
}
