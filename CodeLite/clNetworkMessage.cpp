#include "clNetworkMessage.h"
#include "json_node.h"
#include <algorithm>

clNetworkMessage::clNetworkMessage() {}
clNetworkMessage::clNetworkMessage(const wxString& str)
{
    JSONRoot root(str);
    if(!root.isOk()) { return; }
    JSONElement e = root.toElement();
    JSONElement iter = e.firstChild();
    while(iter.isOk()) {
        m_values.insert({ iter.getName(), iter.toString() });
        iter = e.nextChild();
    }
}

clNetworkMessage::~clNetworkMessage() {}

wxString clNetworkMessage::ToString() const
{
    JSONRoot root(cJSON_Object);
    JSONElement e = root.toElement();
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
