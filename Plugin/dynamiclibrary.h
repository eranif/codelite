//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dynamiclibrary.h
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
#ifndef __dynamiclibrary__
#define __dynamiclibrary__

#include "codelite_exports.h"
#include "wx/dynlib.h"
#include "wx/string.h"

class WXDLLIMPEXP_SDK clDynamicLibrary
{

#if defined(__WXMSW__)
    wxDynamicLibrary m_lib;
#else // Mac OSX
    void* m_dllhandle;
#endif
    wxString m_error;

public:
    clDynamicLibrary();
    ~clDynamicLibrary();

    bool Load(const wxString& name);
    void Detach();
    void* GetSymbol(const wxString& name, bool* success);

    wxString GetError() { return m_error; }
};
#endif // __dynamiclibrary__
