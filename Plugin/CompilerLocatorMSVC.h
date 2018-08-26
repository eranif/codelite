//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorMSVC.h
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

#ifndef COMPILERLOCATORMSVC_H
#define COMPILERLOCATORMSVC_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

//-------------------------------------------------------------------------
// For a complete list of MSVC compilers
// See here: https://en.wikipedia.org/wiki/Microsoft_Visual_Studio#History
//-------------------------------------------------------------------------

class CompilerLocatorMSVC : public ICompilerLocator
{
public:
    CompilerLocatorMSVC();
    virtual ~CompilerLocatorMSVC();
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder) { return NULL; }

protected:
    void AddTools(const wxString& masterFolder, const wxString& name, const wxString& platform);
    void AddTool(const wxString& toolpath, const wxString& extraArgs, const wxString& toolname, CompilerPtr compiler);
    void AddCompilerOptions(CompilerPtr compiler);
    void AddLinkerOptions(CompilerPtr compiler);
};

#endif // COMPILERLOCATORMSVC_H
