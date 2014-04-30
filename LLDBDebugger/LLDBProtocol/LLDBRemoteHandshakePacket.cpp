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
