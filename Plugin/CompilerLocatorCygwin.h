#ifndef COMPILERLOCATORCYGWIN_H
#define COMPILERLOCATORCYGWIN_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorCygwin : public ICompilerLocator
{
public:
    CompilerLocatorCygwin();
    virtual ~CompilerLocatorCygwin(){}
    wxString GetGCCVersion(const wxString& gccBinary);
    
    /**
     * @brief return an array of suffixes for the gcc executable
     * Under Cygwin, the 'gcc.exe' is a symlink which pointer to either
     * gcc-3.exe or gcc-4.exe (and in the future gcc-5.exe)
     */
    wxArrayString GetSuffixes(const wxString &binFolder);
    
private:
    void AddTools(const wxString& binFolder, const wxString& name, const wxString &suffix);
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs = "");
    
public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORCYGWIN_H
