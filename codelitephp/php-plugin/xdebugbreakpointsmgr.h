#ifndef XDEBUGBREAKPOINTSMGR_H
#define XDEBUGBREAKPOINTSMGR_H

#include "XDebugBreakpoint.h"
#include <cl_command_event.h>
#include "xdebugevent.h"

class XDebugBreakpointsMgr : public wxEvtHandler
{
    XDebugBreakpoint::List_t m_breakpoints;
    wxString                 m_workspacePath;
protected:
    void Notify();
    void Save();
    
public:
    XDebugBreakpointsMgr();
    virtual ~XDebugBreakpointsMgr();

    bool HasBreakpoint(const wxString &filename, int line) const;
    void AddBreakpoint(const wxString &filename, int line);
    void DeleteBreakpoint(const wxString &filename, int line);
    void DeleteAllBreakpoints();
    
    const XDebugBreakpoint::List_t& GetBreakpoints() const {
        return m_breakpoints;
    }
    XDebugBreakpoint::List_t& GetBreakpoints() {
        return m_breakpoints;
    }
    
    void OnXDebugSessionEnded(XDebugEvent &e);
    void OnXDebugSesstionStarting(XDebugEvent &e);
    void OnWorkspaceOpened(PHPEvent &e);
    void OnWorkspaceClosed(PHPEvent &e);
    void OnEditorChanged(wxCommandEvent &e);
    
    bool GetBreakpoint(const wxString& filename, int line, XDebugBreakpoint& bp);
    bool GetBreakpoint(const wxString& filename, int line, XDebugBreakpoint& bp) const;
    size_t GetBreakpointsForFile(const wxString &filename, XDebugBreakpoint::List_t& bps) const;
};

#endif // XDEBUGBREAKPOINTSMGR_H
