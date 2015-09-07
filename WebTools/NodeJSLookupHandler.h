#ifndef NODEJSLOOKUPHANDLER_H
#define NODEJSLOOKUPHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSLookupHandler : public NodeJSHandlerBase
{
public:
    NodeJSLookupHandler();
    virtual ~NodeJSLookupHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSLOOKUPHANDLER_H
