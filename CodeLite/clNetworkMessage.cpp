#include "clNetworkMessage.h"
#include "JSON.h"
#include <algorithm>

clNetworkMessage::clNetworkMessage() {}
clNetworkMessage::clNetworkMessage(const wxString& str)
{
    JSON root(str);
    if(!root.isOk()) { return; }
    JSONItem e = root.toElement();
    JSONItem iter = e.firstChild();
    while(iter.isOk()) {
        m_values.insert({ iter.getName(), iter.toString() });
        iter = e.nextChild();
    }
}

clNetworkMessage::~clNetworkMessage() {}

wxString clNetworkMessage::ToString() const
{
    JSON root(cJSON_Object);
    JSONItem e = root.toElement();
    std::for_each(m_values.begin(), m_values.end(),
                  [&](const wxStringMap_t::value_type& vt) { e.addProperty(vt.first, vt.second); });
    char* data = e.FormatRawString(false);
    if(data) {
        wxString s(data);
        free(data);
        return s;
    }
    return "";
}

void clNetworkMessage::SendMessage(clSocketBase* socket) { socket->WriteMessage(ToString()); }

void clNetworkMessage::SendMessage(clSocketClientAsync* socket) { socket->Send(ToString()); }
