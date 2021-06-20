//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilersFoundDlg.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef COMPILERSFOUNDDLG_H
#define COMPILERSFOUNDDLG_H
#include "CompilersFoundDlgBase.h"
#include "ICompilerLocator.h"
#include <ICompilerLocator.h>

class CompilersFoundDlg : public CompilersFoundDlgBase
{
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

    /**
     * @brief return list of compilers to configure
     */
    size_t GetSelectedCompilers(ICompilerLocator::CompilerVec_t& compilers);
};
#endif // COMPILERSFOUNDDLG_H
