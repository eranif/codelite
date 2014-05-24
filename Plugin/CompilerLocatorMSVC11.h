#ifndef COMPILERLOCATORMSVC11_H
#define COMPILERLOCATORMSVC11_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

class CompilerLocatorMSVC11 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC11();
    virtual ~CompilerLocatorMSVC11();
    
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC11_H
