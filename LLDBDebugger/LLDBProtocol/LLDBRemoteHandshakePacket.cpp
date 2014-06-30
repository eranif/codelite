//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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
    JSONRoot root(json);
    FromJSON( root.toElement() );
}

void LLDBRemoteHandshakePacket::FromJSON(const JSONElement& json)
{
    m_host = json.namedObject("m_host").toString();
}

JSONElement LLDBRemoteHandshakePacket::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_host", m_host);
    return json;
}
