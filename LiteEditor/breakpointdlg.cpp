//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : breakpointdlg.cpp
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
 #include "breakpointdlg.h"
#include "debuggermanager.h"
#include "manager.h"
#include "frame.h"
#include "macros.h"
#include "globals.h"

BreakpointDlg::BreakpointDlg( wxWindow* parent )
		: BreakpointTab( parent )
		, m_selectedItem(wxNOT_FOUND)
{
	Initialize();
	ConnectButton(m_buttonDelete, BreakpointDlg::OnDelete);
	ConnectButton(m_buttonDeleteAll, BreakpointDlg::OnDeleteAll);
}

void BreakpointDlg::Initialize()
{
	std::vector<BreakpointInfo> bps;
	ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(bps);

	// This does the display stuff
	m_listCtrlBreakpoints->Initialise(bps);

	// Store the internal and external ids
	m_ids.clear();
	std::vector<BreakpointInfo>::iterator iter = bps.begin();
	for(; iter != bps.end(); ++iter) {
		struct bpd_IDs IDs(*iter);
		m_ids.push_back(IDs);
	}

	int count = m_listCtrlBreakpoints->GetItemCount();
	bool hasitems = count > 0;
	if (hasitems) {
		// Select the first item if there's not already a selection
		if (m_selectedItem == wxNOT_FOUND) {
			m_selectedItem = 0;
		}
		if (m_selectedItem >= count) {	// e.g. if the selection was the last item, then one is deleted
			m_selectedItem = count-1;
		}
		// Even if an item was previously selected, refreshing the pane means we need to reselect
		m_listCtrlBreakpoints->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}

	// Since any change results in Initialize() being rerun, we can do updateUI here
	m_buttonEdit->Enable(hasitems);
	m_buttonDelete->Enable(hasitems);
	m_buttonDeleteAll->Enable(hasitems);
}

void BreakpointDlg::OnDelete(wxCommandEvent &e)
{
	if (m_selectedItem != wxNOT_FOUND) {
	// Delete by this item's id, which was carefully stored in Initialize()
		int id = m_ids[m_selectedItem].GetBestId();
		ManagerST::Get()->GetBreakpointsMgr()->DelBreakpoint(id);
		m_selectedItem = wxNOT_FOUND;

		Frame::Get()->SetStatusMessage(wxT("Breakpoint successfully deleted"), 0);
	}

	Initialize();	// ReInitialise, as either a bp was deleted, or the data was corrupt
}

void BreakpointDlg::OnDeleteAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints();
	m_selectedItem = wxNOT_FOUND;
	Initialize();

	Frame::Get()->SetStatusMessage(wxT("All Breakpoints deleted"), 0);
}

void BreakpointDlg::OnItemSelected(wxListEvent &e)
{
	m_selectedItem = e.m_itemIndex;
}

void BreakpointDlg::OnItemDeselected(wxListEvent &e)
{
	wxUnusedVar(e);
	m_selectedItem = wxNOT_FOUND;
}

void BreakpointDlg::OnItemActivated(wxListEvent &e)
{
	wxString file = GetColumnText(m_listCtrlBreakpoints, e.m_itemIndex, 0);
	wxString line = GetColumnText(m_listCtrlBreakpoints, e.m_itemIndex, 1);
	long line_number;
	line.ToLong(&line_number);

	ManagerST::Get()->OpenFile(file, wxEmptyString, line_number-1);
}

void BreakpointDlg::OnItemRightClick(wxListEvent& e)
{
	wxCommandEvent c;
	OnEdit(c);
}

void BreakpointDlg::OnEdit(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (m_selectedItem == wxNOT_FOUND) {
		return;
	}

	ManagerST::Get()->GetBreakpointsMgr()->EditBreakpoint(m_selectedItem);
	Initialize();	// Make any changes visible
}

void BreakpointDlg::OnAdd(wxCommandEvent& e)
{
	wxUnusedVar(e);

	ManagerST::Get()->GetBreakpointsMgr()->AddBreakpoint();
	Initialize();	// Make any changes visible
}

void BreakpointsListctrl::Initialise(std::vector<BreakpointInfo>& bps)
{
	ClearAll();
	Freeze();

	InsertColumn(col_id, wxT("ID"));
	InsertColumn(col_type, wxT("Breakpoint Type"));
	InsertColumn(col_enabled, wxT("Enabled"));
	InsertColumn(col_file, wxT("File"));
	InsertColumn(col_lineno, wxT("Line"));
	InsertColumn(col_functionname, wxT("Function"));
	InsertColumn(col_memory, wxT("Memory"));
	InsertColumn(col_ignorecount, wxT("Ignored"));
	InsertColumn(col_extras, wxT("Extras"));

	// Reverse through the items, as AppendListCtrlRow() inserts at 0, and the bps look silly with their id's reverse-sorted
	std::vector<BreakpointInfo>::reverse_iterator iter = bps.rbegin();
	for (; iter != bps.rend(); ++iter) {
		long item = AppendListCtrlRow(this);

		// Store the internal and external ids
		struct bpd_IDs IDs(*iter);
		SetColumnText(this, item, col_id, IDs.GetIdAsString(), wxNOT_FOUND);

		wxString type; type = (iter->bp_type==BP_type_watchpt) ? wxT("Watchpoint") : wxT("Breakpoint");
		SetColumnText(this, item, col_type, type, wxNOT_FOUND);

		wxString disabled;
		if (!iter->is_enabled) {
			disabled = wxT("disabled");
		}
		SetColumnText(this, item, col_enabled, disabled, wxNOT_FOUND);

		// A breakpoint will have either a file/lineno or a function-name (e.g.main(), or a memory address)
		// A watchpoint will have watchpt_data (the variable it's watching). Display that in the 'Function' col
		if ((iter->bp_type==BP_type_watchpt) && (!iter->watchpt_data.IsEmpty())) {
			SetColumnText(this, item, col_functionname, iter->watchpt_data, wxNOT_FOUND);
		} else if (!iter->function_name.IsEmpty()) {
			SetColumnText(this, item, col_functionname, iter->function_name, wxNOT_FOUND);
		} else if (iter->memory_address != -1) {
			wxString addr; addr << iter->memory_address;
			SetColumnText(this, item, col_memory, addr, wxNOT_FOUND);
		} else {
			SetColumnText(this, item, col_file, iter->file, wxNOT_FOUND);
			wxString line; line << iter->lineno;
			SetColumnText(this, item, col_lineno, line, wxNOT_FOUND);
		}

		wxString ignore;
		if (iter->ignore_number) {
			ignore << iter->ignore_number;
		}
		SetColumnText(this, item, col_ignorecount, ignore, wxNOT_FOUND);

		wxString extras;	// Extras are conditions, or a commandlist. If both (unlikely!) just show the condition
		if (!iter->conditions.IsEmpty()) {
			extras = iter->conditions;
		} else if (!iter->commandlist.IsEmpty()) {
				extras = iter->commandlist;
		}
		if (!extras.IsEmpty()) {
			// We don't want to try to display massive commandlist spread over several lines...
			int index = extras.Find(wxT("\\n"));
			if (index != wxNOT_FOUND) {
				extras = extras.Left(index) + wxT("...");
			}
			SetColumnText(this, item, col_extras, extras, wxNOT_FOUND);
		}
	}

	for (int col=0; col < GetColumnCount(); ++col) {
		// If there are no items, size the columns to fit the header labels: it looks ugly otherwise
		if (GetItemCount() == 0) {
			SetColumnWidth(col, wxLIST_AUTOSIZE_USEHEADER);
			continue;
		}
		// Otherwise, use the maximum of the itemsize and headersize
		SetColumnWidth(col, 100);
	}

	Thaw();
}

int BreakpointsListctrl::GetSelection()
{
	long selecteditem = -1;
	selecteditem = GetNextItem(selecteditem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	return (int)selecteditem;
}
