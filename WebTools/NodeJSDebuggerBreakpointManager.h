//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSDebuggerBreakpointManager.h
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

#ifndef NODEJSDEBUGGERBREAKPOINTMANAGER_H
#define NODEJSDEBUGGERBREAKPOINTMANAGER_H

#include "NodeJSDebuggerBreakpoint.h"
#include <wx/event.h>
#include "cl_command_event.h"

class IEditor;
class NodeJSBptManager : public wxEvtHandler
{
    NodeJSBreakpoint::Vec_t m_breakpoints;
    wxFileName m_workspaceFile;

protected:
    void OnEditorChanged(wxCommandEvent& e);
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnFileSaved(clCommandEvent& event);

public:
    NodeJSBptManager();
    virtual ~NodeJSBptManager();
    void Save();

    /**
     * @brief return breakpoints for a given file
     */
    size_t GetBreakpointsForFile(const wxString& filename, NodeJSBreakpoint::Vec_t& bps) const;

    /**
     * @brief set all breakpoints for the given file
     */
    void SetBreakpoints(IEditor* editor);
    /**
     * @brief do we have a breakpoint for a given file and line?
     */
    bool HasBreakpoint(const wxFileName& filename, int line) const;
    /**
     * @brief get the breakpoint for a give file and line
     */
    const NodeJSBreakpoint& GetBreakpoint(const wxFileName& filename, int line) const;
    NodeJSBreakpoint& GetBreakpoint(const wxFileName& filename, int line);
    /**
     * @brief delete a breakpoint by file and line
     */
    void DeleteBreakpoint(const wxFileName& filename, int line);
    /**
     * @brief add breakpoint by file and line
     */
    void AddBreakpoint(const wxFileName& filename, int line);

    const NodeJSBreakpoint::Vec_t& GetBreakpoints() const { return m_breakpoints; }
    
    /**
     * @brief remove all breakpoints from both the manager + UI
     */
    void DeleteAll();
    /**
     * @brief remove a single breakpoint by its ID
     */
    void DeleteByID(const wxString& bpid);
    
    /**
     * @brief return list of all applied breakpoints
     */
    wxArrayString GetAllAppliedBreakpoints() const;
};

#endif // NODEJSDEBUGGERBREAKPOINTMANAGER_H
