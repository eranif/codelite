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
#include "sessionmanager.h"
#include "wx/arrstr.h"
#include "wx/dragimag.h"

class myDragImage;

#define FIRST_INTERNAL_ID 10000

class BreakptMgr
{
	std::vector<BreakpointInfo> m_bps;        // The vector of breakpoints
	std::vector<BreakpointInfo> m_PendingBPs; // These are any breakpoints that the debugger won't (yet) accept (often because they're in a plugin)

	int NextInternalID;		// Used to give each bp a unique internal ID. Start at 10k to avoid confusion with gdb's IDs

	myDragImage* m_dragImage;

	// Delete all breakpoint markers for this file, then re-mark with the currently-correct marker
	void DoRefreshFileBreakpoints(LEditor* editor);
	void DoProvideBestBP_Type(LEditor* editor, const std::vector<BreakpointInfo>& li);	// Tells the editor which is the most appropriate bp marker to show

	// Delete all line-type breakpoint markers in all editors
	// Done before refreshing after a delete, lest it was the last bp in a file
	void DeleteAllBreakpointMarkers();
	std::set<wxString> GetFilesWithBreakpointMarkers();

	/**
	 * Return the index of the bp with the passed id, in the vector that will normally be m_bps
	 */
	int FindBreakpointById(const int id, const std::vector<BreakpointInfo>& li);

	/**
	 * Can gdb accept this alteration, or will be bp have to be replaced?
	 */
	bool CanThisBreakpointBeUpdated(const BreakpointInfo &bp1, const BreakpointInfo &bp2) const;

	/**
	 * Get id of the breakpoint on this line. If multiple bps, ask the user to select
	 */
	int GetDesiredBreakpointIfMultiple(const wxString &fileName, const int lineno, const wxString msg = wxT(""));

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
	 * @brief refresh the breakpoints marker for a given editor
	 * @param editor
	 */
	void RefreshBreakpointsForEditor(LEditor* editor);

	/**
	 * Refresh all line-type breakpoint markers in all editors
	*/
	void RefreshBreakpointMarkers();

	/**
	 * @brief add list of breakpoints to the stored breakpoints list
	 * @param bps
	 */
	void SetBreakpoints(const std::vector<BreakpointInfo>& bps);

	/**
	 * @brief Store list of breakpoints in the pending-breakpoints list
	 * @param bps
	 */
	void SetPendingBreakpoints(const std::vector<BreakpointInfo>& bps) {
		m_PendingBPs.clear(); m_PendingBPs = bps;
	}

	/**
	 * @brief Returns true if there are pending breakpoints
	 */
	bool PendingBreakpointsExist() {
		return ! m_PendingBPs.empty();
	}

	/**
	 * Send again to the debugger any breakpoints that weren't accepted by the debugger the first time
	 *  (e.g. because they're inside a plugin)
	 */
	void ApplyPendingBreakpoints();

	/**
	 * @brief delete all stored breakpoints related to file. this method should does not update the
	 * debugger, so it must be called *before* starting the debugger
	 * @param fileName
	 */
	void DeleteAllBreakpointsByFileName(const wxString &fileName);

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
	bool AddBreakpoint(const BreakpointInfo &bp);

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
	 * Returns a string containing details of any breakpoints on this line
	 */
	wxString GetTooltip(const wxString& fileName, const int lineno);

	/**
	 * Update the m_bps with what the debugger really contains
	 * from vector of breakpoints acquired from -break-list
	 */
	void ReconcileBreakpoints(const std::vector<BreakpointInfo>& li);

	/**
	 * Clears the debugger_ids of all breakpoints.
	 * Called when the debugger has stopped, so they're  no longer valid
	 */
	void ClearBP_debugger_ids();

	/**
	 * Since a bp can't be created disabled, enable them all here when the debugger stops
	 * That way they're guaranteed all to be enabled when it starts again
	 */
	void UnDisableAllBreakpoints();

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
	 * Notification from the debugger that breakpoint id was just hit
	 */
	void BreakpointHit(int id);

	/**
	 * Starts 'drag'n'drop' for breakpoints
	 */
	void DragBreakpoint(LEditor* editor, int line, wxBitmap bitmap);

	/**
	 * The 'drop' bit of breakpoints 'drag'n'drop'
	 */
	void DropBreakpoint(std::vector<BreakpointInfo>& BPs, int newline);

	/**
	 * Getter for the myDragImage pointer
	 */
	myDragImage* GetDragImage() {
		return m_dragImage;
	}

	/**
	 * Get a unique id for a breakpoint, to use when the debugger isn't running
	 */
	int GetNextID() { return ++NextInternalID; }

	/**
	 * Save session
	 */
	 void SaveSession(SessionEntry& session);

	/**
	 * Load session
	 */
	 void LoadSession(const SessionEntry& session);
};

class myDragImage  :  public wxDragImage, public wxEvtHandler
{
	LEditor* editor;
	wxBitmap bitmap;
	std::vector<BreakpointInfo> lineBPs;
	int m_startx; // The initial x position
	wxCursor oldcursor;

public:
	myDragImage(LEditor* ed, wxBitmap bitmap, std::vector<BreakpointInfo>& BPs);
	bool StartDrag();
	void OnMotion(wxMouseEvent& event);
	void OnEndDrag(wxMouseEvent& event);

};
#endif //BREAKPOINTS_MANAGER_H
