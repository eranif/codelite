#ifndef COMPILERLOCATORMSVC12_H
#define COMPILERLOCATORMSVC12_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

class CompilerLocatorMSVC12 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC12();
    virtual ~CompilerLocatorMSVC12();
    
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC12_H
