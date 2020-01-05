#include "ResponseError.h"

LSP::ResponseError::ResponseError() {}

LSP::ResponseError::ResponseError(const wxString& message, IPathConverter::Ptr_t pathConverter)
    : m_pathConverter(pathConverter)
{
    JSON json(message);
    FromJSON(json.toElement(), m_pathConverter);
}

LSP::ResponseError::~ResponseError() {}

void LSP::ResponseError::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    if(json.hasNamedObject("error")) {
        // serialize the parent
        this->Message::FromJSON(json, pathConverter);

        // extract the error message
        JSONItem error = json.namedObject("error");
        // an error message
        m_errorCode = error.namedObject("code").toInt();
        m_message = error.namedObject("message").toString();
    }
}

std::string LSP::ResponseError::ToString(IPathConverter::Ptr_t pathConverter) const
{
    wxUnusedVar(pathConverter);
    return "";
}

JSONItem LSP::ResponseError::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    wxUnusedVar(pathConverter);
    return JSONItem(nullptr);
}
