//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_registry.h
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

#ifndef CLREGISTRY_H
#define CLREGISTRY_H

#include "codelite_exports.h"

#include <map>
#include <wx/ffile.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clRegistry
{
    static wxString m_filename;
    wxFFile m_fp;
    std::map<wxString, wxString> m_entries;

public:
    static void SetFilename(const wxString& filename);

public:
    clRegistry();
    virtual ~clRegistry();

    bool Read(const wxString& key, wxString& val);
};

#endif // CLREGISTRY_H
