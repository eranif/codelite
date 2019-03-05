//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBRemoteHandshakePacket.cpp
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

#include "LLDBRemoteHandshakePacket.h"

LLDBRemoteHandshakePacket::LLDBRemoteHandshakePacket()
{
}

LLDBRemoteHandshakePacket::~LLDBRemoteHandshakePacket()
{
}

LLDBRemoteHandshakePacket::LLDBRemoteHandshakePacket(const wxString& json)
{
    JSON root(json);
    FromJSON( root.toElement() );
}

void LLDBRemoteHandshakePacket::FromJSON(const JSONItem& json)
{
    m_host = json.namedObject("m_host").toString();
}

JSONItem LLDBRemoteHandshakePacket::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("m_host", m_host);
    return json;
}
