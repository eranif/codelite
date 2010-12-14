//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : attachdbgprocdlg.cpp
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
#include "attachdbgprocdlg.h"
#include "windowattrmanager.h"
#include "debuggermanager.h"
#include "globals.h"
#include "procutils.h"
#include <algorithm>

bool AttachDbgProcDlg::ms_useSudo = false;

/// Ascending sorting function
struct PIDSorter {
	bool operator()(const ProcessEntry &rStart, const ProcessEntry &rEnd) {
		return rEnd.pid < rStart.pid;
	}
};

struct NameSorter {
	bool operator()(const ProcessEntry &rStart, const ProcessEntry &rEnd) {
		return rEnd.name.CmpNoCase(rStart.name) < 0;
	}
};

AttachDbgProcDlg::AttachDbgProcDlg( wxWindow* parent )
	: AttachDbgProcBaseDlg( parent )
	, m_selectedItem(wxNOT_FOUND)
{
	wxArrayString choices = DebuggerMgr::Get().GetAvailableDebuggers();
	m_choiceDebugger->Append(choices);

	if (choices.IsEmpty() == false) {
		m_choiceDebugger->SetSelection(0);
	}

	m_listCtrlProcesses->InsertColumn(0, _("PID"));
	m_listCtrlProcesses->InsertColumn(1, _("Name"));
	
	m_checkBoxUseSudo->SetValue(ms_useSudo);
	
	RefreshProcessesList(wxEmptyString);
	m_textCtrlFilter->SetFocus();
	Centre();

	WindowAttrManager::Load(this, wxT("AttachDbgProcDlg"), NULL);
}

void AttachDbgProcDlg::RefreshProcessesList(wxString filter, int colToSort)
{
	m_listCtrlProcesses->Freeze();
	m_listCtrlProcesses->DeleteAllItems();

	filter.Trim().Trim(false);

	//Populate the list with list of processes
	std::vector<ProcessEntry> proclist;
	ProcUtils::GetProcessList(proclist);

	if (colToSort == 0) {//sort by PID
		std::sort(proclist.begin(), proclist.end(), PIDSorter());

	} else if (colToSort == 1) {//sort by name
		std::sort(proclist.begin(), proclist.end(), NameSorter());

	}

	filter.MakeLower();
	for (size_t i=0; i<proclist.size(); i++) {

		// Use case in-sensitive match for the filter
		wxString entryName (proclist.at(i).name);
		entryName.MakeLower();

		// Append only processes that matches the filter string
		if ( filter.IsEmpty() || entryName.Contains(filter) ) {
			long item = AppendListCtrlRow(m_listCtrlProcesses);
			ProcessEntry entry = proclist.at(i);
			wxString spid;
			bool selfPid = (entry.pid == (long)wxGetProcessId());
			spid << entry.pid;
			SetColumnText(m_listCtrlProcesses, item, 0, spid);
			SetColumnText(m_listCtrlProcesses, item, 1, entry.name);

			if (selfPid) {
				m_listCtrlProcesses->SetItemTextColour(item, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
			}
		}

	}

	m_listCtrlProcesses->SetColumnWidth(0, 100);
	m_listCtrlProcesses->SetColumnWidth(1, 500);
	m_listCtrlProcesses->Thaw();
}

wxString AttachDbgProcDlg::GetExeName() const
{
	if (m_selectedItem != wxNOT_FOUND) {
		return GetColumnText(m_listCtrlProcesses, m_selectedItem, 1);
	}
	return wxEmptyString;
}

wxString AttachDbgProcDlg::GetProcessId() const
{
	if (m_selectedItem != wxNOT_FOUND) {
		return GetColumnText(m_listCtrlProcesses, m_selectedItem, 0);
	}
	return wxEmptyString;
}


void AttachDbgProcDlg::OnSortColumn( wxListEvent& event )
{
	RefreshProcessesList( m_textCtrlFilter->GetValue(), event.m_col );
}

void AttachDbgProcDlg::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	EndModal(wxID_OK);
}

void AttachDbgProcDlg::OnItemDeselected( wxListEvent& event )
{
	m_selectedItem = wxNOT_FOUND;
	wxUnusedVar(event);
}

void AttachDbgProcDlg::OnItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	wxUnusedVar(event);
}

void AttachDbgProcDlg::OnBtnAttachUI( wxUpdateUIEvent& event )
{
	event.Enable(m_selectedItem != wxNOT_FOUND);
}

AttachDbgProcDlg::~AttachDbgProcDlg()
{
	ms_useSudo = m_checkBoxUseSudo->IsChecked();
	WindowAttrManager::Save(this, wxT("AttachDbgProcDlg"), NULL);
}

void AttachDbgProcDlg::OnFilter(wxCommandEvent& event)
{
	wxUnusedVar(event);
	RefreshProcessesList(m_textCtrlFilter->GetValue());
}

void AttachDbgProcDlg::OnRefresh(wxCommandEvent& event)
{
	wxUnusedVar(event);
	// Clear all filters and refresh the processes list
	m_textCtrlFilter->Clear();
	RefreshProcessesList(wxEmptyString);
	m_textCtrlFilter->SetFocus();
}

void AttachDbgProcDlg::OnSudoCommandUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxUseSudo->IsChecked());
}

void AttachDbgProcDlg::OnAttachAsAnotherUserUI(wxUpdateUIEvent& event)
{
#if 1
#ifdef __WXMSW__
	event.Check(false);
	event.Enable(false);
#else
	event.Enable(true);
#endif
#endif
}
