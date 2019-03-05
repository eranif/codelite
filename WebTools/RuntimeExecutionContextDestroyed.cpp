#include "NodeJSEvents.h"
#include "RuntimeExecutionContextDestroyed.h"
#include "cl_command_event.h"
#include "event_notifier.h"

RuntimeExecutionContextDestroyed::RuntimeExecutionContextDestroyed()
    : NodeMessageBase("Runtime.executionContextDestroyed")
{
}

RuntimeExecutionContextDestroyed::~RuntimeExecutionContextDestroyed() {}

NodeMessageBase::Ptr_t RuntimeExecutionContextDestroyed::Clone()
{
    return NodeMessageBase::Ptr_t(new RuntimeExecutionContextDestroyed());
}

void RuntimeExecutionContextDestroyed::Process(clWebSocketClient& socket, const JSONItem& json)
{
    clDebugEvent eventFinish(wxEVT_NODEJS_DEBUGGER_FINISHED);
    EventNotifier::Get()->AddPendingEvent(eventFinish);
}
