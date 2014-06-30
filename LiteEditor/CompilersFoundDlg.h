//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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
