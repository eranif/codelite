#ifndef NOTEJSWORKSPACE_H
#define NOTEJSWORKSPACE_H

#include "IWorkspace.h"

class NodeJSWorkspace : public IWorkspace
{
protected:
    static NodeJSWorkspace* ms_workspace;
    
public:
    NodeJSWorkspace();
    virtual ~NodeJSWorkspace();

public:
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;
    
    static NodeJSWorkspace* Get();
    static void Free();
};

#endif // NOTEJSWORKSPACE_H
