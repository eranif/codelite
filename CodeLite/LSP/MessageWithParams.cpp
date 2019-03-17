#include "LSP/MessageWithParams.h"
#include <wx/string.h>
#include "JSON.h"
#include "file_logger.h"
#include "fileutils.h"
#include <sstream>

LSP::MessageWithParams::MessageWithParams() {}

LSP::MessageWithParams::~MessageWithParams() {}

JSONItem LSP::MessageWithParams::ToJSON(const wxString& name) const
{
    JSONItem json = Message::ToJSON(name);
    json.addProperty("method", GetMethod());
    if(m_params) { json.append(m_params->ToJSON("params")); }
    return json;
}

void LSP::MessageWithParams::FromJSON(const JSONItem& json)
{
    // we dont need to un-serialize a request object
    wxUnusedVar(json);
}

std::string LSP::MessageWithParams::ToString() const
{
    // Serialize the object and construct a JSON-RPC message
    JSONItem json = ToJSON("");
    wxString data = json.format(false);
    data.Trim().Trim(false);

    std::string s = FileUtils::ToStdString(data);
    size_t len = s.length();

    std::stringstream ss;
    // Build the request
    ss << "Content-Length: " << len << "\r\n";
    ss << "\r\n";
    ss << s;
    return ss.str();
}

LSP::MessageWithParams::Ptr_t LSP::MessageWithParams::MakeRequest(LSP::MessageWithParams* message_ptr)
{
    LSP::MessageWithParams::Ptr_t p(message_ptr);
    return p;
}
