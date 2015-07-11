#ifndef NODEJSEVALUATEEXPRHANDLER_H
#define NODEJSEVALUATEEXPRHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSEvaluateExprHandler : public NodeJSHandlerBase
{
    wxString m_expression;
    int m_context;

public:
    enum {
        kContextTooltip,
        kContextConsole,
    };

public:
    NodeJSEvaluateExprHandler(const wxString& expression, int context = kContextTooltip);
    virtual ~NodeJSEvaluateExprHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSEVALUATEEXPRHANDLER_H
