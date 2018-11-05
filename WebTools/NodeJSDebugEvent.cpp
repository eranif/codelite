#include "NodeJSDebugEvent.h"

// ------------------------------------------------
// NodeCLIDebuggerEvent
// ------------------------------------------------

NodeJSDebugEvent::NodeJSDebugEvent(const NodeJSDebugEvent& event) { *this = event; }

NodeJSDebugEvent::NodeJSDebugEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

NodeJSDebugEvent::~NodeJSDebugEvent() {}

NodeJSDebugEvent& NodeJSDebugEvent::operator=(const NodeJSDebugEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    m_callFrames = src.m_callFrames;
    return *this;
}
