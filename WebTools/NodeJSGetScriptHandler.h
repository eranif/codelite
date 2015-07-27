#ifndef NODEJSGETSCRIPTHANDLER_H
#define NODEJSGETSCRIPTHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSGetScriptHandler : public NodeJSHandlerBase
{
    wxString m_filename;
    int m_line;
    
public:
    NodeJSGetScriptHandler(const wxString& filename, int line);
    virtual ~NodeJSGetScriptHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSGETSCRIPTHANDLER_H
