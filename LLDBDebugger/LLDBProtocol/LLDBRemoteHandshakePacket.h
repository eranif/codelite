#ifndef LLDBREMOTEHANDSHAKEPACKET_H
#define LLDBREMOTEHANDSHAKEPACKET_H

#include <wx/string.h>
#include "json_node.h"

class LLDBRemoteHandshakePacket
{
    wxString m_host;

public:
    LLDBRemoteHandshakePacket();
    LLDBRemoteHandshakePacket(const wxString &json);
    ~LLDBRemoteHandshakePacket();
    
    // Serialization API
    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;

    void SetHost(const wxString& host) {
        this->m_host = host;
    }
    const wxString& GetHost() const {
        return m_host;
    }

};

#endif // LLDBREMOTEHANDSHAKEPACKET_H
