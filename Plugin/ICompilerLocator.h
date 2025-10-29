//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ICompilerLocator.h
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

#ifndef ICOMPILERLOCATOR_H
#define ICOMPILERLOCATOR_H

#include "codelite_exports.h"
#include "compiler.h"

#include <memory>
#include <vector>

#define COMPILER_FAMILY_CLANG "LLVM/Clang"
#define COMPILER_FAMILY_GCC "GCC"
#define COMPILER_FAMILY_VC "Visual C++"
#define COMPILER_FAMILY_CYGWIN "Cygwin"
#define COMPILER_FAMILY_MSYS2 "MSYS2/GCC"
#define COMPILER_FAMILY_MINGW COMPILER_FAMILY_MSYS2

#ifdef __WXMSW__
#define COMPILER_DEFAULT_FAMILY COMPILER_FAMILY_MINGW
#elif defined(__WXMAC__)
#define COMPILER_DEFAULT_FAMILY COMPILER_FAMILY_CLANG
#else
#define COMPILER_DEFAULT_FAMILY COMPILER_FAMILY_GCC
#endif

class WXDLLIMPEXP_SDK ICompilerLocator
{
public:
    using Ptr_t = std::shared_ptr<ICompilerLocator>;
    using Vect_t = std::vector<ICompilerLocator::Ptr_t>;
    using CompilerVec_t = std::vector<CompilerPtr>;

protected:
    ICompilerLocator::CompilerVec_t m_compilers;
    wxArrayString GetPaths() const;

    /**
     * @brief windows only: scan registry for uninstall information
     */
    void ScanUninstRegKeys();
    virtual void CheckUninstRegKey(const wxString& displayName, const wxString& installFolder,
                                   const wxString& displayVersion)
    {
        wxUnusedVar(displayName);
        wxUnusedVar(installFolder);
        wxUnusedVar(displayVersion);
    }

public:
    ICompilerLocator() = default;
    virtual ~ICompilerLocator() = default;

    /**
     * @brief locate the compiler
     */
    virtual bool Locate() = 0;

    /**
     * @brief locate a compiler in a directory
     */
    virtual CompilerPtr Locate(const wxString& folder) = 0;

    /**
     * @brief return the compiler
     */
    const ICompilerLocator::CompilerVec_t& GetCompilers() const { return m_compilers; }
};

#endif // ICOMPILERLOCATOR_H
