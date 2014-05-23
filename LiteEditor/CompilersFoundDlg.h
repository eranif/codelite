#ifndef COMPILERSFOUNDDLG_H
#define COMPILERSFOUNDDLG_H
#include "CompilersFoundDlgBase.h"
#include <ICompilerLocator.h>

class CompilersFoundDlg : public CompilersFoundDlgBase
{
public:
    CompilersFoundDlg(wxWindow* parent, const ICompilerLocator::CompilerVec_t& compilers);
    virtual ~CompilersFoundDlg();
};
#endif // COMPILERSFOUNDDLG_H
