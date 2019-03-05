//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBRemoteHandshakePacket.h
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

#ifndef LLDBREMOTEHANDSHAKEPACKET_H
#define LLDBREMOTEHANDSHAKEPACKET_H

#include <wx/string.h>
#include "JSON.h"

class LLDBRemoteHandshakePacket
{
    wxString m_host;

public:
    LLDBRemoteHandshakePacket();
    LLDBRemoteHandshakePacket(const wxString &json);
    ~LLDBRemoteHandshakePacket();
    
    // Serialization API
    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;

    void SetHost(const wxString& host) {
        this->m_host = host;
    }
    const wxString& GetHost() const {
        return m_host;
    }

};

#endif // LLDBREMOTEHANDSHAKEPACKET_H
