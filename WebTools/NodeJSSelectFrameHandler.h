#ifndef NODEJSSELECTFRAMEHANDLER_H
#define NODEJSSELECTFRAMEHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSSelectFrameHandler : public NodeJSHandlerBase
{
public:
    NodeJSSelectFrameHandler();
    virtual ~NodeJSSelectFrameHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSSELECTFRAMEHANDLER_H
