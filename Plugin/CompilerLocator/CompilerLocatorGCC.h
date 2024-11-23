//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorGCC.h
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

#pragma once

#include "ICompilerLocator.h" // Base class: ICompilerLocator

#include <wx/filename.h>

class CompilerLocatorGCC : public ICompilerLocator
{
public:
    CompilerLocatorGCC();
    virtual ~CompilerLocatorGCC();
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);

protected:
    void AddTools(CompilerPtr compiler, const wxFileName& gcc);
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxFileName& toolpath,
                 const wxString& extraArgs = "");
};
