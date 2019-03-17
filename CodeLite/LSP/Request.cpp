#include "LSP/Request.h"
#include "JSON.h"

LSP::Request::Request() { m_id = Message::GetNextID(); }

LSP::Request::~Request() {}

JSONItem LSP::Request::ToJSON(const wxString& name) const
{
    JSONItem json = MessageWithParams::ToJSON(name);
    json.addProperty("id", GetId());
    return json;
}

void LSP::Request::FromJSON(const JSONItem& json) { wxUnusedVar(json); }
