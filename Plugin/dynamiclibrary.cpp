//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dynamiclibrary.cpp
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
#include "dynamiclibrary.h"

#include "precompiled_header.h"

#if defined(__WXMAC__) || defined(__WXGTK__)
#include <dlfcn.h>
#endif

clDynamicLibrary::clDynamicLibrary()
#if defined(__WXMAC__) || defined(__WXGTK__)
    : m_dllhandle(NULL)
#endif
{
}

clDynamicLibrary::~clDynamicLibrary()
{
#if defined(__WXMAC__) || defined(__WXGTK__)
    if(m_dllhandle) {
        dlclose(m_dllhandle);
        m_dllhandle = NULL;
    }
#endif
}

bool clDynamicLibrary::Load(const wxString& name)
{
    m_error.Clear();
#if defined(__WXMSW__)
    return m_lib.Load(name, wxDL_NOSHARE);
#else
    // open the library
    //#if defined(__WXGTK__) && defined (ON_64_BIT)
    //	// on GTK we need to pass RTLD_DEEPBIND otherwise symbols clashes
    //	m_dllhandle = dlopen(_C(name), RTLD_LAZY| RTLD_LOCAL | RTLD_DEEPBIND);
    //#else
    m_dllhandle = dlopen(_C(name), RTLD_LAZY);
    //#endif

    if(!m_dllhandle) {
        m_error = wxString(dlerror(), wxConvUTF8);
        return false;
    }
    return true;
#endif
}

void clDynamicLibrary::Detach()
{
    m_error.Clear();
#if defined(__WXMSW__)
    m_lib.Detach();
#else
    if(m_dllhandle) {
        dlclose(m_dllhandle);
        m_dllhandle = NULL;
    }
#endif
}

void* clDynamicLibrary::GetSymbol(const wxString& name, bool* success)
{
    m_error.Clear();
#if defined(__WXMSW__)
    bool rc;
    void* symb = m_lib.GetSymbol(name, &rc);
    *success = rc;
    return symb;
#else
    dlerror(); // reset errors

    // load the symbol
    void* symb = dlsym(m_dllhandle, _C(name));
    if(symb) {
        *success = true;
    } else {
        *success = false;
        m_error = wxString(dlerror(), wxConvUTF8);
    }
    return symb;
#endif
}
