#include "LSP/Request.h"

LSP::Request::Request()
    : m_id(Message::GetNextID())
{
}

nlohmann::json LSP::Request::ToJSON() const
{
    auto json = MessageWithParams::ToJSON();
    json["id"] = GetId();
    return json;
}

void LSP::Request::FromJSON(const JSONItem& json) { wxUnusedVar(json); }
