#include "clDebugRemoteObjectEvent.h"

clDebugRemoteObjectEvent::clDebugRemoteObjectEvent(const clDebugRemoteObjectEvent& event) { *this = event; }

clDebugRemoteObjectEvent::clDebugRemoteObjectEvent(wxEventType commandType, int winid)
    : clDebugEvent(commandType, winid)
{
}

clDebugRemoteObjectEvent::~clDebugRemoteObjectEvent() {}

clDebugRemoteObjectEvent& clDebugRemoteObjectEvent::operator=(const clDebugRemoteObjectEvent& src)
{
    // Call parent operator =
    clDebugEvent::operator=(src);
    m_remoteObject = src.m_remoteObject;
    return *this;
}
