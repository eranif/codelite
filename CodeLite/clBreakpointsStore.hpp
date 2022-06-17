#ifndef CLDEBUGGERBREAKPOINTSTORE_HPP
#define CLDEBUGGERBREAKPOINTSTORE_HPP

#include "UIBreakpoint.hpp"
#include "clDebuggerBreakpoint.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wxStringHash.h"

#include <unordered_set>
#include <vector>
#include <wx/filename.h>

class WXDLLIMPEXP_CL clBreakpointsStore : public wxEvtHandler
{
    // breakpoints as set by the user
    std::unordered_map<wxString, std::unordered_set<UIBreakpoint>> m_source_breakpoints;
    std::unordered_set<UIBreakpoint> m_function_breakpoints;

    // is a debug session in progress?
    bool m_debug_session_in_progress = false;
    wxFileName m_breakpoint_file;

private:
    void Clear();

protected:
    void OnBreakpointAdded(clDebugEvent& event);
    void OnBreakpointDeleted(clDebugEvent& event);
    void OnDebugSessionStarted(clDebugEvent& event);
    void OnDebugSessionEnded(clDebugEvent& event);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

public:
    clBreakpointsStore();
    virtual ~clBreakpointsStore();

    /**
     * @brief store the breakpoints into a file
     */
    void Save(const wxFileName& filename);

    /**
     * @brief load list of breakpoints from the file system
     */
    void Load(const wxFileName& filename);
};

#endif // CLDEBUGGERBREAKPOINTSTORE_HPP
