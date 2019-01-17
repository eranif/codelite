#ifndef COMPILERLOCATOREOSCDT_H
#define COMPILERLOCATOREOSCDT_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK CompilerLocatorEosCDT : public ICompilerLocator
{
public:
    CompilerLocatorEosCDT();
    virtual ~CompilerLocatorEosCDT();
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& path, const wxString& args = "");

public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATOREOSCDT_H
