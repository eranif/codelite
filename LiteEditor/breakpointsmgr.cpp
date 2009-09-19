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
#include "wx/regex.h"
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

bool BreakptMgr::AddBreakpoint(const BreakpointInfo &bp)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// If the debugger is already running, tell it we want a new bp
		// If not, they'll all be added together when the debugger does start
		bool contIsNeeded = PauseDebuggerIfNeeded();

		dbgr->Break(bp);

		if (contIsNeeded) {
			dbgr->Continue();
		}
	}

	BreakpointInfo newBreakpoint(bp);
	SetBestBPType(newBreakpoint);

	bool alreadyExist(false);
	for(size_t i=0; i<m_bps.size(); i++) {
		if(newBreakpoint == m_bps.at(i)) {
			// this breakpoint already exist
			alreadyExist = true;
			break;
		}
	}

	if( !alreadyExist ) m_bps.push_back(newBreakpoint);

	DeleteAllBreakpointMarkers();
	RefreshBreakpointMarkers();
	return true;
}

// Add a breakpoint using the 'Properties' dialog
void BreakptMgr::AddBreakpoint()
{
	BreakptPropertiesDlg dlg(NULL);
	dlg.SetTitle(_("Create a breakpoint or watchpoint"));

	LEditor* const editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	BreakpointInfo bp;
	bp.Create(editor ? editor->GetFileName().GetFullPath() : wxString(), editor ? editor->GetCurrentLine() : -1, GetNextID());
	dlg.EnterBPData(bp);

	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	if (AddBreakpoint(dlg.b)) {
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if ((!dlg.b.is_enabled) && dbgr && dbgr->IsRunning()) {
			SetBPEnabledState(dlg.b.debugger_id, dlg.b.is_enabled);
		}
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

wxString BreakptMgr::GetTooltip(const wxString& fileName, const int lineno)
{
	if (fileName.IsEmpty() || lineno < 0) {
		return wxEmptyString;
	}

	std::vector<BreakpointInfo> li;
	GetBreakpoints(li, fileName, lineno);

	wxString tooltip;
	std::vector<BreakpointInfo>::iterator iter = li.begin();
	for (; iter != li.end(); ++iter) {
		if (! tooltip.IsEmpty()) {
			tooltip << wxT("\n");
		}
		if (iter->is_temp) {
			tooltip << _("Temporary ");
		}
		int id = (iter->debugger_id > 0 ? iter->debugger_id : iter->internal_id - FIRST_INTERNAL_ID);
		tooltip << wxString::Format(_("Breakpoint %d"), id);
		if (! iter->is_enabled) {
			tooltip << _(" (disabled)");
		}
		if (iter->ignore_number > 0) {
			tooltip << wxString::Format(_(", ignore-count = %u"), iter->ignore_number);
		}

		if (! iter->conditions.IsEmpty()) {
			tooltip << wxString::Format(_(". Condition: %s"), iter->conditions.c_str());
		}

		if (! iter->commandlist.IsEmpty()) {
			tooltip << wxString::Format(_(". Commands: %s"), iter->commandlist.c_str());
		}
	}

	return tooltip;
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
		if ((editor->GetFileName() == b.file) && (b.lineno != -1)) {
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
		int count=0;
		for(std::multimap<int, BreakpointInfo>::iterator it = range.first; it != range.second; it++){
			v.push_back(it->second);
			++count;
		}
		// Inc over the rest of the range, otherwise there'll be duplication when there's >1 bp on a line
		while (--count > 0) {
			++i;
		}
		// Now work out which is the most significant (in marker terms) and tell the editor
		DoProvideBestBP_Type(editor, v);
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
		*iter = bp;
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
			if (dbgr->RemoveBreak(id)) {
				// Strangely, -break-delete doesn't output any confirmation except for ^done. So do it here
				wxString msg = ((m_bps.at(index).bp_type == BP_type_watchpt) ? wxT("Watchpoint ") : wxT("Breakpoint "));
				ManagerST::Get()->UpdateAddLine(msg + wxString::Format(_("%u deleted"), id));
			}
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

// Toggle a breakpoint's enabled state
bool BreakptMgr::ToggleEnabledStateByLineno(const wxString& file, const int lineno)
{
	wxString msg(_("Select the breakpoint that you want to alter"));
	int bid = GetDesiredBreakpointIfMultiple(file, lineno, msg);
	if (bid == wxID_CANCEL || bid == BP_type_none) {
		return false;
	}

	int index = FindBreakpointById(bid);

	// sanity
	if (index < 0 || index >= (int)m_bps.size()) {
		wxLogMessage(wxT("ToggleEnabledStateByLineno(): Insane index"));
		return false;
	}

	if (! SetBPEnabledState(bid, !m_bps.at(index).is_enabled)) {
		return false;
	}

	m_bps.at(index).is_enabled = ! m_bps.at(index).is_enabled;
	RefreshBreakpointMarkers();
	return true;
}

// The debugger labels each breakpoint with an id and passes it here via Manager::UpdateBpAdded.
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

// Set a breakpoint's ignore count
bool BreakptMgr::IgnoreByLineno(const wxString& file, const int lineno)
{
	wxString msg(_("Select the breakpoint to have its ignore-count changed"));
	int bid = GetDesiredBreakpointIfMultiple(file, lineno, msg);
	if (bid == wxID_CANCEL || bid == BP_type_none) {
		return false;
	}

	int index = FindBreakpointById(bid);

	// sanity
	if (index < 0 || index >= (int)m_bps.size()) {
		wxLogMessage(wxT("IgnoreByLineno(): Insane index"));
		return false;
	}

	BreakpointInfo bp = m_bps.at(index);
	if (bp.bp_type == BP_type_invalid) {
		return false;
	}

	long newvalue = wxGetNumberFromUser( _("Please enter the new ignore-count"), wxT(""), wxT("Set ignore-count"), bp.ignore_number, 0, 1000000);
	if ((newvalue == -1) || (newvalue == (long)bp.ignore_number)) {
		return false;
	}

	if (! SetBPIgnoreCount(bid, newvalue)) {
		return false;
	}

	m_bps.at(index).ignore_number = newvalue;
	// Explicitly set the best type, in case the user just reset a previously-ignored bp
	SetBestBPType(m_bps.at(index));
	RefreshBreakpointMarkers();
	return true;
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
		wxLogMessage(wxT("BreakptMgr::EditBreakpoint: Insane index"));
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

	dlg.EnterBPData(bp);
	if (dlg.ShowModal() != wxID_OK) {
		return ;
	}

	SetBestBPType(dlg.b);	// The edited data's available. Use it to determine the best bp_type
	if (bp == dlg.b) {
		// Nothing was altered
		return ;
	}

	// We've got our altered dlg.b If the debugger's running, we can update it now
	// Otherwise, it'll be automatically inserted correctly when the debugger starts
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		if (CanThisBreakpointBeUpdated(dlg.b, bp)) {
			if (dlg.b.ignore_number != bp.ignore_number) {
				if (! SetBPIgnoreCount(dlg.b.debugger_id, dlg.b.ignore_number)) {
					return;	// Harsh, but what else can one do?
				}
			}
			if (dlg.b.is_enabled != bp.is_enabled) {
				if (! SetBPEnabledState(dlg.b.debugger_id, dlg.b.is_enabled)) {
					return ;
				}
			}
			if (dlg.b.conditions != bp.conditions) {
				if (! SetBPConditon(dlg.b)) {
					return ;
				}
			}
			if (dlg.b.commandlist != bp.commandlist) {
				if (! SetBPCommands(dlg.b)) {
					return ;
				}
			}
		} else {
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
	}

	// Replace the old data with the new, in m_bps
	m_bps.at(index) = dlg.b;
	DeleteAllBreakpointMarkers();
	RefreshBreakpointMarkers();
}

void BreakptMgr::ReconcileBreakpoints(std::vector<BreakpointInfo>& li)
{
	std::vector<BreakpointInfo> updated_bps;
	std::vector<BreakpointInfo>::iterator li_iter = li.begin();
	for (; li_iter != li.end(); ++li_iter) {
		int id = FindBreakpointById(li_iter->debugger_id);
		if (id == wxNOT_FOUND) {
			continue; // Shouldn't happen
		}
		// We've match the debugger_id from -break-list with a bp
		// Update the ignore-count, then store it in a new vector
		BreakpointInfo bp = m_bps.at(id);
		bp.ignore_number = li_iter->ignore_number;
		SetBestBPType(bp);	// as this might have just changed
		updated_bps.push_back(bp);
	}
	// All the still-existing bps have been added to updated_bps
	// So throw away m_bps (which will contain stale bps) and replace with the new vector
	// First though, delete all markers. Otherwise, if the last in a file has been deleted...
	DeleteAllBreakpointMarkers();
	m_bps.clear();
	SetBreakpoints(updated_bps);

	RefreshBreakpointMarkers();
	// update the Breakpoints pane too
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

	// When a a breakpoint is hit, see if it's got a command-list that needs faking
void BreakptMgr::BreakpointHit(int id)
{
	int index = FindBreakpointById(id);
	if ((index == wxNOT_FOUND) || (index >= FIRST_INTERNAL_ID)) {
		return;
	}

	BreakpointInfo bp = m_bps.at(index);
	if (bp.commandlist.IsEmpty()) {
		return;
	}

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// A likely command, presumably at the end of the command-list, is 'continue' or 'cont'
		// Filter this out and do it separately, otherwise Manager::UpdateLostControl isn't called to blank the indicator 
		static wxRegEx reContinue(wxT("(([[:space:]]|(^))((cont$)|(continue)))"));
		bool needsCont = false;
		wxString commands = bp.commandlist;
		if (reContinue.IsValid() && reContinue.Matches(commands)) {
			size_t start, len;
			if (reContinue.GetMatch(&start,&len)) {
				commands = commands.Left(start);
				needsCont = true;
			}
		}
		if (! commands.IsEmpty()) {	// Just in case someone's _only_ command is 'continue' !
			dbgr->ExecuteCmd(commands);
		}
		if (needsCont) {
			dbgr->Continue();
		}
	}
}

/*------------------------------- Implementation -------------------------------*/

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

bool BreakptMgr::SetBPIgnoreCount(const int bid, const int ignorecount)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// If the debugger is already running, tell it about the new ignore level
		// If not, it'll happen automatically when the debugger does start
		bool contIsNeeded = PauseDebuggerIfNeeded();
		bool result = dbgr->SetIgnoreLevel(bid, ignorecount);
		if (contIsNeeded) {
			dbgr->Continue();
		}
		return result;
	}
	return true;
}

bool BreakptMgr::SetBPCommands(const BreakpointInfo& bp)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// If the debugger is already running, tell it about the commands
		// If not, it'll happen automatically when the debugger does start
		bool contIsNeeded = PauseDebuggerIfNeeded();
		bool result = dbgr->SetCommands(bp);
		if (contIsNeeded) {
			dbgr->Continue();
		}
		return result;
	}
	return false;
}

bool BreakptMgr::SetBPConditon(const BreakpointInfo& bp)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// If the debugger is already running, tell it about the condition
		// If not, it'll happen automatically when the debugger does start
		bool contIsNeeded = PauseDebuggerIfNeeded();
		bool result = dbgr->SetCondition(bp);
		if (contIsNeeded) {
			dbgr->Continue();
		}
		return result;
	}
	return false;
}

bool BreakptMgr::SetBPEnabledState(const int bid, const bool enable)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		// If the debugger is already running, tell it about the new ignore level
		// If not, it'll happen automatically when the debugger does start
		bool contIsNeeded = PauseDebuggerIfNeeded();
		bool result = dbgr->SetEnabledState(bid, enable);
		if (contIsNeeded) {
			dbgr->Continue();
		}
		return result;
	}
	return true;
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
		if ((fileName == iter->file) && (iter->lineno != -1)) {
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

void BreakptMgr::SaveSession(SessionEntry& session)
{
	session.SetBreakpoints(m_bps);
}

void BreakptMgr::LoadSession(const SessionEntry& session)
{
	const std::vector<BreakpointInfo>& breakpoints = session.GetBreakpoints();
	for (std::vector<BreakpointInfo>::const_iterator itr = breakpoints.begin(); itr != breakpoints.end(); ++itr) {
		BreakpointInfo bp = *itr;
		bp.internal_id = GetNextID();
		AddBreakpoint(bp);
	}
}

void BreakptMgr::DragBreakpoint(LEditor* editor, int line, wxBitmap bitmap)
{
	// See if there's a bp marker under the cursor. If so, let the user drag it
	std::vector<BreakpointInfo> lineBPs;
	if (GetBreakpoints(lineBPs, editor->GetFileName().GetFullPath(), line+1) == 0) {
		return;
	}

	m_dragImage = new myDragImage(editor, bitmap, lineBPs);
	m_dragImage->StartDrag();
}

void BreakptMgr::DropBreakpoint(std::vector<BreakpointInfo>& BPs, int newline)
{
	// We've received back the vector of bps that we passed to DragBreakpoint()
	// We need to remove one from m_bps, and from the debugger if running,
	// then add it back with the lineno altered to newline
	wxString msg(_("Select the breakpoint that you want to move"));
	int bid = GetDesiredBreakpointIfMultiple(BPs[0].file, BPs[0].lineno, msg);
	if (bid == wxID_CANCEL || bid == BP_type_none) {
		return;
	}

	int index = FindBreakpointById(bid);
	BreakpointInfo bp = *(m_bps.begin()+index);
	bp.lineno = newline+1;
	// AddBreakpoint() doesn't do the disabled state: it can't, and there's no good way round this
	// So if the dragged bp was disabled, we need to enable it here, otherwise it retains its disabled icon
	bp.is_enabled = true;
	
	if (DelBreakpoint(bid)) {
		AddBreakpoint(bp);
		Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
	}
}

//---------------------------------------------------------

myDragImage::myDragImage(LEditor* ed, wxBitmap btmp, std::vector<BreakpointInfo>& BPs)
	: wxDragImage(btmp, wxCURSOR_POINT_LEFT), editor(ed), bitmap(btmp), lineBPs(BPs)
{
	// In theory, we could pass a blank cursor to wxDragImage::BeginDrag
	// In practice, that doesn't seem to work, so do it here, and undo it in OnEndDrag()
	oldcursor = editor->SetCursor(wxCursor(wxCURSOR_BLANK));
}

bool myDragImage::StartDrag()
{
	BeginDrag( wxPoint(0, 0),editor, false );
	wxPoint pt = editor->ScreenToClient(wxGetMousePosition());
	// Store the initial x position, as we know it must be in the bp margin
	m_startx = pt.x;
	Move(pt);
	Show();
	
	return false;
}

void myDragImage::OnMotion(wxMouseEvent& event)
{
	Move(event.GetPosition());
	event.Skip();
}

void myDragImage::OnEndDrag(wxMouseEvent& event)
{
	Hide();
	EndDrag();
	editor->SetCursor(oldcursor);
	editor->Disconnect(wxEVT_MOTION, wxMouseEventHandler(myDragImage::OnMotion), NULL, this);
	editor->Disconnect(wxEVT_LEFT_UP, wxMouseEventHandler(myDragImage::OnEndDrag), NULL, this);
	
	// If the cursor is within spitting distance of the bp margin, assume it's a genuine drop
	wxPoint pt = event.GetPosition();
	if ((pt.x < 0) || ((pt.x - m_startx) > 10)) {
		return;
	}
	// Find the desired new line, and check it's different from the previous
	long pos = editor->PositionFromPoint(pt);
	int newline = editor->LineFromPosition(pos);
	if ((newline+1) != lineBPs[0].lineno) {
		ManagerST::Get()->GetBreakpointsMgr()->DropBreakpoint(lineBPs, newline);
	}
}
