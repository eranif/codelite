#include "LSP/Request.h"

LSP::Request::Request()
    : m_id(Message::GetNextID())
{
}

JSONItem LSP::Request::ToJSON() const
{
    JSONItem json = MessageWithParams::ToJSON();
    json.addProperty("id", GetId());
    return json;
}

void LSP::Request::FromJSON(const JSONItem& json) { wxUnusedVar(json); }
