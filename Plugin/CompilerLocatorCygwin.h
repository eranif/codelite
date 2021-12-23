//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorCygwin.h
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

#ifndef COMPILERLOCATORCYGWIN_H
#define COMPILERLOCATORCYGWIN_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK CompilerLocatorCygwin : public ICompilerLocator
{
public:
    CompilerLocatorCygwin();
    virtual ~CompilerLocatorCygwin() {}
    wxString GetGCCVersion(const wxString& gccBinary);

    /**
     * @brief return an array of suffixes for the gcc executable
     * Under Cygwin, the 'gcc.exe' is a symlink which pointer to either
     * gcc-3.exe or gcc-4.exe (and in the future gcc-5.exe)
     */
    wxArrayString GetSuffixes(const wxString& binFolder);

private:
    void AddTools(const wxString& binFolder, const wxString& name, const wxString& suffix);
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                 const wxString& extraArgs = "");

public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

#endif // COMPILERLOCATORCYGWIN_H
