#include "ResponseError.h"

LSP::ResponseError::ResponseError() {}

LSP::ResponseError::ResponseError(const wxString& message)
{
    JSON json(message);
    FromJSON(json.toElement());
}

LSP::ResponseError::~ResponseError() {}

void LSP::ResponseError::FromJSON(const JSONItem& json)
{
    if(json.hasNamedObject("error")) {
        // serialize the parent
        this->Message::FromJSON(json);

        // extract the error message
        JSONItem error = json.namedObject("error");
        // an error message
        m_errorCode = error.namedObject("code").toInt();
        m_message = error.namedObject("message").toString();
    }
}

std::string LSP::ResponseError::ToString() const { return ""; }

JSONItem LSP::ResponseError::ToJSON(const wxString& name) const { return JSONItem(nullptr); }
