#ifndef COMPILERLOCATORCLANG_H
#define COMPILERLOCATORCLANG_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorCLANG : public ICompilerLocator
{
protected:
    void MSWLocate();
    void AddTools(CompilerPtr compiler, const wxString &installFolder);
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs = "");
    wxString GetClangVersion(const wxString &clangBinary);
    wxString GetCompilerFullName(const wxString &clangBinary);

public:
    CompilerLocatorCLANG();
    virtual ~CompilerLocatorCLANG();

public:
    bool Locate();
    CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORCLANG_H
