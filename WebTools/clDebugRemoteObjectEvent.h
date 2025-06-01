#ifndef CLDEBUGREMOTEOBJECTEVENT_H
#define CLDEBUGREMOTEOBJECTEVENT_H

#include "RemoteObject.h"
#include "cl_command_event.h"

class clDebugRemoteObjectEvent : public clDebugEvent
{
    nSerializableObject::Ptr_t m_remoteObject;

public:
    clDebugRemoteObjectEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clDebugRemoteObjectEvent(const clDebugRemoteObjectEvent&) = default;
    clDebugRemoteObjectEvent& operator=(const clDebugRemoteObjectEvent&) = delete;
    ~clDebugRemoteObjectEvent() override = default;
    wxEvent* Clone() const override { return new clDebugRemoteObjectEvent(*this); }
    void SetRemoteObject(nSerializableObject::Ptr_t remoteObject) { this->m_remoteObject = remoteObject; }
    nSerializableObject::Ptr_t GetRemoteObject() const { return m_remoteObject; }
};

using clDebugRemoteObjectEventFunction = void (wxEvtHandler::*)(clDebugRemoteObjectEvent&);
#define clDebugRemoteObjectEventHandler(func) wxEVENT_HANDLER_CAST(clDebugRemoteObjectEventFunction, func)

#endif // CLDEBUGREMOTEOBJECTEVENT_H
