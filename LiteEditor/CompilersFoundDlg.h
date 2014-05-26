#ifndef COMPILERSFOUNDDLG_H
#define COMPILERSFOUNDDLG_H
#include "CompilersFoundDlgBase.h"
#include <ICompilerLocator.h>

class CompilersFoundDlg : public CompilersFoundDlgBase
{
    std::map<wxString, CompilerPtr> m_defaultCompilers;
    ICompilerLocator::CompilerVec_t m_allCompilers;

private:
    /**
     * @brief under MSW, update the toolchain (make) for VC and Clang compilers
     * using the active MinGW compiler
     */
    void MSWUpdateToolchain(CompilerPtr compiler);
    
public:
    CompilersFoundDlg(wxWindow* parent, const ICompilerLocator::CompilerVec_t& compilers);
    virtual ~CompilersFoundDlg();
    
    CompilerPtr GetCompiler(const wxDataViewItem& item) const;
    bool IsDefaultCompiler(CompilerPtr compiler) const;
    
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
};
#endif // COMPILERSFOUNDDLG_H
