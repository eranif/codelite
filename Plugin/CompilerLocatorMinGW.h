#ifndef COMPILERLOCATORMINGW_H
#define COMPILERLOCATORMINGW_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class WXDLLIMPEXP_SDK CompilerLocatorMinGW : public ICompilerLocator
{
protected:
    void AddTools(CompilerPtr compiler, const wxString &binFolder, const wxString &name = "");
    void AddTool(CompilerPtr compiler, const wxString &toolname, const wxString &toolpath, const wxString &extraArgs = "");
    
public:
    CompilerLocatorMinGW();
    virtual ~CompilerLocatorMinGW();

public:
    virtual bool Locate();
};

#endif // COMPILERLOCATORMINGW_H
