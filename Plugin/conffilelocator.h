//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : conffilelocator.h
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

#ifndef __conffilelocator__
#define __conffilelocator__
#include "codelite_exports.h"

#include <wx/string.h>

class WXDLLIMPEXP_SDK ConfFileLocator
{

    static ConfFileLocator* ms_instance;
    wxString m_installPath;
    wxString m_startupPath;

private:
    ConfFileLocator(const ConfFileLocator& rhs);
    ConfFileLocator& operator=(const ConfFileLocator& rhs);

public:
    static ConfFileLocator* Instance();
    static void Release();

    void Initialize(const wxString& installpath, const wxString& startuppath);
    wxString Locate(const wxString& baseName);
    wxString GetLocalCopy(const wxString& baseName);
    wxString GetDefaultCopy(const wxString& baseName);
    void DeleteLocalCopy(const wxString& basename);

private:
    ConfFileLocator();
    virtual ~ConfFileLocator();
};
#endif // __conffilelocator__
