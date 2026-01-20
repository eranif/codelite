#include "LSP/MessageWithParams.h"

#include "JSON.h"

#include <sstream>
#include <wx/string.h>

JSONItem LSP::MessageWithParams::ToJSON(const wxString& name) const
{
    JSONItem json = Message::ToJSON(name);
    json.addProperty("method", GetMethod());
    if (m_params) {
        json.append(m_params->ToJSON("params"));
    }
    return json;
}

void LSP::MessageWithParams::FromJSON(const JSONItem& json)
{
    // we don't need to un-serialize a request object
    wxUnusedVar(json);
}

std::string LSP::MessageWithParams::ToString() const
{
    // Serialize the object and construct a JSON-RPC message
    JSONItem json = ToJSON("");
    char* data = json.FormatRawString(false);

    std::string s;
    size_t len = strlen(data);

    // Build the request header
    std::stringstream ss;
    ss << "Content-Length: " << len << "\r\n";
    ss << "Content-Type: application/json; charset=utf-8" << "\r\n";
    ss << "\r\n";
    s = ss.str();

    // append the data
    s.append(data, len);

    // release the buffer
    free(data);
    return s;
}

LSP::MessageWithParams::Ptr_t LSP::MessageWithParams::MakeRequest(LSP::MessageWithParams* message_ptr)
{
    LSP::MessageWithParams::Ptr_t p(message_ptr);
    return p;
}
