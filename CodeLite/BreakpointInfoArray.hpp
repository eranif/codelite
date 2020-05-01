#ifndef BREAKPOINTINFOARRAY_HPP
#define BREAKPOINTINFOARRAY_HPP

#include "clDebuggerBreakpoint.hpp"
#include "codelite_exports.h"
#include "serialized_object.h"

class WXDLLIMPEXP_CL BreakpointInfoArray : public SerializedObject
{
    clDebuggerBreakpoint::Vec_t m_breakpoints;

public:
    BreakpointInfoArray();
    virtual ~BreakpointInfoArray();
    void SetBreakpoints(const clDebuggerBreakpoint::Vec_t& breakpoints) { this->m_breakpoints = breakpoints; }
    const clDebuggerBreakpoint::Vec_t& GetBreakpoints() const { return m_breakpoints; }
    
    void DeSerialize(Archive& arch) override;
    void Serialize(Archive& arch) override;
};

#endif // BREAKPOINTINFOARRAY_HPP
