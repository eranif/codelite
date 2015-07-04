#ifndef NODEJSHANDLERBASE_H
#define NODEJSHANDLERBASE_H

#include "smart_ptr.h"
#include <wx/string.h>

class NodeJSDebugger;
class NodeJSHandlerBase
{
public:
    typedef SmartPtr<NodeJSHandlerBase> Ptr_t;
    
    NodeJSHandlerBase();
    virtual ~NodeJSHandlerBase();
    
    virtual void Process(NodeJSDebugger* debugger, const wxString& output) = 0;
};

#endif // NODEJSHANDLERBASE_H
