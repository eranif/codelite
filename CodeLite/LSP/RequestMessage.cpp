#include "LSP/RequestMessage.h"
#include <wx/string.h>
#include "JSON.h"
#include "file_logger.h"
#include "fileutils.h"
#include <sstream>

LSP::RequestMessage::RequestMessage() { m_id = Message::GetNextID(); }

LSP::RequestMessage::~RequestMessage() {}

JSONItem LSP::RequestMessage::ToJSON(const wxString& name) const
{
    JSONItem json = Message::ToJSON(name);
    json.addProperty("id", GetId());
    json.addProperty("method", GetMethod());
    if(m_params) { json.append(m_params->ToJSON("params")); }
    return json;
}

void LSP::RequestMessage::FromJSON(const JSONItem& json)
{
    // we dont need to un-serialize a request object
    wxUnusedVar(json);
}

std::string LSP::RequestMessage::ToString() const
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

LSP::RequestMessage::Ptr_t LSP::RequestMessage::MakeRequest(LSP::RequestMessage* message_ptr)
{
    LSP::RequestMessage::Ptr_t p(message_ptr);
    p->BuildUID();
    return p;
}

void LSP::RequestMessage::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    wxUnusedVar(response);
}
