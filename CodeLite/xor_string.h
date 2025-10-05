//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : xor_string.h
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

#ifndef XORSTRING_H
#define XORSTRING_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL XORString
{
    wxString m_value;
    
protected:
    wxString toHexString(const wxString &value) const;
    wxString fromHexString(const wxString& hexString) const;
    wxString XOR(const wxString& str, const wxChar KEY) const;
    
public:
    XORString(const wxString &value);
    XORString() = default;
    virtual ~XORString() = default;

    wxString Decrypt(const wxChar byte = 's') const;
    wxString Encrypt(const wxChar byte = 's') const;
};

#endif // XORSTRING_H
