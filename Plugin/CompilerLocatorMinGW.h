#ifndef COMPILERLOCATORMINGW_H
#define COMPILERLOCATORMINGW_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator
#include <macros.h>

class WXDLLIMPEXP_SDK CompilerLocatorMinGW : public ICompilerLocator
{
    wxStringSet_t m_locatedFolders;
    
protected:
    void AddTools(const wxString &binFolder, const wxString &name = "");
    void AddTool(CompilerPtr compiler, const wxString &toolname, const wxString &toolpath, const wxString &extraArgs = "");
    wxString FindBinFolder(const wxString &parentPath);
    wxString GetGCCVersion(const wxString &gccBinary);
    
public:
    CompilerLocatorMinGW();
    virtual ~CompilerLocatorMinGW();

public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORMINGW_H
