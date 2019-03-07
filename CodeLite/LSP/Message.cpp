#include "Message.h"

LSP::Message::Message() {}

LSP::Message::~Message() {}

JSONItem LSP::Message::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("jsonrpc", m_jsonrpc);
    return json;
}

void LSP::Message::FromJSON(const JSONItem& json) { m_jsonrpc = json.namedObject("jsonrpc").toString(); }

int LSP::Message::GetNextID()
{
    static int requestId = 0;
    return ++requestId;
}
