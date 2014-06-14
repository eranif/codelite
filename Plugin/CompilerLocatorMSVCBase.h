#ifndef COMPILERLOCATORMSVCBASE_H
#define COMPILERLOCATORMSVCBASE_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorMSVCBase : public ICompilerLocator
{
public:
    CompilerLocatorMSVCBase();
    virtual ~CompilerLocatorMSVCBase();
    virtual CompilerPtr Locate(const wxString &folder) {
        return NULL;
    }
    
protected:
    void AddTools(const wxString &masterFolder, const wxString &name);
    void AddTool(const wxString &toolpath, const wxString &extraArgs, const wxString &toolname, CompilerPtr compiler);
    void FindSDKs(CompilerPtr compiler);
    void AddIncOrLibPath(const wxString& path_to_add, wxString &add_to_me);
};

#endif // COMPILERLOCATORMSVCBASE_H
