#include "LSP/MessageWithParams.h"

#include "JSON.h"
#include "LSP/JSONRpcMessage.hpp"

#include <sstream>
#include <wx/string.h>

nlohmann::json LSP::MessageWithParams::ToJSON() const
{
    auto json = Message::ToJSON();
    json["method"] = GetMethod().ToStdString(wxConvUTF8);
    if (m_params) {
        json["params"] = m_params->ToJSON();
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
    JSONItem json = ToJSON();
    JSONRpcMessage message{std::move(json)};
    return message.ToString();
}

LSP::MessageWithParams::Ptr_t LSP::MessageWithParams::MakeRequest(LSP::MessageWithParams* message_ptr)
{
    LSP::MessageWithParams::Ptr_t p(message_ptr);
    return p;
}
