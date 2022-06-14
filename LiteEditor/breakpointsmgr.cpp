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
#include "breakpointsmgr.h"

#include "BreakpointsView.hpp"
#include "breakpointpropertiesdlg.h"
#include "debuggermanager.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "manager.h"

#include <wx/numdlg.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>

//---------------------------------------------------------
BreakptMgr::BreakptMgr()
{
    NextInternalID = FIRST_INTERNAL_ID;
    m_expectingControl = false;
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &BreakptMgr::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_BREAKPOINTS_LIST, &BreakptMgr::OnReconcileBreakpoints, this);
}

BreakptMgr::~BreakptMgr()
{
    Clear();
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &BreakptMgr::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_BREAKPOINTS_LIST, &BreakptMgr::OnReconcileBreakpoints, this);
}

bool BreakptMgr::AddBreakpointByAddress(const wxString& address)
{
    clDebuggerBreakpoint bp;
    bp.memory_address = address;
    bp.origin = BO_Other;
    bp.internal_id = GetNextID();
    return AddBreakpoint(bp);
}

bool BreakptMgr::AddBreakpointByLineno(const wxString& file, int lineno, const wxString& conditions, bool is_temp,
                                       bool is_disabled)
{
    clDebuggerBreakpoint bp;
    bp.Create(file, lineno, GetNextID());
    bp.origin = BO_Editor;
    if(is_temp) {
        bp.bp_type = BP_type_tempbreak;
        bp.is_temp = true;
    }
    bp.is_enabled = !is_disabled;
    bp.conditions = conditions;
    return AddBreakpoint(bp);
}

bool BreakptMgr::AddBreakpoint(const clDebuggerBreakpoint& bp)
{
    if(bp.bp_type != BP_type_watchpt && bp.file.IsEmpty() && bp.function_name.IsEmpty() &&
       bp.memory_address.IsEmpty() && bp.lineno == wxNOT_FOUND) {
        // no function nor file? no memory address?
        // do nothing then
        return true;
    }

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        // If the debugger is already running, tell it we want a new bp
        // If not, they'll all be added together when the debugger does start
        bool contIsNeeded = PauseDebuggerIfNeeded();

        dbgr->Break(bp);

        if(contIsNeeded) {
            dbgr->Continue();
        }
    }

    clDebuggerBreakpoint newBreakpoint(bp);
    SetBestBPType(newBreakpoint);

    clDebuggerBreakpoint::Vec_t::const_iterator iter = std::find(m_bps.begin(), m_bps.end(), newBreakpoint);
    if(iter == m_bps.end()) {
        // new breakpoint
        m_bps.push_back(newBreakpoint);
    }

    DeleteAllBreakpointMarkers();
    RefreshBreakpointMarkers();
    return true;
}

// Add a breakpoint using the 'Properties' dialog
void BreakptMgr::AddBreakpoint()
{
    BreakptPropertiesDlg dlg(NULL);
    dlg.SetTitle(_("Create a breakpoint or watchpoint"));

    clEditor* const editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    clDebuggerBreakpoint bp;
    bp.Create(editor ? CLRealPath(editor->GetFileName().GetFullPath()) : wxString(), editor ? editor->GetCurrentLine() : -1,
              GetNextID());
    dlg.EnterBPData(bp);

    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    if(AddBreakpoint(dlg.b)) {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if((!dlg.b.is_enabled) && dbgr && dbgr->IsRunning()) {
            SetBPEnabledState(dlg.b.debugger_id, dlg.b.is_enabled);
        }
        wxString msg;
        if(dlg.b.bp_type == BP_type_watchpt) {
            msg = _("Watchpoint successfully added");
        } else {
            msg = _("Breakpoint successfully added");
        }
        clMainFrame::Get()->GetStatusBar()->SetMessage(msg);
    }
}

void BreakptMgr::GetBreakpoints(clDebuggerBreakpoint::Vec_t& li)
{
    DoRemoveDuplicateBreakpoints();
    li = m_bps;
}

// Get all known breakpoints for this line/file
clDebuggerBreakpoint& BreakptMgr::GetBreakpoint(const wxString& fileName, const int lineno)
{
    auto iter = std::find_if(m_bps.begin(), m_bps.end(),
                             [&](const clDebuggerBreakpoint& a) { return a.file == fileName && a.lineno == lineno; });
    if(iter == m_bps.end()) {
        static clDebuggerBreakpoint empty;
        return empty;
    }
    return *iter;
}

const clDebuggerBreakpoint& BreakptMgr::GetBreakpoint(const wxString& fileName, const int lineno) const
{
    auto iter = std::find_if(m_bps.begin(), m_bps.end(),
                             [&](const clDebuggerBreakpoint& a) { return a.file == fileName && a.lineno == lineno; });
    if(iter == m_bps.end()) {
        static clDebuggerBreakpoint empty;
        return empty;
    }
    return *iter;
}

void BreakptMgr::GetTooltip(const wxString& fileName, int lineno, wxString& tip, wxString& title)
{
    if(fileName.IsEmpty() || lineno < 0) {
        return;
    }

    const clDebuggerBreakpoint& bp = GetBreakpoint(fileName, lineno);
    if(bp.IsNull()) {
        return;
    }

    int id = (bp.debugger_id > 0 ? bp.debugger_id : bp.internal_id - FIRST_INTERNAL_ID);
    wxString strike = (bp.is_enabled ? "" : "~~");
    title << "### " << strike << _("Breakpoint# ") << id << strike;

    if(bp.is_temp && !bp.conditions.IsEmpty()) {
        tip << _("Temporary conditional breakpoint");
    } else if(bp.is_temp) {
        tip << _("Temporary breakpoint");
    } else if(!bp.conditions.IsEmpty()) {
        tip << _("Conditional breakpoint");
    } else {
        tip << _("Normal breakpoint");
    }

    if(!bp.is_enabled) {
        tip << _(" (disabled)");
    }
    tip << "\n";

    if(bp.ignore_number > 0) {
        tip << _("Ignore-count: ") << bp.ignore_number << "\n";
    }
    if(!bp.conditions.IsEmpty()) {
        tip << _("Condition: ") << "`" << bp.conditions << "`\n";
    }
    if(!bp.commandlist.IsEmpty()) {
        tip << _("Commands: ") << "\n";

        wxArrayString commands = ::wxStringTokenize(bp.commandlist, "\n", wxTOKEN_STRTOK);
        for(const wxString& command : commands) {
            tip << "  `" << command << "`\n";
        }
    }

    tip.Trim().Trim(false);
}

// Delete all line-type breakpoint markers in all editors
// Done before refreshing after a delete or edit, lest it was the last bp in a file
void BreakptMgr::DeleteAllBreakpointMarkers()
{
    clEditor::Vec_t editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); ++i) {
        editors.at(i)->DelAllBreakpointMarkers();
    }
}

// Refresh all line-type breakpoint markers in all editors
void BreakptMgr::RefreshBreakpointMarkers()
{
    std::vector<clEditor*> editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);

    for(size_t i = 0; i < editors.size(); i++) {
        DoRefreshFileBreakpoints(editors.at(i));
    }

    // Fire wxEVT_BREAKPOINTS_UPDATED event
    clDebugEvent event(wxEVT_BREAKPOINTS_UPDATED);
    EventNotifier::Get()->QueueEvent(event.Clone());
}

// Delete all breakpoint markers for this file, then re-mark with the currently-correct marker
void BreakptMgr::DoRefreshFileBreakpoints(clEditor* editor)
{
    // Load the file's line-type bps into fi, and make a set of their line-numbers
    std::multimap<int, clDebuggerBreakpoint> bps;
    for(size_t i = 0; i < m_bps.size(); ++i) {
        clDebuggerBreakpoint& b = m_bps[i];
        wxString remotePath = editor->GetRemotePath();
        if(!remotePath.empty()) {
            // this is a remote file
            wxFileName fn(b.file);
            wxString unix_path = fn.GetFullPath(wxPATH_UNIX);
            if(remotePath == unix_path && b.lineno != -1) {
                b.file.swap(unix_path);
                bps.insert({ b.lineno, b });
            }
        } else {
            if((editor->GetFileName() == b.file) && (b.lineno != -1)) {
                bps.insert({ b.lineno, b });
            }
        }
    }

    editor->DelAllBreakpointMarkers();

    // the multimap now holds a table of
    // line numbers and breakpointsInfo, collect all line numbers's breakpoint info into vector
    // and place markers
    for(std::multimap<int, clDebuggerBreakpoint>::iterator i = bps.begin(); i != bps.end(); i++) {
        std::pair<std::multimap<int, clDebuggerBreakpoint>::iterator,
                  std::multimap<int, clDebuggerBreakpoint>::iterator>
            range = bps.equal_range(i->first);
        std::vector<clDebuggerBreakpoint> v;
        int count = 0;
        for(std::multimap<int, clDebuggerBreakpoint>::iterator it = range.first; it != range.second; it++) {
            v.push_back(it->second);
            ++count;
        }
        // Inc over the rest of the range, otherwise there'll be duplication when there's >1 bp on a line
        while(--count > 0) {
            ++i;
        }
        // Now work out which is the most significant (in marker terms) and tell the editor
        DoProvideBestBP_Type(editor, v);
    }
}

// Given a list of bps, tell the editor which is the most significant (in marker terms)
void BreakptMgr::DoProvideBestBP_Type(clEditor* editor, const std::vector<clDebuggerBreakpoint>& li)
{
    if((editor == NULL) || (li.size() == 0)) {
        return;
    }
    // If there's an enabled bp of any sort, it beats all disabled ones
    // Otherwise, BP_type_break > BP_type_tempbreak > BP_type_cmdlistbreak > BP_type_condbreak > BP_type_ignoredbreak
    int values[BP_LAST_MARKED_ITEM + 1]; // Allow for BP_type_none = 0
    values[BP_type_break] = 100;
    values[BP_type_tempbreak] = 90;
    values[BP_type_cmdlistbreak] = 80;
    values[BP_type_condbreak] = 70;
    values[BP_type_ignoredbreak] = 60;
    values[BP_type_none] = 0;

    BreakpointType best = BP_type_none;
    int best_value = 0;
    bool is_disabled = false;
    std::vector<clDebuggerBreakpoint>::const_iterator iter = li.begin();
    for(; iter != li.end(); ++iter) {
        BreakpointType bpt = iter->bp_type;
        if(bpt == BP_type_invalid) {
            continue;
        }
        int val = values[bpt];
        if(!iter->is_enabled) {
            val /= 2; // Halving the value for disability means that abled always outranks disabled, without otherwise
                      // interfering with the order
        }
        if(val > best_value) {
            best = bpt;
            best_value = val;
            is_disabled = !iter->is_enabled; // If the new item wins, store its data
        }
    }

    if(best > 0) {
        // ATTN: this wierd allocation is due to bug in optimization of g++
        // which seems to crash (removing optimization fixes this problem,
        // but we prefer to stick with optimization level 2)
        std::vector<clDebuggerBreakpoint>* v = new std::vector<clDebuggerBreakpoint>(li);
        editor->SetBreakpointMarker(li.at(0).lineno, best, is_disabled, *v);
        delete v;
    }
}

// Invalidates each debugger_id
void BreakptMgr::DebuggerStopped()
{
    std::vector<clDebuggerBreakpoint> newList;
    for(size_t i = 0; i < m_bps.size(); i++) {
        clDebuggerBreakpoint& bp = m_bps.at(i);
        bp.debugger_id = -1;

        // We collect all the breakpoints which their origin
        // was the editor
        if(bp.file.IsEmpty() == false && bp.lineno != wxNOT_FOUND) {
            newList.push_back(bp);
        }
    }

    for(size_t i = 0; i < m_pendingBreakpointsList.size(); i++) {
        clDebuggerBreakpoint& bp = m_pendingBreakpointsList.at(i);
        bp.debugger_id = -1;

        // We collect all the breakpoints which their origin
        // was the editor
        if(bp.origin == BO_Editor) {
            newList.push_back(bp);
        }
    }

    m_bps.clear();
    m_pendingBreakpointsList.clear();

    m_bps = newList;
    DoRemoveDuplicateBreakpoints();
}

bool BreakptMgr::DelBreakpoint(double id)
{
    int index = FindBreakpointById(id, m_bps);
    if(index == wxNOT_FOUND) {
        return false;
    }

    // remove it from the debugger if it's running
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        if(id > FIRST_INTERNAL_ID) {
            // This shouldn't happen while the debugger is running (debugger_id should be valid)
            // But if it does, assume it was a bp that gdb couldn't create, and just remove from the bp list
        } else {
            bool contIsNeeded = PauseDebuggerIfNeeded();
            if(dbgr->RemoveBreak(id)) {
                // Strangely, -break-delete doesn't output any confirmation except for ^done. So do it here
                wxString msg = ((m_bps.at(index).bp_type == BP_type_watchpt) ? _("Watchpoint ") : _("Breakpoint "));
                ManagerST::Get()->UpdateAddLine(msg + wxString::Format(_("%i deleted"), (int)id));
            }
            if(contIsNeeded) {
                dbgr->Continue();
            }
        }
    }

    // Delete all markers before removing bp from the vector. Otherwise if id was the last in a file...
    DeleteAllBreakpointMarkers();

    m_bps.erase(m_bps.begin() + index);
    RefreshBreakpointMarkers();
    return true;
}

bool BreakptMgr::DelBreakpointByLineno(const wxString& file, const int lineno)
{
    const clDebuggerBreakpoint& bp = GetBreakpoint(file, lineno);
    if(bp.IsNull()) {
        return false;
    }

    double bpId = bp.GetId();
    if(bpId == wxID_CANCEL || bpId == BP_type_none)
        return false;

    DelBreakpoint(bpId);
    return true;
}

void BreakptMgr::ApplyPendingBreakpoints()
{
    if(!PendingBreakpointsExist()) {
        return; // Nothing to do
    }

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!(dbgr && dbgr->IsRunning())) {
        return; // If the debugger isn't running, there's no point (and we shouldn't have reached here anyway)
    }
    bool contIsNeeded = PauseDebuggerIfNeeded();

    for(size_t i = m_pendingBreakpointsList.size(); i > 0; --i) {
        clDebuggerBreakpoint bp = m_pendingBreakpointsList.at(i - 1);

        // First check to see if the debugger already accepted this one
        // The answer should be no, as acceptance should have removed it from this list
        int index = FindBreakpointById(bp.internal_id, m_bps);
        if(index != wxNOT_FOUND) {
            // Hmm. See if there's a valid debugger_id. If so, the bp *was* accepted, and shouldn't be on the pending
            // list
            if(m_bps.at(index).debugger_id != -1) {
                m_pendingBreakpointsList.erase(m_pendingBreakpointsList.begin() + i - 1);
            }
            continue; // The bp hasn't been assessed yet; it's probably pointless to try to reapply it atm
        }

        // This bp didn't 'take' the first time; "..try, try again" :p
        dbgr->Break(bp);
        m_bps.push_back(bp);
    }

    if(contIsNeeded) {
        dbgr->Continue();
    }

    RefreshBreakpointMarkers();
}

void BreakptMgr::DelAllBreakpoints()
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        bool contIsNeeded = PauseDebuggerIfNeeded();
        dbgr->RemoveAllBreaks();
        if(contIsNeeded) {
            dbgr->Continue();
        }
    }

    // Delete all markers before clearing m_bps, otherwise we won't know which files they were in
    DeleteAllBreakpointMarkers();
    m_bps.clear();
    m_pendingBreakpointsList.clear(); // Delete any pending bps too

    // Fire wxEVT_BREAKPOINTS_UPDATED event
    clDebugEvent evt(wxEVT_BREAKPOINTS_UPDATED);
    EventNotifier::Get()->QueueEvent(evt.Clone());
}

void BreakptMgr::SetAllBreakpointsEnabledState(bool enabled)
{
    unsigned int successes = 0;
    bool debuggerIsRunning = false;
    bool contIsNeeded = false;

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        debuggerIsRunning = true;
        contIsNeeded = PauseDebuggerIfNeeded();
    }

    for(size_t i = 0; i < m_bps.size(); ++i) {
        clDebuggerBreakpoint& bp = m_bps.at(i);
        if(((bp.debugger_id != -1) || !debuggerIsRunning) // Sanity check for when the debugger's running
           && (bp.is_enabled != enabled)) {               // No point setting it to the current status
            if(debuggerIsRunning) {
                if(dbgr->SetEnabledState(bp.debugger_id, enabled)) {
                    bp.is_enabled = enabled;
                    ++successes;
                }
            } else {
                bp.is_enabled = enabled;
                ++successes;
            }
        }
    }

    if(debuggerIsRunning && contIsNeeded) {
        dbgr->Continue();
    }

    if(successes) {
        RefreshBreakpointMarkers();
        clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();

        wxString msg = wxString::Format(wxT("%u "), successes);
        msg << (enabled ? _("breakpoints enabled") : _("breakpoints disabled"));
        clMainFrame::Get()->GetStatusBar()->SetMessage(msg);
    }
}

// The debugger labels each breakpoint with an id and passes it here via Manager::UpdateBpAdded.
// By storing it as clDebuggerBreakpoint::debugger_id, we can be certain that this bp refers to the gdb breakpoint
void BreakptMgr::SetBreakpointDebuggerID(const int internal_id, const int debugger_id)
{
    std::vector<clDebuggerBreakpoint>::iterator iter = m_bps.begin();
    for(; iter != m_bps.end(); ++iter) {
        if(iter->internal_id == internal_id) {
            // We've found the match. However, if breakpoint creation had failed, confess, then delete
            if(debugger_id == -1) {
                wxString msg;
                if(iter->bp_type == BP_type_watchpt) {
                    msg = _("Watchpoint creation unsuccessful");
                } else {
                    msg = _("Breakpoint creation unsuccessful");
                }

                // add message to the debugger tab
                ManagerST::Get()->UpdateAddLine(msg);
                DeleteAllBreakpointMarkers(); // Must do this before the erase, otherwise the last-bp-in-file will be
                                              // missed
                m_bps.erase(iter);

                // update the UI as well
                clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
                RefreshBreakpointMarkers();
                return;
            }
            // Otherwise store the valid debugger_id
            iter->debugger_id = debugger_id;
            // Remove the bp from  the 'pending' array
            int index = FindBreakpointById(internal_id, m_pendingBreakpointsList);
            if(index != wxNOT_FOUND) {
                m_pendingBreakpointsList.erase(m_pendingBreakpointsList.begin() + index);
            }
            // update the UI as well
            clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
            return;
        }
    }
    clLogMessage(wxT("SetBreakpointDebuggerID(): Failed to match internal_id to debugger_id"));
}

// Set a breakpoint's ignore count
bool BreakptMgr::IgnoreByLineno(const wxString& file, const int lineno)
{
    clDebuggerBreakpoint& bp = GetBreakpoint(file, lineno);
    if(bp.IsNull()) {
        return false;
    }

    if(bp.bp_type == BP_type_invalid) {
        return false;
    }

    long newvalue = ::wxGetNumberFromUser(_("Please enter the new ignore-count"), wxT(""), _("Set ignore-count"),
                                          bp.ignore_number, 0, 1000000);
    if((newvalue == -1) || (newvalue == (long)bp.ignore_number)) {
        return false;
    }

    if(!SetBPIgnoreCount(bp.GetId(), newvalue)) {
        return false;
    }

    bp.ignore_number = newvalue;

    // Explicitly set the best type, in case the user just reset a previously-ignored bp
    SetBestBPType(bp);

    RefreshBreakpointMarkers();
    return true;
}

void BreakptMgr::EditBreakpointByLineno(const wxString& file, const int lineno)
{
    const clDebuggerBreakpoint& bp = GetBreakpoint(file, lineno);
    if(bp.IsNull())
        return;

    int bid = bp.GetId();
    if(bid == wxID_CANCEL || bid == BP_type_none) {
        return;
    }

    int index = FindBreakpointById(bid, m_bps);
    if(index == wxNOT_FOUND) {
        return;
    }

    bool dummy;
    EditBreakpoint(index, dummy);
}

void BreakptMgr::EditBreakpoint(int index, bool& bpExist)
{
    // sanity
    bpExist = true;
    if(index < 0 || index >= (int)m_bps.size()) {
        clLogMessage(wxT("BreakptMgr::EditBreakpoint: Insane index"));
        bpExist = false;
        return;
    }

    clDebuggerBreakpoint bp = m_bps.at(index);
    BreakptPropertiesDlg dlg(NULL);
    wxString title;
    if(bp.bp_type == BP_type_watchpt) {
        title = _("Properties for watchpoint ");
    } else {
        title = _("Properties for breakpoint ");
    }
    double id = bp.debugger_id;
    if(id == -1) {
        id = bp.internal_id - FIRST_INTERNAL_ID;
    }
    title << id;
    dlg.SetTitle(title);

    dlg.EnterBPData(bp);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    SetBestBPType(dlg.b); // The edited data's available. Use it to determine the best bp_type
    if(bp == dlg.b) {
        // Nothing was altered
        return;
    }

    // We've got our altered dlg.b If the debugger's running, we can update it now
    // Otherwise, it'll be automatically inserted correctly when the debugger starts
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        if(CanThisBreakpointBeUpdated(dlg.b, bp)) {
            if(dlg.b.ignore_number != bp.ignore_number) {
                if(!SetBPIgnoreCount(dlg.b.debugger_id, dlg.b.ignore_number)) {
                    return; // Harsh, but what else can one do?
                }
            }
            if(dlg.b.is_enabled != bp.is_enabled) {
                if(!SetBPEnabledState(dlg.b.debugger_id, dlg.b.is_enabled)) {
                    return;
                }
            }
            if(dlg.b.conditions != bp.conditions) {
                if(!SetBPConditon(dlg.b)) {
                    return;
                }
            }
            if(dlg.b.commandlist != bp.commandlist) {
                if(!SetBPCommands(dlg.b)) {
                    return;
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
            if(contIsNeeded) {
                dbgr->Continue();
            }
        }
    }

    // Replace the old data with the new, in m_bps
    m_bps.at(index) = dlg.b;
    DeleteAllBreakpointMarkers();
    RefreshBreakpointMarkers();
}

void BreakptMgr::OnReconcileBreakpoints(clDebugEvent& event)
{
    // reconcile the real breakpoint list with the one stored here
    const auto& actual_bps_list = event.GetBreakpoints();

    std::vector<clDebuggerBreakpoint> updated_bps;
    updated_bps.reserve(actual_bps_list.size());

    auto debugger = DebuggerMgr::Get().GetActiveDebugger();
    bool remote_debugging = false;
    if(debugger && debugger->IsSSHDebugging()) {
        // convert breakpoint path to unix paths
        remote_debugging = true;
    }

    for(auto bp : actual_bps_list) {
        if(remote_debugging) {
            wxFileName fn(bp.file);
            bp.file = fn.GetFullPath(wxPATH_UNIX);
        }

        int index = FindBreakpointById(bp.debugger_id, m_bps);
        if(index == wxNOT_FOUND) {

            if(IsDuplicate(bp, updated_bps))
                continue;

            // This will happen e.g. if a bp was auto-set on Main()
            // If so, its internal_id will be invalid
            bp.internal_id = GetNextID();
            updated_bps.emplace_back(bp);

        } else {
            // We've match the debugger_id from -break-list with a bp
            // Update the ignore-count, then store it in a new vector
            clDebuggerBreakpoint bpt = m_bps[index];
            bpt.ignore_number = bp.ignore_number;
            bpt.what = bp.what;
            bpt.at = bp.at;
            bpt.file = bp.file;

            // Remove it from the m_bps list
            m_bps.erase(m_bps.begin() + index);

            SetBestBPType(bpt); // as this might have just changed
            updated_bps.emplace_back(bpt);
        }
    }

    // All the still-existing bps have been added to updated_bps
    // So throw away m_bps (which will contain stale bps) and replace with the new vector
    // First though, delete all markers. Otherwise, if the last in a file has been deleted...
    DeleteAllBreakpointMarkers();

    // All the stale breakpoints should be assigned to the 'm_pendingBreakpointList'
    m_pendingBreakpointsList = m_bps;

    m_bps.clear();
    SetBreakpoints(updated_bps);

    RefreshBreakpointMarkers();
    // update the Breakpoints pane too
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

// When a a breakpoint is hit, see if it's got a command-list that needs faking
void BreakptMgr::BreakpointHit(double id)
{
    int index = FindBreakpointById(id, m_bps);
    if((index == wxNOT_FOUND) || (index >= FIRST_INTERNAL_ID)) {
        return;
    }

    clDebuggerBreakpoint bp = m_bps.at(index);
    if(!bp.commandlist.IsEmpty()) {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning()) {
            // A likely command, presumably at the end of the command-list, is 'continue' or 'cont'
            // Filter this out and do it separately, otherwise Manager::UpdateLostControl isn't called to blank the
            // indicator
            static wxRegEx reContinue(wxT("(([[:space:]]|(^))((cont$)|(continue)))"));
            bool needsCont = false;
            wxString commands = bp.commandlist;
            if(reContinue.IsValid() && reContinue.Matches(commands)) {
                size_t start, len;
                if(reContinue.GetMatch(&start, &len)) {
                    commands = commands.Left(start);
                    needsCont = true;
                }
            }
            if(!commands.IsEmpty()) { // Just in case someone's _only_ command is 'continue' !
                dbgr->ExecuteCmd(commands);
            }
            if(needsCont) {
                dbgr->Continue();
            }
        }
    }

    if(bp.bp_type == BP_type_tempbreak) {
        // If this is a temporary bp, remove it from m_bps now it's been hit
        // Otherwise it will be treated as a 'Pending' bp, the button will be displayed
        // and, if clicked, the bp will be resurrected.
        int index = FindBreakpointById(id, m_bps);
        if(index != wxNOT_FOUND) {
            m_bps.erase(m_bps.begin() + index);
        }
    }
}

/*------------------------------- Implementation -------------------------------*/

// Construct set of files containing bps
std::set<wxString> BreakptMgr::GetFilesWithBreakpointMarkers()
{
    // Make a set of all filenames containing a file-relevant variety of breakpoint
    std::set<wxString> filenames;
    std::vector<clDebuggerBreakpoint>::iterator iter = m_bps.begin();
    for(; iter != m_bps.end(); ++iter) {
        // If this bp is a lineno or function type, add its file to the set
        wxString fileName = iter->file;
        if((!fileName.IsEmpty()) && (iter->memory_address.IsEmpty())) {
            filenames.insert(fileName);
        }
    }

    return filenames;
}

int BreakptMgr::FindBreakpointById(double id, const std::vector<clDebuggerBreakpoint>& li)
{
    std::vector<clDebuggerBreakpoint>::const_iterator iter = li.begin();
    for(; iter != li.end(); ++iter) {
        clDebuggerBreakpoint b = *iter;
        // If we were passed an id > 10000, it must be internal
        double this_id = (id > FIRST_INTERNAL_ID ? b.internal_id : b.debugger_id);
        if(id == this_id) {
            return (int)(iter - li.begin());
        }
    }

    return wxNOT_FOUND;
}

bool BreakptMgr::CanThisBreakpointBeUpdated(const clDebuggerBreakpoint& a, const clDebuggerBreakpoint& b) const
{
    // Since, afaik, the only things that *can* be updated are: conditions/commands/ignores/enables,
    // return true if all but these are == (except for bp_type, which will have changed as a consequence of those)
    clDebuggerBreakpoint temp = a;
    temp.ignore_number = b.ignore_number;
    temp.is_enabled = b.is_enabled;
    temp.conditions = b.conditions;
    temp.commandlist = b.commandlist;
    temp.bp_type = b.bp_type;
    return (temp == b);
}

bool BreakptMgr::SetBPIgnoreCount(double bid, const int ignorecount)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        // If the debugger is already running, tell it about the new ignore level
        // If not, it'll happen automatically when the debugger does start
        bool contIsNeeded = PauseDebuggerIfNeeded();
        bool result = dbgr->SetIgnoreLevel(bid, ignorecount);
        if(contIsNeeded) {
            dbgr->Continue();
        }
        return result;
    }
    return true;
}

bool BreakptMgr::SetBPCommands(const clDebuggerBreakpoint& bp)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        // If the debugger is already running, tell it about the commands
        // If not, it'll happen automatically when the debugger does start
        bool contIsNeeded = PauseDebuggerIfNeeded();
        bool result = dbgr->SetCommands(bp);
        if(contIsNeeded) {
            dbgr->Continue();
        }
        return result;
    }
    return false;
}

bool BreakptMgr::SetBPConditon(const clDebuggerBreakpoint& bp)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        // If the debugger is already running, tell it about the condition
        // If not, it'll happen automatically when the debugger does start
        bool contIsNeeded = PauseDebuggerIfNeeded();
        bool result = dbgr->SetCondition(bp);
        if(contIsNeeded) {
            dbgr->Continue();
        }
        return result;
    }
    return false;
}

bool BreakptMgr::SetBPEnabledState(double bid, const bool enable)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        // If the debugger is already running, tell it about the new 'enable' level
        // If not, it'll happen automatically when the debugger does start
        bool contIsNeeded = PauseDebuggerIfNeeded();
        bool result = dbgr->SetEnabledState(bid, enable);
        if(contIsNeeded) {
            dbgr->Continue();
        }
        return result;
    }
    return true;
}

void BreakptMgr::SetBestBPType(clDebuggerBreakpoint& bp)
{
    if(bp.bp_type == BP_type_watchpt) {
        return;
    }

    if(bp.ignore_number > 0) {
        bp.bp_type = BP_type_ignoredbreak;
        return;
    }

    if(bp.IsConditional()) {
        bp.bp_type = BP_type_condbreak;
        return;
    }

    if(!bp.commandlist.IsEmpty()) {
        bp.bp_type = BP_type_cmdlistbreak;
        return;
    }

    if(bp.is_temp) {
        bp.bp_type = BP_type_tempbreak;
        return;
    }

    bp.bp_type = BP_type_break; // Default option
}

// If the debugger is running but can't interact, pause it and return true (to flag needs restart)
bool BreakptMgr::PauseDebuggerIfNeeded()
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning() && !ManagerST::Get()->DbgCanInteract()) {
        SetExpectingControl(true);
        dbgr->Interrupt();
        return true;
    }
    return false;
}

void BreakptMgr::DeleteAllBreakpointsByFileName(const wxString& fileName)
{
    std::vector<clDebuggerBreakpoint>::iterator iter = m_bps.begin();
    while(iter != m_bps.end()) {
        if((fileName == iter->file) && (iter->lineno != -1)) {
            iter = m_bps.erase(iter);
        } else {
            ++iter;
        }
    }
}

void BreakptMgr::SetBreakpoints(const std::vector<clDebuggerBreakpoint>& bps)
{
    // append the breakpoint list
    m_bps.insert(m_bps.end(), bps.begin(), bps.end());
}

bool BreakptMgr::AreThereEnabledBreakpoints(bool enabled /*= true*/)
{
    for(size_t i = 0; i < m_bps.size(); ++i) {
        clDebuggerBreakpoint& bp = m_bps.at(i);
        if(bp.is_enabled == enabled) {
            // There's at least one bp that can be altered
            return true;
        }
    }

    return false;
}

void BreakptMgr::SaveSession(SessionEntry& session) { session.SetBreakpoints(m_bps); }

void BreakptMgr::LoadSession(const SessionEntry& session)
{
    // when loading new session, clear everything before we continue
    DelAllBreakpoints();

    for(auto bp : session.GetBreakpoints()) {
        bp.internal_id = GetNextID();
        bp.origin = BO_Editor;
        AddBreakpoint(bp);
    }
    RefreshBreakpointMarkers();
}

void BreakptMgr::DragBreakpoint(clEditor* editor, int line, wxBitmap bitmap)
{
    // See if there's a bp marker under the cursor. If so, let the user drag it
    clDebuggerBreakpoint& bp = GetBreakpoint(CLRealPath(editor->GetFileName().GetFullPath()), line + 1);
    if(bp.IsNull()) {
        return;
    }

    m_dragImage = new myDragImage(editor, bitmap, bp);
    m_dragImage->StartDrag();
}

void BreakptMgr::DropBreakpoint(const clDebuggerBreakpoint& bp, int newline)
{
    if(DelBreakpoint(bp.GetId())) {
        clDebuggerBreakpoint new_bp = bp;
        new_bp.lineno = newline;
        AddBreakpoint(new_bp);
    }
}

//---------------------------------------------------------

myDragImage::myDragImage(clEditor* ed, const wxBitmap& bitmap, const clDebuggerBreakpoint& bp)
    : wxDragImage(bitmap, wxCURSOR_POINT_LEFT)
    , editor(ed)
    , m_bp(bp)
{
    // In theory, we could pass a blank cursor to wxDragImage::BeginDrag
    // In practice, that doesn't seem to work, so do it here, and undo it in OnEndDrag()
    oldcursor = editor->GetCursor();
    editor->SetCursor(wxCursor(wxCURSOR_BLANK));
}

bool myDragImage::StartDrag()
{
    BeginDrag(wxPoint(0, 0), editor, false);
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
    if((pt.x < 0) || ((pt.x - m_startx) > 10)) {
        return;
    }
    // Find the desired new line, and check it's different from the previous
    long pos = editor->PositionFromPoint(pt);
    int newline = editor->LineFromPosition(pos);
    if((newline + 1) != m_bp.lineno) {
        ManagerST::Get()->GetBreakpointsMgr()->DropBreakpoint(m_bp, newline);
    }
}

void BreakptMgr::RefreshBreakpointsForEditor(clEditor* editor) { DoRefreshFileBreakpoints(editor); }

bool BreakptMgr::IsDuplicate(const clDebuggerBreakpoint& bp, const std::vector<clDebuggerBreakpoint>& bpList)
{
    if(bp.debugger_id != wxNOT_FOUND) {
        return false; // A real breakpoint
    }

    wxString bpFile = bp.file;
    for(size_t i = 0; i < bpList.size(); i++) {
        if(bpList.at(i).file.IsEmpty() == false) {
            wxFileName fn(bpList.at(i).file);
            fn.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG);

            if(fn.GetFullPath() == bpFile && bp.lineno == bpList.at(i).lineno)
                return true;
        } else {
            if(bpList.at(i).file == bpFile && bp.lineno == bpList.at(i).lineno)
                return true;
        }
    }
    return false;
}

void BreakptMgr::DoRemoveDuplicateBreakpoints()
{
    std::vector<clDebuggerBreakpoint> bps;
    std::map<wxString, clDebuggerBreakpoint> uniqueNormalBreakpoints;
    for(size_t i = 0; i < m_bps.size(); ++i) {
        const clDebuggerBreakpoint& bi = m_bps.at(i);

        if(bi.bp_type != BP_type_break) {
            bps.push_back(bi);

        } else {

            if(bi.debugger_id != wxNOT_FOUND) {
                // an actual breakpoint, so it must be unique
                bps.push_back(bi);
                continue;
            }

            wxString key;

            /// construct a unique identifier for the breakpoint
            if(!bi.memory_address.IsEmpty()) {
                key << bi.memory_address;

            } else if(!bi.function_name.IsEmpty()) {
                key << bi.function_name;

            } else {
                key << bi.file << bi.lineno;
            }

            if(uniqueNormalBreakpoints.count(key)) {
                continue;
            } else {
                uniqueNormalBreakpoints.insert(std::make_pair(key, bi));
            }
        }
    }
    std::map<wxString, clDebuggerBreakpoint>::iterator iter = uniqueNormalBreakpoints.begin();
    for(; iter != uniqueNormalBreakpoints.end(); ++iter) {
        bps.push_back(iter->second);
    }
    m_bps.swap(bps);
}

int BreakptMgr::DelBreakpointByAddress(const wxString& address)
{
    std::vector<clDebuggerBreakpoint> allBps; // Start by finding all on the line
    GetBreakpoints(allBps);

    int breakpointsRemoved = 0;
    for(size_t i = 0; i < allBps.size(); i++) {
        clDebuggerBreakpoint& bp = allBps.at(i);
        if(bp.memory_address == address) {
            int bpId = (bp.debugger_id == -1 ? bp.internal_id : bp.debugger_id);

            if(bpId == wxID_CANCEL || bpId == BP_type_none)
                continue;

            if(DelBreakpoint(bpId)) {
                breakpointsRemoved++;
            }
        }
    }
    return breakpointsRemoved;
}

void BreakptMgr::GetAllMemoryBreakpoints(clDebuggerBreakpoint::Vec_t& memoryBps)
{
    clDebuggerBreakpoint::Vec_t allBps; // Start by finding all on the line
    GetBreakpoints(allBps);

    for(size_t i = 0; i < allBps.size(); i++) {
        clDebuggerBreakpoint& bp = allBps.at(i);
        if(!bp.memory_address.IsEmpty()) {
            memoryBps.push_back(bp);
        }
    }
}

void BreakptMgr::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    DelAllBreakpoints();
}
