//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorMSVCBase.h
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

#ifndef COMPILERLOCATORMSVCBASE_H
#define COMPILERLOCATORMSVCBASE_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorMSVCBase : public ICompilerLocator
{
public:
    CompilerLocatorMSVCBase();
    virtual ~CompilerLocatorMSVCBase();
    virtual CompilerPtr Locate(const wxString &folder) {
        return NULL;
    }
    
protected:
    void AddTools(const wxString &masterFolder, const wxString &name);
    void AddTool(const wxString &toolpath, const wxString &extraArgs, const wxString &toolname, CompilerPtr compiler);
    void FindSDKs(CompilerPtr compiler);
    void AddIncOrLibPath(const wxString& path_to_add, wxString &add_to_me);
};

#endif // COMPILERLOCATORMSVCBASE_H
