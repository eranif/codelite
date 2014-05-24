#ifndef COMPILERLOCATORMSVC_H
#define COMPILERLOCATORMSVC_H

#include <CompilerLocatorMSVCBase.h>

/// Visual Studio 2010 (VC10)
class CompilerLocatorMSVC10 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC10();
    virtual ~CompilerLocatorMSVC10();

public:
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC_H
