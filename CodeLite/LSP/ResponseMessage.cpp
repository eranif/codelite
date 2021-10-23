#include "ResponseMessage.h"
#include "StringUtils.h"
#include <wx/tokenzr.h>

LSP::ResponseMessage::ResponseMessage(std::unique_ptr<JSON>&& json)
{
    // a valid JSON-RPC response
    m_json = std::move(json);
    FromJSON(m_json->toElement());
}

LSP::ResponseMessage::~ResponseMessage() {}

std::string LSP::ResponseMessage::ToString() const
{
    if(!m_json || !m_json->isOk()) {
        return "";
    }
    return StringUtils::ToStdString(m_json->toElement().format(false));
}

// we dont really serialise response messages
JSONItem LSP::ResponseMessage::ToJSON(const wxString& name) const { return JSONItem(nullptr); }

void LSP::ResponseMessage::FromJSON(const JSONItem& json)
{
    Message::FromJSON(json);
    m_id = json.namedObject("id").toInt();
}

bool LSP::ResponseMessage::Has(const wxString& property) const
{
    return m_json && m_json->toElement().hasNamedObject(property);
}

JSONItem LSP::ResponseMessage::Get(const wxString& property) const
{
    if(!Has(property)) {
        return JSONItem(nullptr);
    }
    return m_json->toElement().namedObject(property);
}

std::vector<LSP::Diagnostic> LSP::ResponseMessage::GetDiagnostics() const
{
    JSONItem params = Get("params");
    if(!params.isOk()) {
        return {};
    }

    std::vector<LSP::Diagnostic> res;
    JSONItem arrDiags = params.namedObject("diagnostics");
    int size = arrDiags.arraySize();
    for(int i = 0; i < size; ++i) {
        LSP::Diagnostic d;
        d.FromJSON(arrDiags.arrayItem(i));
        res.push_back(d);
    }
    return res;
}

wxString LSP::ResponseMessage::GetDiagnosticsUri() const
{
    JSONItem params = Get("params");
    if(!params.isOk()) {
        return "";
    }
    return params.namedObject("uri").toString();
}

bool LSP::ResponseMessage::IsErrorResponse() const { return Has("error"); }
