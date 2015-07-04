#include "NodeJSCallstackHandler.h"
#include "NodeJSDebugger.h"
#include "NodeJSEvents.h"
#include "event_notifier.h"

NodeJSCallstackHandler::NodeJSCallstackHandler()
{
}

NodeJSCallstackHandler::~NodeJSCallstackHandler()
{
}

void NodeJSCallstackHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK);
    event.SetString(output);
    EventNotifier::Get()->ProcessEvent(event);
}
