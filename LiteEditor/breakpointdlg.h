//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : breakpointdlg.h
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
#ifndef __breakpointdlg__
#define __breakpointdlg__

#include "breakpointdlgbase.h"
#include "breakpointsmgr.h"
#include "cl_command_event.h"

// Since a breakpoint will always have an internal id, but a valid debugger one only when the debugger's running, store
// both here
struct bpd_IDs {
    double debugger_id;
    double internal_id;
    bpd_IDs(const clDebuggerBreakpoint& bp)
    {
        debugger_id = bp.debugger_id;
        internal_id = bp.internal_id;
    }
    // If the debugger is running, debugger_id will (should) be >0. Otherwise use the internal_id
    double GetBestId() const { return debugger_id == -1 ? internal_id : debugger_id; }

    // Internal IDs start at FIRST_INTERNAL_ID + 1, == 10001
    wxString GetIdAsString() const
    {
        double id = (GetBestId() > FIRST_INTERNAL_ID ? GetBestId() - FIRST_INTERNAL_ID : GetBestId());
        wxString idstr;
        idstr << id;
        return idstr;
    }
};

class BreakpointDlg : public BreakpointTabBase
{
    std::vector<bpd_IDs> m_ids;

protected:
    void OnBreakpointActivated(wxDataViewEvent& event);
    void OnContextMenu(wxDataViewEvent& event);
    void OnAdd(wxCommandEvent& e);
    void OnEdit(wxCommandEvent& e);
    void OnDelete(wxCommandEvent& e);
    void OnDeleteAll(wxCommandEvent& e);
    void OnApplyPending(wxCommandEvent& e);
    void OnBreakpointsUpdated(clDebugEvent& event);

public:
    BreakpointDlg(wxWindow* parent);
    virtual ~BreakpointDlg();
    void Initialize();
};

#endif // __breakpointdlg__
