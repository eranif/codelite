#ifndef NODEJSCONTINUEHANDLER_H
#define NODEJSCONTINUEHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSContinueHandler : public NodeJSHandlerBase
{
public:
    NodeJSContinueHandler();
    virtual ~NodeJSContinueHandler();
public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSCONTINUEHANDLER_H
