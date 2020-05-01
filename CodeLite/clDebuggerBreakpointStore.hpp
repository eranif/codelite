#ifndef CLDEBUGGERBREAKPOINTSTORE_HPP
#define CLDEBUGGERBREAKPOINTSTORE_HPP

#include "clDebuggerBreakpoint.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clDebuggerBreakpointStore
{
    clDebuggerBreakpoint::Vec_t m_breakpoints;

public:
    clDebuggerBreakpointStore();
    virtual ~clDebuggerBreakpointStore();

    /**
     * @brief store the breakpoints into a file
     */
    void Save(const wxFileName& filename);

    /**
     * @brief load list of breakpoints from the file system
     */
    void Load(const wxFileName& filename);

    // access methods
    void SetBreakpoints(const clDebuggerBreakpoint::Vec_t& breakpoints) { this->m_breakpoints = breakpoints; }
    const clDebuggerBreakpoint::Vec_t& GetBreakpoints() const { return m_breakpoints; }
    clDebuggerBreakpoint::Vec_t& GetBreakpoints() { return m_breakpoints; }
};

#endif // CLDEBUGGERBREAKPOINTSTORE_HPP
