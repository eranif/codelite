#ifndef COMPILERLOCATORMSVC9_H
#define COMPILERLOCATORMSVC9_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

/// Visual Studio 2008 (VC9)
class CompilerLocatorMSVC9 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC9();
    virtual ~CompilerLocatorMSVC9();
    
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC9_H
