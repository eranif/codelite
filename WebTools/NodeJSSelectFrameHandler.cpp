#include "NodeJSSelectFrameHandler.h"
#include "NodeJSEvents.h"
#include "event_notifier.h"
#include "cl_command_event.h"

NodeJSSelectFrameHandler::NodeJSSelectFrameHandler() {}

NodeJSSelectFrameHandler::~NodeJSSelectFrameHandler() {}

void NodeJSSelectFrameHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME);
    event.SetString(output);
    EventNotifier::Get()->AddPendingEvent(event);
}
