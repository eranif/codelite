#ifndef RUNTIMEEXECUTIONCONTEXTDESTROYED_H
#define RUNTIMEEXECUTIONCONTEXTDESTROYED_H

#include "NodeMessageBase.h" // Base class: NodeMessageBase

class RuntimeExecutionContextDestroyed : public NodeMessageBase
{
public:
    RuntimeExecutionContextDestroyed();
    virtual ~RuntimeExecutionContextDestroyed();

public:
    virtual NodeMessageBase::Ptr_t Clone();
    virtual void Process(const JSONElement& json);
};

#endif // RUNTIMEEXECUTIONCONTEXTDESTROYED_H
