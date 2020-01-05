#include "JSON.h"
#include "LSP/Request.h"

LSP::Request::Request() { m_id = Message::GetNextID(); }

LSP::Request::~Request() {}

JSONItem LSP::Request::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = MessageWithParams::ToJSON(name, pathConverter);
    json.addProperty("id", GetId());
    return json;
}

void LSP::Request::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    wxUnusedVar(json);
    wxUnusedVar(pathConverter);
}
