#ifndef BREAKPOINTSHELPER_HPP
#define BREAKPOINTSHELPER_HPP

#include "DebugSession.hpp"
#include "clModuleLogger.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "dap/Client.hpp"
#include "dap/dap.hpp"
#include "globals.h"
#include "imanager.h"

#include <unordered_map>
#include <unordered_set>

class BreakpointsHelper : public wxEvtHandler
{
    std::unordered_map<wxString, clDebuggerBreakpoint::Vec_t> m_ui_breakpoints;
    dap::Client& m_client;
    const DebugSession& m_session;
    clModuleLogger& LOG;

protected:
    void OnToggleBreakpoint(clDebugEvent& event);
    wxString NormalisePathForSend(const wxString& path) const;

public:
    BreakpointsHelper(dap::Client& client, const DebugSession& session, clModuleLogger& log);
    virtual ~BreakpointsHelper();
    /// apply breakpoints. if path is empty, apply all breakpoints, to all the sources
    void ApplyBreakpoints(const wxString& path);
};

#endif // BREAKPOINTSHELPER_HPP
