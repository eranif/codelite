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

#include <wx/filename.h>

class WXDLLIMPEXP_SDK CompilerLocatorCLANG : public ICompilerLocator
{
public:
    CompilerLocatorCLANG() = default;
    ~CompilerLocatorCLANG() override = default;
    bool Locate() override;
    CompilerPtr Locate(const wxString& folder) override;

protected:
    void AddTools(CompilerPtr compiler, const wxFileName& clang);
    void AddTool(CompilerPtr compiler, const wxString& name, const wxFileName& fntool,
                 const wxString& extraArgs = wxEmptyString);
    wxString GetCompilerFullName(const wxString& clangBinary);
    void CheckUninstRegKey(const wxString& displayName, const wxString& installFolder,
                           const wxString& displayVersion) override;
    CompilerPtr AddCompiler(const wxFileName& clang);
};

#endif // COMPILERLOCATORCLANG_H
