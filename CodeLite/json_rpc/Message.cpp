#include "Message.h"

json_rpc::Message::Message() {}

json_rpc::Message::~Message() {}

JSONElement json_rpc::Message::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("jsonrpc", m_jsonrpc);
    return json;
}

void json_rpc::Message::FromJSON(const JSONElement& json) { m_jsonrpc = json.namedObject("jsonrpc").toString(); }

int json_rpc::Message::GetNextID()
{
    static int requestId = 0;
    return ++requestId;
}
