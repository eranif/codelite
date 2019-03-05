#include "ResponseMessage.h"

json_rpc::ResponseMessage::ResponseMessage() {}

json_rpc::ResponseMessage::~ResponseMessage() {}

wxString json_rpc::ResponseMessage::ToString() const { return ""; }

// we dont really serialise response messages
JSONItem json_rpc::ResponseMessage::ToJSON(const wxString& name) const { return JSONItem(nullptr); }

void json_rpc::ResponseMessage::FromJSON(const JSONItem& json) 
{
    Message::FromJSON(json);
    m_id = json.namedObject("id").toInt();
}
