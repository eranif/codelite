#include "ResponseMessage.h"
#include <wx/tokenzr.h>

#define HEADER_CONTENT_LENGTH "Content-Length"

LSP::ResponseMessage::ResponseMessage(wxString& message)
{
    // Strip the headers
    wxStringMap_t headers;
    int headersSize = ReadHeaders(message, headers);
    if(headersSize == wxNOT_FOUND) { return; }

    if(headers.count(HEADER_CONTENT_LENGTH) == 0) { return; }
    wxString lenstr = headers[HEADER_CONTENT_LENGTH];
    long nLen(-1);
    if(!lenstr.ToCLong(&nLen)) { return; }

    // Make sure that the message is complete
    if((int)message.length() < (headersSize + nLen)) { return; }

    // Remove the message from the buffer
    m_jsonMessage = message.Mid(0, headersSize + nLen);
    message.Remove(0, headersSize + nLen);

    // Remove the headers part from the JSON message
    m_jsonMessage.Remove(0, headersSize);

    // a valid JSON-RPC response
    m_json.reset(new JSON(m_jsonMessage));
    if(!m_json->isOk()) {
        m_json.reset(nullptr);
    } else {
        FromJSON(m_json->toElement());
    }
}

LSP::ResponseMessage::~ResponseMessage() {}

std::string LSP::ResponseMessage::ToString() const { return ""; }

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
    if(!Has(property)) { return JSONItem(nullptr); }
    return m_json->toElement().namedObject(property);
}

int LSP::ResponseMessage::ReadHeaders(const wxString& message, wxStringMap_t& headers)
{
    int where = message.Find("\r\n\r\n");
    if(where == wxNOT_FOUND) { return wxNOT_FOUND; }
    wxString headerSection = message.Mid(0, where); // excluding the "\r\n\r\n"
    wxArrayString lines = ::wxStringTokenize(headerSection, "\n", wxTOKEN_STRTOK);
    for(wxString& header : lines) {
        header.Trim().Trim(false);
        wxString name = header.BeforeFirst(':');
        wxString value = header.AfterFirst(':');
        headers.insert({ name.Trim().Trim(false), value.Trim().Trim(false) });
    }

    // return the headers section + the separator
    return (where + 4);
}

std::vector<LSP::Diagnostic> LSP::ResponseMessage::GetDiagnostics() const
{
    JSONItem params = Get("params");
    if(!params.isOk()) { return {}; }

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
    if(!params.isOk()) { return ""; }

    return params.namedObject("uri").toString();
}
