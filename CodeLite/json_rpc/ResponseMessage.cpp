#include "ResponseMessage.h"

json_rpc::ResponseMessage::ResponseMessage(const wxString& message)
{
    m_json.reset(new JSON(message));
    if(!m_json->isOk()) {
        m_json.reset(nullptr);
    } else {
        FromJSON(m_json->toElement());
    }
}

json_rpc::ResponseMessage::~ResponseMessage() {}

wxString json_rpc::ResponseMessage::ToString() const { return ""; }

// we dont really serialise response messages
JSONItem json_rpc::ResponseMessage::ToJSON(const wxString& name) const { return JSONItem(nullptr); }

void json_rpc::ResponseMessage::FromJSON(const JSONItem& json)
{
    Message::FromJSON(json);
    m_id = json.namedObject("id").toInt();
}

bool json_rpc::ResponseMessage::Has(const wxString& property) const
{
    return m_json && m_json->toElement().hasNamedObject(property);
}

JSONItem json_rpc::ResponseMessage::Get(const wxString& property) const
{
    if(!Has(property)) { return JSONItem(nullptr); }
    return m_json->toElement().namedObject(property);
}
