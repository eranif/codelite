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

class WXDLLIMPEXP_SDK clBreakpointsStore : public wxEvtHandler
{
    // breakpoints as set by the user
    std::unordered_map<wxString, UIBreakpoint::set_t> m_source_breakpoints;
    std::unordered_map<UIBreakpoint, UIBreakpoint> m_function_breakpoints;

    // is a debug session in progress?
    wxFileName m_breakpoint_file;

private:
    void Clear();

protected:
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

    /**
     * @brief update the breakpoints for a given source
     */
    void SetBreakpoints(const wxString& filepath, const UIBreakpoint::set_t& bps);

    /**
     * @brief associate a source breakpoint with a function breakpoint
     */
    void ResolveFunctionBreakpoint(const UIBreakpoint& func_bp, const UIBreakpoint& bp);

    /**
     * @brief return all breakpoints associated a given file. If no file is provided
     * return all breakpoints
     */
    size_t GetAllSourceBreakpoints(UIBreakpoint::set_t* output, const wxString& filepath = wxEmptyString) const;

    /**
     * @brief return true if a breakpoint is already set for a given file and line
     */
    bool HasSourceBreakpoint(const wxString& path, int lineNumber) const;

    /**
     * @brief add a source breakpoint
     */
    void AddSourceBreakpoint(const wxString& path, int lineNumber);

    /**
     * @brief delete source breakpoint
     * if lineNumber is wxNOT_FOUND, delete all breakpoints for this file
     */
    void DeleteSourceBreakpoint(const wxString& path, int lineNumber);
};

#endif // CLDEBUGGERBREAKPOINTSTORE_HPP
