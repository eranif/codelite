#include "Message.h"

#include "LSP/basic_types.h"

#include <wx/tokenzr.h>

namespace
{
#define HEADER_CONTENT_LENGTH "Content-Length"

#define STATE_NORMAL 0
#define STATE_DOUBLE_QUOTES 1
#define STATE_ESCAPE 2

bool HasCompleteMessage(const wxString& jsonMsg, unsigned long contentLength, unsigned long startIndex)
{
    if(jsonMsg.length() < startIndex) {
        return wxNOT_FOUND;
    }

    // Check that we have enough buffer
    if(jsonMsg.length() > (startIndex + contentLength)) {
        return wxNOT_FOUND;
    }

    // we have enough buffer
    return true;
}

int ReadHeaders(const wxString& message, wxStringMap_t& headers)
{
    int where = message.Find("\r\n\r\n");
    if(where == wxNOT_FOUND) {
        return wxNOT_FOUND;
    }
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

} // namespace
LSP::Message::Message() {}

LSP::Message::~Message() {}

JSONItem LSP::Message::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("jsonrpc", m_jsonrpc);
    return json;
}

void LSP::Message::FromJSON(const JSONItem& json) { m_jsonrpc = json.namedObject("jsonrpc").toString(); }

int LSP::Message::GetNextID()
{
    static int requestId = 0;
    return ++requestId;
}

std::unique_ptr<JSON> LSP::Message::GetJSONPayload(wxString& network_buffer)
{
    // Strip the headers
    wxStringMap_t headers;
    int headersSize = ReadHeaders(network_buffer, headers);
    if(headersSize == wxNOT_FOUND) {
        LSP_WARNING() << "Unable to read headers from buffer:" << endl;
        LSP_WARNING() << network_buffer << endl;
        return nullptr;
    }

    if(headers.count(HEADER_CONTENT_LENGTH) == 0) {
        LSP_WARNING() << "LSP message header does not contain the Content-Length header!" << endl;
        LSP_WARNING() << headers << endl;
        return nullptr;
    }

    unsigned long contentLength = 0;
    if(!headers[HEADER_CONTENT_LENGTH].ToCULong(&contentLength)) {
        LSP_WARNING() << "Failed to convert Content-Length header to number" << endl;
        LSP_WARNING() << "Content-Length:" << headers[HEADER_CONTENT_LENGTH] << endl;
        return nullptr;
    }

    if(!HasCompleteMessage(network_buffer, contentLength, headersSize)) {
        LSP_DEBUG() << "Input buffer is too small" << endl;
        return nullptr;
    }

    wxString json_str = network_buffer.Mid(0, headersSize + contentLength);

    // Remove the message from the network buffer
    network_buffer.Remove(0, json_str.length());

    // remove the headers section from the message and construct a JSON object
    json_str.Remove(0, headersSize);
    std::unique_ptr<JSON> json(new JSON(json_str));
    if(!json->isOk()) {
        LSP_ERROR() << "Unable to parse JSON object from response!" << endl;
        LSP_ERROR() << "\n" << json_str << endl;
        return json;
    }
    return json;
}
