#include "DebuggerResumed.h"
#include "NodeJSEvents.h"
#include "cl_command_event.h"
#include "event_notifier.h"

DebuggerResumed::DebuggerResumed()
    : NodeMessageBase("Debugger.resumed")
{
}

DebuggerResumed::~DebuggerResumed() {}

void DebuggerResumed::Process(clWebSocketClient& socket, const JSONItem& json)
{
    // Notify the UI that the debugger paused
    clDebugEvent resumeEvent(wxEVT_NODEJS_DEBUGGER_INTERACT);
    resumeEvent.SetString("");
    resumeEvent.SetAnswer(false);
    EventNotifier::Get()->ProcessEvent(resumeEvent);
}

NodeMessageBase::Ptr_t DebuggerResumed::Clone() { return NodeMessageBase::Ptr_t(new DebuggerResumed()); }
