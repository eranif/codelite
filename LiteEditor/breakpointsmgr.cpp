//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : breakpointsmgr.cpp
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
#include "wx/numdlg.h"
#include "breakpointsmgr.h"
#include "debuggermanager.h"
#include "manager.h"
#include "frame.h"
#include "breakpointpropertiesdlg.h"
#include "breakpointdlg.h"

//---------------------------------------------------------

bool BreakptMgr::AddBreakpointByLineno(const wxString& file, const int lineno, const wxString& conditions/*=wxT("")*/, const bool is_temp/*=false*/)
{
	BreakpointInfo bp;
	bp.Create(file, lineno, GetNextID());
	if (is_temp) {
		bp.bp_type = BP_type_tempbreak;
		bp.is_temp = true;
	}
	bp.conditions = conditions;

	return AddBreakpoint(bp);
}

bool BreakptMgr::AddBreakpoint(BreakpointInfo &bp)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// If the debugger is already running, tell it we want a new bp
		// If not, they'll all be added together when the debugger does start
		bool contIsNeeded = PauseDebuggerIfNeeded();
		dbgr->Break(bp);
		// dbgr->Break(bp) doesn't set the ignore/disabled/etc states
		// but we can't do it now, as we don't yet know the debugger_id
		// However it will happen later, in SetBreakpointDebuggerID
		if (contIsNeeded) {
			dbgr->Continue();
		}
	}

	SetBestBPType(bp);
	m_bps.push_back(bp);

	RefreshBreakpointMarkers();
	return true;
}

// Add a breakpoint using the 'Properties' dialog
void BreakptMgr::AddBreakpoint()
{
	BreakptPropertiesDlg dlg(NULL);
	dlg.SetTitle(_("Create a breakpoint or watchpoint"));

	BreakpointInfo bp;
	bp.Create(ManagerST::Get()->GetActiveEditor()->GetFileName().GetFullPath(), -1, GetNextID());
	dlg.EnterBPData(bp);

	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	if (AddBreakpoint(dlg.b)) {
		wxString msg;
		if (dlg.b.bp_type == BP_type_watchpt) {
			msg = _("Watchpoint successfully added");
		} else {
			msg = _("Breakpoint successfully added");
		}
		Frame::Get()->SetStatusMessage(msg, 0);
	}
}

void BreakptMgr::GetBreakpoints(std::vector<BreakpointInfo> &li)
{
	li = m_bps;
}

// Get all known breakpoints for this line/file
unsigned int BreakptMgr::GetBreakpoints(std::vector<BreakpointInfo>& li, const wxString &fileName, const int lineno)
{
	li.clear();
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for (; iter != m_bps.end(); iter++) {
		BreakpointInfo b = *iter;
		if ( (b.lineno == lineno) && (b.file == fileName) ) {
			li.push_back(b);
		}
	}
	return li.size();
}

// Try to find a breakpoint on this line/file to match the data
// Returns whether >0 was found, with matches stored in the vector
bool BreakptMgr::GetMatchingBreakpoints(std::vector<BreakpointInfo>& li, const wxString &fileName, const int lineno, enum BP_type bp_type)
{
	std::vector<BreakpointInfo> allonline;	// Start by finding all on the line
	if ( ! GetBreakpoints(allonline, fileName, lineno) ) {
		return false;
	}

	li.clear();
	std::vector<BreakpointInfo>::iterator iter = allonline.begin();
	for (; iter != allonline.end(); ++iter) {	// Now add the matching ones to li
		BreakpointInfo b = *iter;
		if (b.bp_type == bp_type) {
			li.push_back(b);
		}
	}
	return ! li.empty();
}

// Delete all line-type breakpoint markers in all editors
// Done before refreshing after a delete or edit, lest it was the last bp in a file
void BreakptMgr::DeleteAllBreakpointMarkers()
{
	std::set<wxString> filenames = GetFilesWithBreakpointMarkers();
	std::set<wxString>::iterator filenames_iter = filenames.begin();
	for (; filenames_iter != filenames.end(); ++filenames_iter) {
		LEditor* editor = Frame::Get()->GetMainBook()->FindEditor(*filenames_iter);
		if (editor) {
			editor->DelAllBreakpointMarkers();
		}
	}
}
// Refresh all line-type breakpoint markers in all editors
void BreakptMgr::RefreshBreakpointMarkers()
{
	std::set<wxString> filenames = GetFilesWithBreakpointMarkers();
	std::set<wxString>::iterator filenames_iter = filenames.begin();
	for (; filenames_iter != filenames.end(); ++filenames_iter) {
		LEditor* editor = Frame::Get()->GetMainBook()->FindEditor(*filenames_iter);
		if (editor) {
			DoRefreshFileBreakpoints(editor);
		}
	}
}

// Delete all breakpoint markers for this file, then re-mark with the currently-correct marker
void BreakptMgr::DoRefreshFileBreakpoints(LEditor* editor)
{
	// Load the file's line-type bps into fi, and make a set of their line-numbers
	std::multimap<int, BreakpointInfo> bps;
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for (; iter != m_bps.end(); ++iter) {
		BreakpointInfo b = *iter;
		if ((b.file == editor->GetFileName().GetFullPath()) && (b.lineno != -1)) {
			bps.insert(std::pair<int, BreakpointInfo>(b.lineno, b));
		}
	}

	editor->DelAllBreakpointMarkers();

	// the multimap now holds a table of
	// line numbers and breakpointsInfo, collect all line numbers's breakpoint info into vector
	// and place markers
	for(std::multimap<int, BreakpointInfo>::iterator i=bps.begin(); i != bps.end(); i++){
		std::pair<std::multimap<int, BreakpointInfo>::iterator, std::multimap<int, BreakpointInfo>::iterator> range = bps.equal_range(i->first);
		std::vector<BreakpointInfo> v;
		for(std::multimap<int, BreakpointInfo>::iterator it = range.first; it != range.second; it++){
			v.push_back(it->second);

			// Now work out which is the most significant (in marker terms) and tell the editor
			DoProvideBestBP_Type(editor, v);
		}
	}
}

// Given a list of bps, tell the editor which is the most significant (in marker terms)
void BreakptMgr::DoProvideBestBP_Type(LEditor* editor, const std::vector<BreakpointInfo>& li)
{
	if ((editor == NULL) || (li.size() == 0)) {
		return;
	}
	// If there's an enabled bp of any sort, it beats all disabled ones
	// Otherwise, BP_type_break > BP_type_tempbreak > BP_type_cmdlistbreak > BP_type_condbreak > BP_type_ignoredbreak
	int values[BP_LAST_MARKED_ITEM+2]; // Allow for BP_type_invalid = -1 & BP_type_none = 0
	values[BP_type_break] = 100;
	values[BP_type_tempbreak] = 90;
	values[BP_type_cmdlistbreak] = 80;
	values[BP_type_condbreak] = 70;
	values[BP_type_ignoredbreak] = 60;
	values[BP_type_none] = 0;
	values[BP_type_invalid] = -1;

	BP_type best = BP_type_none;
	int best_value = 0;
	bool is_disabled = false;
	std::vector<BreakpointInfo>::const_iterator iter = li.begin();
	for (; iter != li.end(); ++iter) {
		BP_type bpt = iter->bp_type;
		int val = values[bpt];
		if (!iter->is_enabled) {
			val /= 2;	// Halving the value for disability means that abled always outranks disabled, without otherwise interfering with the order
		}
		if (val > best_value) {
			best = bpt;
			best_value = val;
			is_disabled = ! iter->is_enabled;	// If the new item wins, store its data
		}
	}


	if (best > 0) {
		// ATTN: this wierd allocation is due to bug in optimization of g++
		// which seems to crash (removing optimization fixes this problem,
		// but we prefer to stick with optimization level 2)
		std::vector<BreakpointInfo> *v = new std::vector<BreakpointInfo>(li);
		editor->SetBreakpointMarker(li.at(0).lineno, best, is_disabled, *v);
		delete v;
	}
}

// Invalidates each debugger_id
void BreakptMgr::ClearBP_debugger_ids()
{
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for (; iter != m_bps.end(); iter++) {
		BreakpointInfo bp = *iter;
		bp.debugger_id = -1;
	}
}

bool BreakptMgr::DelBreakpoint(const int id)
{
	int index = FindBreakpointById(id);
	if (index == wxNOT_FOUND) {
		return false;
	}

	//remove it from the debugger if it's running
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		if (id > FIRST_INTERNAL_ID) {
			// This shouldn't happen while the debugger is running (debugger_id should be valid)
			// But if it does, assume it was a bp that gdb couldn't create, and just remove from the bp list
		} else {
			bool contIsNeeded = PauseDebuggerIfNeeded();
			dbgr->RemoveBreak(id);
			if (contIsNeeded) {
				dbgr->Continue();
			}
		}
	}

	// Delete all markers before removing bp from the vector. Otherwise if id was the last in a file...
	DeleteAllBreakpointMarkers();

	m_bps.erase(m_bps.begin()+index);

	RefreshBreakpointMarkers();
	return true;
}

int BreakptMgr::DelBreakpointByLineno(const wxString& file, const int lineno)
{
	wxString msg(_("Select the breakpoint to be deleted"));
	int bid = GetDesiredBreakpointIfMultiple(file, lineno, msg);
	if (bid == wxID_CANCEL || bid == BP_type_none) {
		return bid;
	}

	return (DelBreakpoint(bid) == true ? true : -1); // Use -1 as an arbitrary failed-to-delete flag
}

void BreakptMgr::DelAllBreakpoints()
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr) {
		dbgr->RemoveAllBreaks();
	}

	// Delete all markers before clearing m_bps, otherwise we won't know which files they were in
	DeleteAllBreakpointMarkers();
	m_bps.clear();
}

// The debugger labels each breakpoint with an id. DbgGdb::Poke grabs this
// and passes it here via DbgCmdHandlerBp::ProcessOutput and Manager::UpdateBpAdded.
// By storing it as BreakpointInfo::debugger_id, we can be certain that this bp refers to the gdb breakpoint
void BreakptMgr::SetBreakpointDebuggerID(const int internal_id, const int debugger_id)
{
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for (; iter != m_bps.end(); ++iter) {
		if (iter->internal_id == internal_id) {
			// We've found the match. However, if breakpoint creation had failed, confess, then delete
			if (debugger_id == -1) {
				wxString msg;
				if (iter->bp_type == BP_type_watchpt) {
					msg = _("Watchpoint creation unsuccessful");
				} else {
					msg = _("Breakpoint creation unsuccessful");
				}

				// add message to the debugger tab
				ManagerST::Get()->UpdateAddLine(msg);
				m_bps.erase(iter);

				// update the UI as well
				Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

				return;
			}
			// Otherwise store the valid debugger_id
			iter->debugger_id = debugger_id;

			// update the UI as well
			Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
			return;
		}
	}
	wxLogMessage(wxT("SetBreakpointDebuggerID(): Failed to match internal_id to debugger_id"));
}

void BreakptMgr::EditBreakpointByLineno(const wxString& file, const int lineno)
{
	wxString msg(_("Select the breakpoint to edit"));
	int bid = GetDesiredBreakpointIfMultiple(file, lineno, msg);
	if (bid == wxID_CANCEL || bid == BP_type_none) {
		return;
	}

	int index = FindBreakpointById(bid);
	if (index == wxNOT_FOUND) {
		return;
	}

	bool dummy;
	EditBreakpoint(index, dummy);
}

void BreakptMgr::EditBreakpoint(int index, bool &bpExist)
{
	// sanity
	bpExist = true;
	if (index < 0 || index >= (int)m_bps.size()) {
		bpExist = false;
		return;
	}

	BreakpointInfo bp = m_bps.at(index);
	BreakptPropertiesDlg dlg(NULL);
	wxString title;
	if (bp.bp_type == BP_type_watchpt) {
		title = _("Properties for watchpoint ");
	} else {
		title = _("Properties for breakpoint ");
	}
	int id = bp.debugger_id;
	if (id == -1) {
		id = bp.internal_id - FIRST_INTERNAL_ID;
	}
	title << id;
	dlg.SetTitle(title);

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {

		dlg.EnterBPData(bp);
		if (dlg.ShowModal() != wxID_OK) {
			return ;
		}

		SetBestBPType(dlg.b);	// The edited data's available. Use it to determine the best bp_type
		if (bp == dlg.b) {
			return ;
		}

		// If it can't be updated (because gdb wouldn't be able to cope with the change), replace
		bool contIsNeeded = PauseDebuggerIfNeeded();
		dbgr->RemoveBreak(bp.debugger_id);
		dbgr->Break(dlg.b);

		// dbgr->Break(bp) doesn't set the ignore/disabled/etc states
		// but we can't do it now, as we don't yet know the debugger_id
		// However it will happen later, in SetBreakpointDebuggerID
		if (contIsNeeded) {
			dbgr->Continue();
		}
	}

	// Replace the old data with the new, in m_bps
	m_bps.at(index) = dlg.b;
	DeleteAllBreakpointMarkers();
	RefreshBreakpointMarkers();
}

/*------------------------------- Implementation -------------------------------*/

// Tell the debugger about the ignore count, conditions etc
void BreakptMgr::DoBreakpointExtras(BreakpointInfo &bp)
{
//	if (bp.ignore_number) {
//		SetBPIgnoreCount(bp.debugger_id, bp.ignore_number);
//	}
//	if (! bp.is_enabled) {
//		SetBPEnabledState(bp.debugger_id, bp.is_enabled);
//	}
//	if (! bp.conditions.IsEmpty()) {
//		SetBPConditon(bp);
//	}
//	if (! bp.commandlist.IsEmpty()) {
//		SetBPCommands(bp);
//	}
}

// Get a breakpoint on this line. If multiple bps, ask the user to select
int BreakptMgr::GetDesiredBreakpointIfMultiple(const wxString &fileName, const int lineno, const wxString msg /*=wxT("")*/)
{
	std::vector<BreakpointInfo> allonline;	// Start by finding all on the line
	if ( ! GetBreakpoints(allonline, fileName, lineno) ) {
		return BP_type_none;
	}

	if (allonline.size() == 1) {
		BreakpointInfo bp = allonline[0];
		return (bp.debugger_id == -1 ? bp.internal_id : bp.debugger_id );
	}

	wxString message(msg);
	if (message.IsEmpty()) {
		message = _("Please select an item");
	}
	wxDialog dlg(NULL, wxID_ANY, message,wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	BreakpointsListctrl* list = new BreakpointsListctrl(&dlg);
	list->Initialise(allonline);
	sizer->Add( list, 1, wxEXPAND | wxALL, 20 );
	wxStdDialogButtonSizer* btnsizer = new wxStdDialogButtonSizer();
	btnsizer->AddButton( new wxButton(&dlg, wxID_OK) );
	btnsizer->AddButton( new wxButton(&dlg, wxID_CANCEL) );
	btnsizer->Realize();
	sizer->Add( btnsizer, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 25 );
	dlg.SetSizer(sizer);
	sizer->Layout();
	if (dlg.ShowModal() != wxID_OK) {
		return wxID_CANCEL;
	}

	int selection = list->GetSelection();
	if (selection == wxNOT_FOUND) {
		return wxID_CANCEL;
	}

	BreakpointInfo bp = allonline[selection];
	return (bp.debugger_id == -1 ? bp.internal_id : bp.debugger_id );
}

// Construct set of files containing bps
std::set<wxString> BreakptMgr::GetFilesWithBreakpointMarkers()
{
	// Make a set of all filenames containing a file-relevant variety of breakpoint
	std::set<wxString> filenames;
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for (; iter != m_bps.end(); ++iter) {
		// If this bp is a lineno or function type, add its file to the set
		wxString fileName = iter->file;
		if ((!fileName.IsEmpty()) && (iter->memory_address == -1)) {
			filenames.insert(fileName);
		}
	}

	return filenames;
}

int BreakptMgr::FindBreakpointById(const int id)
{
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for (; iter != m_bps.end(); ++iter) {
		BreakpointInfo b = *iter;
		// If we were passed an id > 10000, it must be internal
		int this_id = (id > FIRST_INTERNAL_ID ? b.internal_id : b.debugger_id);
		if (id == this_id) {
			return (int)(iter - m_bps.begin());
		}
	}

	return wxNOT_FOUND;
}

bool BreakptMgr::CanThisBreakpointBeUpdated(const BreakpointInfo &a, const BreakpointInfo &b) const
{
	// Since, afaik, the only things that *can* be updated are: conditions/commands/ignores/enables,
	// return true if all but these are == (except for bp_type, which will have changed as a consequence of those)
	BreakpointInfo temp = a;
	temp.ignore_number = b.ignore_number;
	temp.is_enabled = b.is_enabled;
	temp.conditions = b.conditions;
	temp.commandlist = b.commandlist;
	temp.bp_type = b.bp_type;
	return (temp == b);
}

void BreakptMgr::SetBestBPType(BreakpointInfo& bp)
{
	if (bp.bp_type == BP_type_watchpt) {
		return;
	}

	if (bp.ignore_number > 0) {
		bp.bp_type = BP_type_ignoredbreak;
		return;
	}

	if (bp.IsConditional()) {
		bp.bp_type = BP_type_condbreak;
		return;
	}

	if (!bp.commandlist.IsEmpty()) {
		bp.bp_type = BP_type_cmdlistbreak;
		return;
	}

	if (bp.is_temp) {
		bp.bp_type = BP_type_tempbreak;
		return;
	}

	bp.bp_type = BP_type_break;	// Default option
}

// If the debugger is running but can't interact, pause it and return true (to flag needs restart)
bool BreakptMgr::PauseDebuggerIfNeeded()
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning() && !ManagerST::Get()->DbgCanInteract()) {
		dbgr->Interrupt();
		return true;
	}
	return false;
}

void BreakptMgr::DeleteAllBreakpointsByFileName(const wxString& fileName)
{
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	while (iter != m_bps.end()) {
		if (fileName == iter->file) {
			iter = m_bps.erase(iter);
		} else {
			++iter;
		}
	}
}

void BreakptMgr::SetBreakpoints(const std::vector<BreakpointInfo>& bps)
{
	// append the breakpoint list
	m_bps.insert(m_bps.end(), bps.begin(), bps.end());
}
