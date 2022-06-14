#include "UIBreakpoint.hpp"

UIBreakpoint::UIBreakpoint() {}

UIBreakpoint::~UIBreakpoint() {}

bool UIBreakpoint::SameAs(const UIBreakpoint& other) const
{
    // check the type first
    if(GetType() != other.GetType()) {
        return false;
    }

    switch(GetType()) {
    case UIBreakpointType::INVALID:
        return true;
    case UIBreakpointType::SOURCE:
        return m_file == other.m_file && m_line == other.m_line;
    case UIBreakpointType::FUNCTION:
        return m_function == other.m_function;
    }
    return false; // should not get here
}
