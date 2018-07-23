#include "NodeJSEvaluateExprHandler.h"
#include <imanager.h>
#include "globals.h"
#include "json_node.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "NodeJSEvents.h"
#include "event_notifier.h"
#include "cl_command_event.h"
#include "NodeJSDebugger.h"

NodeJSEvaluateExprHandler::NodeJSEvaluateExprHandler(const wxString& expression, eNodeJSContext context)
    : m_expression(expression)
    , m_context(context)
{
}

NodeJSEvaluateExprHandler::~NodeJSEvaluateExprHandler() {}

void NodeJSEvaluateExprHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    JSONRoot root(output);
    JSONElement json = root.toElement();
    if(json.namedObject("body").hasNamedObject("text") && clGetManager()->GetActiveEditor()) {
        wxString title = m_expression;
        wxString content = json.namedObject("body").namedObject("text").toString();
        wxString type = json.namedObject("body").namedObject("type").toString();

        if(type == "string") {
            content.Prepend("\"").Append("\"");
        }
        if(m_context == kNodeJSContextTooltip) {
            debugger->ShowTooltip(m_expression, output);
            
        } else {
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_EXPRESSION_EVALUATED);
            event.SetString(content);
            EventNotifier::Get()->ProcessEvent(event);
        }
    } else if(json.hasNamedObject("success") && !json.namedObject("success").toBool()) {
        clDebugEvent event(wxEVT_NODEJS_DEBUGGER_EXPRESSION_EVALUATED);
        event.SetString(json.namedObject("message").toString());
        EventNotifier::Get()->ProcessEvent(event);
    }
}
