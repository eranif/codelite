//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorMinGW.h
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

#ifndef COMPILERLOCATORMINGW_H
#define COMPILERLOCATORMINGW_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

#include <macros.h>

class WXDLLIMPEXP_SDK CompilerLocatorMinGW : public ICompilerLocator
{
    wxStringSet_t m_locatedFolders;

protected:
    void AddTools(const wxString& binFolder, const wxString& name = "");
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                 const wxString& extraArgs = "");
    wxString FindBinFolder(const wxString& parentPath);
    wxString GetGCCVersion(const wxString& gccBinary);
    virtual void CheckUninstRegKey(const wxString& displayName, const wxString& installFolder,
                                   const wxString& displayVersion);

public:
    CompilerLocatorMinGW();
    virtual ~CompilerLocatorMinGW();

public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORMINGW_H
