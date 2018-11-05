#ifndef NODECLIDEBUGGEREVENT_H
#define NODECLIDEBUGGEREVENT_H

#include "NodeJSONSerialisable.h"
#include "cl_command_event.h"

class NodeJSDebugEvent : public clCommandEvent
{
    clJSONObject::Vec_t m_callFrames;

public:
    NodeJSDebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    NodeJSDebugEvent(const NodeJSDebugEvent& event);
    NodeJSDebugEvent& operator=(const NodeJSDebugEvent& src);
    clJSONObject::Vec_t& GetCallFrames() { return m_callFrames; }
    const clJSONObject::Vec_t& GetCallFrames() const { return m_callFrames; }
    void SetCallFrames(const clJSONObject::Vec_t& callFrames) { m_callFrames = callFrames; }
    virtual ~NodeJSDebugEvent();
    virtual wxEvent* Clone() const { return new NodeJSDebugEvent(*this); };
};

typedef void (wxEvtHandler::*NodeJSDebugEventNewFunction)(NodeJSDebugEvent&);
#define NodeJSDebugEventNewHandler(func) wxEVENT_HANDLER_CAST(NodeJSDebugEventNewFunction, func)

#endif // NODECLIDEBUGGEREVENT_H
