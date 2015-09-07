#include "NodeJSLookupHandler.h"
#include "file_logger.h"
#include "event_notifier.h"
#include "NodeJSEvents.h"

NodeJSLookupHandler::NodeJSLookupHandler() {}

NodeJSLookupHandler::~NodeJSLookupHandler() {}

void NodeJSLookupHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    wxUnusedVar(debugger);
    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_LOOKUP);
    event.SetString(output);
    EventNotifier::Get()->ProcessEvent(event);
}
