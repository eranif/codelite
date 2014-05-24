#ifndef COMPILERLOCATORMSVC_H
#define COMPILERLOCATORMSVC_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorMSVC : public ICompilerLocator
{
public:
    CompilerLocatorMSVC();
    virtual ~CompilerLocatorMSVC();

public:
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC_H
