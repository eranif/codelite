#include "json_rpc/RequestMessage.h"
#include <wx/string.h>
#include "json_node.h"

json_rpc::RequestMessage::RequestMessage() { m_id = Message::GetNextID(); }

json_rpc::RequestMessage::~RequestMessage() {}

JSONElement json_rpc::RequestMessage::ToJSON(const wxString& name) const
{
    JSONElement json = Message::ToJSON(name);
    json.addProperty("id", GetId());
    json.addProperty("method", GetMethod());
    if(m_params) { json.append(m_params->ToJSON("params")); }
    return json;
}

void json_rpc::RequestMessage::FromJSON(const JSONElement& json)
{
    // we dont need to un-serialize a request object
    wxUnusedVar(json);
}

wxString json_rpc::RequestMessage::ToString() const
{
    // Serialize the object and construct a JSON-RPC message
    JSONElement json = ToJSON("");

    wxString data = json.format();
    size_t len = data.length();

    // Build the request
    wxString buffer;
    buffer << "Content-Type: application/jsonrpc; charset=utf8\r\n";
    buffer << "Content-Length: " << len << "\r\n";
    buffer << "\r\n";
    buffer << data;
    return buffer;
}
