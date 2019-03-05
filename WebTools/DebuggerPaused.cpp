#include "CallFrame.h"
#include "DebuggerPaused.h"
#include "NodeJSEvents.h"
#include "event_notifier.h"
#include <vector>
#include "file_logger.h"

DebuggerPaused::DebuggerPaused()
    : NodeMessageBase("Debugger.paused")
{
}

DebuggerPaused::~DebuggerPaused() {}

void DebuggerPaused::Process(clWebSocketClient& socket, const JSONItem& json)
{
    m_stopReason = json.namedObject("reason").toString();
    JSONItem frames = json.namedObject("callFrames");
    nSerializableObject::Vec_t V;
    int size = frames.arraySize();
    for(int i = 0; i < size; ++i) {
        JSONItem frame = frames.arrayItem(i);
        nSerializableObject::Ptr_t f(new CallFrame());
        f->FromJSON(frame);
        V.push_back(f);
    }
    
    wxString extraMessage;
    if(json.hasNamedObject("data")) {
        JSONItem data = json.namedObject("data");
        if(data.hasNamedObject("description")) {
            extraMessage = data.namedObject("description").toString();
        }
    }
    // Notify the UI that the debugger paused
    clDebugEvent pauseEvent(wxEVT_NODEJS_DEBUGGER_INTERACT);
    pauseEvent.SetString(m_stopReason);
    pauseEvent.SetArguments(extraMessage);
    pauseEvent.SetAnswer(true);
    EventNotifier::Get()->ProcessEvent(pauseEvent);

    // Notify the UI to update the view
    clDebugCallFramesEvent callFrameEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK);
    callFrameEvent.SetCallFrames(V);
    EventNotifier::Get()->ProcessEvent(callFrameEvent);
}

NodeMessageBase::Ptr_t DebuggerPaused::Clone() { return NodeMessageBase::Ptr_t(new DebuggerPaused()); }
