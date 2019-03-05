#include "ResponseMessage.h"

json_rpc::ResponseMessage::ResponseMessage() {}

json_rpc::ResponseMessage::~ResponseMessage() {}

wxString json_rpc::ResponseMessage::ToString() const { return ""; }

// we dont really serialise response messages
JSONElement json_rpc::ResponseMessage::ToJSON(const wxString& name) const { return JSONElement(nullptr); }

void json_rpc::ResponseMessage::FromJSON(const JSONElement& json) 
{
    Message::FromJSON(json);
    m_id = json.namedObject("id").toInt();
}
