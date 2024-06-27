//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorCLANG.h
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

#ifndef COMPILERLOCATORCLANG_H
#define COMPILERLOCATORCLANG_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator
#include "codelite_exports.h"
#include "compiler.h"
#include "wx/filename.h"

class WXDLLIMPEXP_SDK CompilerLocatorCLANG : public ICompilerLocator
{
    struct MSYS2Env {
        int cpuBits;     // 32bit or 64bit
        wxString prefix; // directory prefix
    };
    std::vector<MSYS2Env> m_msys2Envs;

protected:
    void MSWLocate();
    void AddTools(CompilerPtr compiler, const wxString& installFolder, const wxString& suffix = "");
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                 const wxString& extraArgs = "");
    wxString GetClangVersion(const wxString& clangBinary);
    wxString GetCompilerFullName(const wxString& clangBinary);
    bool ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion);
    virtual void CheckUninstRegKey(const wxString& displayName, const wxString& installFolder,
                                   const wxString& displayVersion);
    CompilerPtr AddCompiler(const wxString& clangFolder, const wxString& name = "", const wxString& suffix = "");

public:
    CompilerLocatorCLANG();
    virtual ~CompilerLocatorCLANG();

public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORCLANG_H
