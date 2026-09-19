#ifndef COMPILERLOCATOREOSCDT_H
#define COMPILERLOCATOREOSCDT_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK CompilerLocatorEosCDT : public ICompilerLocator
{
protected:
    bool CheckExists(const wxString& path, wxString& foundPath) const;
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& path, const wxString& args = "") const;
    CompilerPtr CreateCompiler(const wxString& path) const;

public:
    CompilerLocatorEosCDT() = default;
    ~CompilerLocatorEosCDT() override = default;

public:
    bool Locate() override;
    CompilerPtr Locate(const wxString& folder) override;
};

#endif // COMPILERLOCATOREOSCDT_H
