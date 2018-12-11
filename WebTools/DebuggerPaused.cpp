#include "CallFrame.h"
#include "DebuggerPaused.h"
#include "NodeJSEvents.h"
#include "event_notifier.h"
#include <vector>

DebuggerPaused::DebuggerPaused()
    : NodeMessageBase("Debugger.paused")
{
}

DebuggerPaused::~DebuggerPaused() {}

void DebuggerPaused::Process(clWebSocketClient& socket, const JSONElement& json)
{
    m_stopReason = json.namedObject("reason").toString();
    JSONElement frames = json.namedObject("callFrames");
    nSerializableObject::Vec_t V;
    int size = frames.arraySize();
    for(int i = 0; i < size; ++i) {
        JSONElement frame = frames.arrayItem(i);
        nSerializableObject::Ptr_t f(new CallFrame());
        f->FromJSON(frame);
        V.push_back(f);
    }

    // Notify the UI that the debugger paused
    clDebugEvent pauseEvent(wxEVT_NODEJS_DEBUGGER_INTERACT);
    pauseEvent.SetString(m_stopReason);
    pauseEvent.SetAnswer(true);
    EventNotifier::Get()->ProcessEvent(pauseEvent);

    // Notify the UI to update the view
    clDebugCallFramesEvent callFrameEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK);
    callFrameEvent.SetCallFrames(V);
    EventNotifier::Get()->ProcessEvent(callFrameEvent);
}

NodeMessageBase::Ptr_t DebuggerPaused::Clone() { return NodeMessageBase::Ptr_t(new DebuggerPaused()); }
