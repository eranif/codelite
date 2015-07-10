#include "NodeJSEvaluateExprHandler.h"
#include <imanager.h>
#include "globals.h"
#include "json_node.h"
#include "ieditor.h"
#include <wx/stc/stc.h>


NodeJSEvaluateExprHandler::NodeJSEvaluateExprHandler(const wxString& expression)
    : m_expression(expression)
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
        
        wxString expression = clGetManager()->GetActiveEditor()->GetWordAtCaret();
        if(type == "string") {
            content.Prepend("\"").Append("\"");
        }
        clGetManager()->GetActiveEditor()->ShowRichTooltip(content, title);
    }
}
