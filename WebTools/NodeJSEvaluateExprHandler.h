#ifndef NODEJSEVALUATEEXPRHANDLER_H
#define NODEJSEVALUATEEXPRHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase
#include "NodeJS.h"

class NodeJSEvaluateExprHandler : public NodeJSHandlerBase
{
    wxString m_expression;
    eNodeJSContext m_context;

public:
    NodeJSEvaluateExprHandler(const wxString& expression, eNodeJSContext context = kNodeJSContextTooltip);
    virtual ~NodeJSEvaluateExprHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSEVALUATEEXPRHANDLER_H
