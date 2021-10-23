#include "Message.h"
#include <wx/tokenzr.h>

namespace
{
#define HEADER_CONTENT_LENGTH "Content-Length"

#define STATE_NORMAL 0
#define STATE_DOUBLE_QUOTES 1
#define STATE_ESCAPE 2

/// Since clangd might return a binary characters it breaks the wxString conversion
/// This means that the "Content-Lenght" value might point us into an invalid position
/// inside the position (i.e. broken json mssage)
/// So we dont rely on the header length, but instead we count the chars ourself
int FindCompleteMessage(const wxString& jsonMsg, int startIndex)
{
    if(jsonMsg[startIndex] != '{') {
        return wxNOT_FOUND;
    }
    int depth = 1;
    int msglen = 1; // skip the '{'
    int state = STATE_NORMAL;
    size_t strLen = jsonMsg.length();
    for(size_t i = (startIndex + 1); i < strLen; ++i, ++msglen) {
        wxChar ch = jsonMsg[i];
        switch(state) {
        case STATE_NORMAL:
            switch(ch) {
            case '{':
            case '[':
                ++depth;
                break;
            case ']':
            case '}':
                --depth;
                if(depth == 0) {
                    return (msglen + 1); // include this char
                }
                break;
            case '"':
                state = STATE_DOUBLE_QUOTES;
                break;
            default:
                break;
            }
            break;
        case STATE_DOUBLE_QUOTES:
            switch(ch) {
            case '\\':
                state = STATE_ESCAPE;
                break;
            case '"':
                state = STATE_NORMAL;
                break;
            default:
                break;
            }
            break;
        case STATE_ESCAPE:
            switch(ch) {
            default:
                state = STATE_DOUBLE_QUOTES;
                break;
            }
            break;
        }
    }
    return wxNOT_FOUND;
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
        return nullptr;
    }

    if(headers.count(HEADER_CONTENT_LENGTH) == 0) {
        return nullptr;
    }

    int msglen = FindCompleteMessage(network_buffer, headersSize);
    if(msglen == wxNOT_FOUND) {
        return nullptr;
    }

    if((headersSize + msglen) > (int)network_buffer.length()) {
        return nullptr;
    }
    wxString json_str = network_buffer.Mid(0, headersSize + msglen);

    // Remove the message from the network buffer
    network_buffer.Remove(0, headersSize + msglen);

    // remove the headers section from the message and construct a JSON object
    json_str.Remove(0, headersSize);
    std::unique_ptr<JSON> json(new JSON(json_str));
    if(json->isOk()) {
        return json;
    }
    return nullptr;
}
