#include "Message.h"

json_rpc::Message::Message() {}

json_rpc::Message::~Message() {}

JSONItem json_rpc::Message::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("jsonrpc", m_jsonrpc);
    return json;
}

void json_rpc::Message::FromJSON(const JSONItem& json) { m_jsonrpc = json.namedObject("jsonrpc").toString(); }

int json_rpc::Message::GetNextID()
{
    static int requestId = 0;
    return ++requestId;
}
