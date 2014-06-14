#ifndef COMPILERLOCATORGCC_H
#define COMPILERLOCATORGCC_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorGCC : public ICompilerLocator
{
public:
    CompilerLocatorGCC();
    virtual ~CompilerLocatorGCC();

protected:
    void AddTools(CompilerPtr compiler, const wxString &binFolder, const wxString &suffix = "");
    void AddTool(CompilerPtr compiler, const wxString &toolname, const wxString &toolpath, const wxString &suffix, const wxString &extraArgs = "");

public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORGCC_H
