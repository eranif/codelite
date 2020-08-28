#include "BreakpointInfoArray.hpp"

BreakpointInfoArray::BreakpointInfoArray() {}

BreakpointInfoArray::~BreakpointInfoArray() {}

void BreakpointInfoArray::DeSerialize(Archive& arch)
{
    size_t bt_count = 0;
    m_breakpoints.clear();
    if(!arch.Read("Count", bt_count))
        return;

    for(size_t i = 0; i < bt_count; i++) {
        wxString name;
        name << "Breakpoint" << i;
        clDebuggerBreakpoint bkpt;
        arch.Read(name, (SerializedObject*)&bkpt);
        m_breakpoints.push_back(bkpt);
    }
}

void BreakpointInfoArray::Serialize(Archive& arch)
{
    arch.Write(wxT("Count"), (size_t)m_breakpoints.size());
    for(size_t i = 0; i < m_breakpoints.size(); i++) {
        wxString name;
        name << "Breakpoint" << i;
        arch.Write(name, (SerializedObject*)&m_breakpoints[i]);
    }
}
