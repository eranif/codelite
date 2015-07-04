#include "NodeJSContinueHandler.h"
#include "json_node.h"
#include "NodeJSDebugger.h"

NodeJSContinueHandler::NodeJSContinueHandler()
{
}

NodeJSContinueHandler::~NodeJSContinueHandler()
{
}

void NodeJSContinueHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    wxUnusedVar(debugger);
    wxUnusedVar(output);
}
