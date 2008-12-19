//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : breakpointmgr.h
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
 #ifndef BREAKPOINTS_MANAGER_H
#define BREAKPOINTS_MANAGER_H

#include "list"
#include "vector"
#include <set>
#include "wx/string.h"
#include "debugger.h"
#include "cl_editor.h"
#include "wx/arrstr.h"

#define FIRST_INTERNAL_ID 10000

class BreakptMgr
{
	std::vector<BreakpointInfo> m_bps;
	int NextInternalID;		// Used to give each bp a unique internal ID. Start at 10k to avoid confusion with gdb's IDs
	// Delete all breakpoint markers for this file, then re-mark with the currently-correct marker
	void DoRefreshFileBreakpoints(LEditor* editor, const wxString fileName);
	void DoProvideBestBP_Type(LEditor* editor, std::vector<BreakpointInfo>& li);	// Tells the editor which is the most appropriate bp marker to show
	// Delete all line-type breakpoint markers in all editors
	// Done before refreshing after a delete, lest it was the last bp in a file
	void DeleteAllBreakpointMarkers();
	std::set<wxString> GetFilesWithBreakpointMarkers();
	int FindBreakpointById(const int id);

	/**
	 * Can gdb accept this alteration, or will be bp have to be replaced?
	 */
	bool CanThisBreakpointBeUpdated(const BreakpointInfo &bp1, const BreakpointInfo &bp2) const;

	/**
	 * Tell the debugger about the ignore count, conditions etc
	 */
	void DoBreakpointExtras(BreakpointInfo &bp);

	/**
	 * Get id of the breakpoint on this line. If multiple bps, ask the user to select
	 */
	int GetDesiredBreakpointIfMultiple(const wxString &fileName, const int lineno, const wxString msg = wxT(""));

	/**
	 * Refresh all line-type breakpoint markers in all editors
	*/
	void RefreshBreakpointMarkers();

	/**
	 * Sets bp.bp_type to the value most appropriate to its contents
	 */
	void SetBestBPType(BreakpointInfo& bp);

	/**
	 * Ignore this bp
	 */
	bool SetBPIgnoreCount(const int bid, const int ignorecount);

	/**
	 * Enable or Disable this breakpoint
	 */
	bool SetBPEnabledState(const int bid, const bool enable);

	/**
	 * Set this breakpoint's condition to that in bp.condition
	 */
	bool SetBPConditon(const BreakpointInfo &bp);

	/**
	 * Set this breakpoint's command-list to that in bp.commandlist
	 */
	bool SetBPCommands(const BreakpointInfo &bp);

	/**
	 * Clear the list of breakpoints
	 */
	void Clear() {
		m_bps.clear();
	}

public:

	BreakptMgr() {
		NextInternalID = FIRST_INTERNAL_ID;
	}

	~BreakptMgr() {
		Clear();
	}

	/**
	 * Add a breakpoint to the current debugger at the given line-number/file
	 * Depending on the parameters, a temporary/ignored/conditional/commandlist bp can be created
	 */
	bool AddBreakpointByLineno(const wxString& file, const int lineno, const wxString& conditions = wxT(""), const bool is_temp = false);

	/**
	 * Add a breakpoint using the 'Properties' dialog
	 * Depending on the parameters, a temporary/ignored/conditional/commandlist bp can be created
	 */
	void AddBreakpoint();

	/**
	 * Add a break point to the current debugger
	 * all breakpoints will be passed to the debugger
	 * once started
	 */
	bool AddBreakpoint(BreakpointInfo &bp);

	/**
	 * Delete break point by id
	 */
	bool DelBreakpoint(const int id);

	/**
	 * Delete break point by file and line
	 */
	int DelBreakpointByLineno(const wxString& file, const int lineno);

	/**
	 * Toggle a breakpoint's enabled state
	 */
	bool ToggleEnabledStateByLineno(const wxString& file, const int lineno);

	/**
	 * Summon the BreakptProperties dialog for a bp
	 */
	void EditBreakpointByLineno(const wxString& file, const int lineno);

	/**
	 * Set a breakpoint's ignore count
	 */
	bool IgnoreByLineno(const wxString& file, const int lineno);

	/**
	 * return list of breakpoints
	 */
	void GetBreakpoints(std::vector<BreakpointInfo> &li);

	/**
	 * When a breakpoint is added, the debugger_id it returns finally arrives here
	 * Update the breakpoint with it
	 */
	void SetBreakpointDebuggerID(const int internal_id, const int debugger_id);

	/**
	 * return the number of this line's breakpoints
	 * with the breakpoints themselves in li
	 */
	unsigned int GetBreakpoints(std::vector<BreakpointInfo>& li, const wxString &fileName, const int lineno);

	/**
	 * return whether this line has a breakpoint of type bp_type
	 * Any matches found are returned in li
	 */
	bool GetMatchingBreakpoints(std::vector<BreakpointInfo>& li, const wxString &fileName, const int lineno, enum BP_type bp_type);

	/**
	 * Clears the debugger_ids of all breakpoints.
	 * Called when the debugger has stopped, so they're  no longer valid
	 */
	void ClearBP_debugger_ids();

	/**
	 * remove all breakpoints
	 */
	void DelAllBreakpoints();

	/**
	 * The 'edit a breakpoint' dialog
	 */
	void EditBreakpoint(int index, bool &bpExist);

	/**
	 * If the debugger is running but can't interact, pause it.
	 * Return true if a pause was done, to flag that a restart is needed
	 */
	bool PauseDebuggerIfNeeded();

	/**
	 * Get a unique id for a breakpoint, to use when the debugger isn't running
	 */
	int GetNextID() { return ++NextInternalID; }
};
#endif //BREAKPOINTS_MANAGER_H
