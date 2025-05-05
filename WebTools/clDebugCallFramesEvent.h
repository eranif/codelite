#ifndef NODECLIDEBUGGEREVENT_H
#define NODECLIDEBUGGEREVENT_H

#include "nSerializableObject.h"
#include "cl_command_event.h"

class clDebugCallFramesEvent : public clDebugEvent
{
    nSerializableObject::Vec_t m_callFrames;

public:
    clDebugCallFramesEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clDebugCallFramesEvent(const clDebugCallFramesEvent&) = default;
    clDebugCallFramesEvent& operator=(const clDebugCallFramesEvent&) = delete;
    nSerializableObject::Vec_t& GetCallFrames() { return m_callFrames; }
    const nSerializableObject::Vec_t& GetCallFrames() const { return m_callFrames; }
    void SetCallFrames(const nSerializableObject::Vec_t& callFrames) { m_callFrames = callFrames; }
    ~clDebugCallFramesEvent() override = default;
    wxEvent* Clone() const override { return new clDebugCallFramesEvent(*this); };
};

using clDebugCallFramesEventFunction = void (wxEvtHandler::*)(clDebugCallFramesEvent&);
#define clDebugCallFramesEventHandler(func) wxEVENT_HANDLER_CAST(clDebugCallFramesEventFunction, func)

#endif // NODECLIDEBUGGEREVENT_H
