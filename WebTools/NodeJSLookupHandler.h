#ifndef NODEJSLOOKUPHANDLER_H
#define NODEJSLOOKUPHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase
#include "NodeJS.h"

class NodeJSLookupHandler : public NodeJSHandlerBase
{
    eNodeJSContext m_context;
public:
    NodeJSLookupHandler(eNodeJSContext context);
    virtual ~NodeJSLookupHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSLOOKUPHANDLER_H
