//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : xor_string.cpp
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

#include "xor_string.h"

#include <wx/crt.h>

wxString XORString::Decrypt(const wxChar byte) const
{
    wxString dec = fromHexString(m_value);
    dec = XOR(dec, byte);
    return dec;
}

wxString XORString::Encrypt(const wxChar byte) const
{
    wxString enc = XOR(m_value, byte);
    enc = toHexString(enc);
    return enc;
}

XORString::XORString(const wxString& value)
    : m_value(value)
{
}

wxString XORString::toHexString(const wxString &value) const
{
    wxString output;
    for(size_t i=0; i<value.length(); ++i) {
        wxChar buf[5] = {0, 0, 0, 0, 0};
        wxSprintf(buf, "%04X", (int)value[i]);
        output << buf;
    }
    return output;
}

wxString XORString::fromHexString(const wxString& hexString) const
{
    wxString output;
    size_t cnt = hexString.length() / 4;
    for (size_t i = 0; cnt > i; ++i) {
        wxString dec = hexString.Mid(i*4, 4);
        int hexVal;
        wxSscanf(dec, "%X", &hexVal);
        output << (wxChar)hexVal;
    }
    return output;
}

wxString XORString::XOR(const wxString& str, const wxChar KEY) const
{
    wxString output;
    for(size_t i=0; i<str.length(); ++i) {
        
        wxChar ch = str[i];
        wxChar x = (ch ^ KEY);
        output << x;
    }
    return output;
}
