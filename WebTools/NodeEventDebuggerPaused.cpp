#include "CallFrame.h"
#include "NodeEventDebuggerPaused.h"
#include "NodeJSEvents.h"
#include "event_notifier.h"
#include <vector>

NodeEventDebuggerPaused::NodeEventDebuggerPaused()
    : NodeDbgEventBase("Debugger.paused")
{
}

NodeEventDebuggerPaused::~NodeEventDebuggerPaused() {}

void NodeEventDebuggerPaused::Process(const JSONElement& json)
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
    clDebugEvent pauseEvent(wxEVT_NODEJS_DEBUGGER_CAN_INTERACT);
    pauseEvent.SetString(m_stopReason);
    EventNotifier::Get()->ProcessEvent(pauseEvent);

    // Notify the UI to update the view
    NodeJSDebugEvent callFrameEvent(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK);
    callFrameEvent.SetCallFrames(V);
    EventNotifier::Get()->ProcessEvent(callFrameEvent);
}

NodeDbgEventBase::Ptr_t NodeEventDebuggerPaused::Clone()
{
    return NodeDbgEventBase::Ptr_t(new NodeEventDebuggerPaused());
}
