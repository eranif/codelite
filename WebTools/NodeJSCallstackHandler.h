#ifndef NODEJSCALLSTACKHANDLER_H
#define NODEJSCALLSTACKHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSCallstackHandler : public NodeJSHandlerBase
{
public:
    NodeJSCallstackHandler();
    virtual ~NodeJSCallstackHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSCALLSTACKHANDLER_H
