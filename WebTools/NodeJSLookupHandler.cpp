#include "NodeJSLookupHandler.h"
#include "file_logger.h"
#include "event_notifier.h"
#include "NodeJSEvents.h"

NodeJSLookupHandler::NodeJSLookupHandler(eNodeJSContext context)
    : m_context(context)
{
}

NodeJSLookupHandler::~NodeJSLookupHandler() {}

void NodeJSLookupHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    wxUnusedVar(debugger);
    if(m_context == kNodeJSContextLocals) {
        clDebugEvent event(wxEVT_NODEJS_DEBUGGER_LOCALS_LOOKUP);
        event.SetString(output);
        EventNotifier::Get()->ProcessEvent(event);
    } else {
        clDebugEvent event(wxEVT_NODEJS_DEBUGGER_TOOLTIP_LOOKUP);
        event.SetString(output);
        EventNotifier::Get()->ProcessEvent(event);
    }
}
