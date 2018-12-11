#ifndef NODECLIDEBUGGEREVENT_H
#define NODECLIDEBUGGEREVENT_H

#include "nSerializableObject.h"
#include "cl_command_event.h"

class clDebugCallFramesEvent : public clDebugEvent
{
    nSerializableObject::Vec_t m_callFrames;

public:
    clDebugCallFramesEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clDebugCallFramesEvent(const clDebugCallFramesEvent& event);
    clDebugCallFramesEvent& operator=(const clDebugCallFramesEvent& src);
    nSerializableObject::Vec_t& GetCallFrames() { return m_callFrames; }
    const nSerializableObject::Vec_t& GetCallFrames() const { return m_callFrames; }
    void SetCallFrames(const nSerializableObject::Vec_t& callFrames) { m_callFrames = callFrames; }
    virtual ~clDebugCallFramesEvent();
    virtual wxEvent* Clone() const { return new clDebugCallFramesEvent(*this); };
};

typedef void (wxEvtHandler::*clDebugCallFramesEventFunction)(clDebugCallFramesEvent&);
#define clDebugCallFramesEventHandler(func) wxEVENT_HANDLER_CAST(clDebugCallFramesEventFunction, func)

#endif // NODECLIDEBUGGEREVENT_H
