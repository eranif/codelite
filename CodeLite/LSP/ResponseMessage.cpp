#include "ResponseMessage.h"
#include <wx/tokenzr.h>

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
    if(jsonMsg[startIndex] != '{') { return wxNOT_FOUND; }
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

LSP::ResponseMessage::ResponseMessage(wxString& message, IPathConverter::Ptr_t pathConverter)
    : m_pathConverter(pathConverter)
{
    // Strip the headers
    wxStringMap_t headers;
    int headersSize = ReadHeaders(message, headers);
    if(headersSize == wxNOT_FOUND) { return; }

    if(headers.count(HEADER_CONTENT_LENGTH) == 0) { return; }
    int msglen = FindCompleteMessage(message, headersSize);
    if(msglen == wxNOT_FOUND) { return; }

    // Remove the message from the buffer
    if((headersSize + msglen) > (int)message.length()) { return; }
    m_jsonMessage = message.Mid(0, headersSize + msglen);

    message.Remove(0, headersSize + msglen);

    // Remove the headers part from the JSON message
    m_jsonMessage.Remove(0, headersSize);

    // a valid JSON-RPC response
    m_json.reset(new JSON(m_jsonMessage));
    if(!m_json->isOk()) {
        m_json.reset(nullptr);
    } else {
        FromJSON(m_json->toElement(), m_pathConverter);
    }
}

LSP::ResponseMessage::~ResponseMessage() {}

std::string LSP::ResponseMessage::ToString(IPathConverter::Ptr_t pathConverter) const
{
    wxUnusedVar(pathConverter);
    return "";
}

// we dont really serialise response messages
JSONItem LSP::ResponseMessage::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    wxUnusedVar(pathConverter);
    return JSONItem(nullptr);
}

void LSP::ResponseMessage::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    Message::FromJSON(json, pathConverter);
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

std::vector<LSP::Diagnostic> LSP::ResponseMessage::GetDiagnostics(IPathConverter::Ptr_t pathConverter) const
{
    JSONItem params = Get("params");
    if(!params.isOk()) { return {}; }

    std::vector<LSP::Diagnostic> res;
    JSONItem arrDiags = params.namedObject("diagnostics");
    int size = arrDiags.arraySize();
    for(int i = 0; i < size; ++i) {
        LSP::Diagnostic d;
        d.FromJSON(arrDiags.arrayItem(i), pathConverter);
        res.push_back(d);
    }
    return res;
}

wxString LSP::ResponseMessage::GetDiagnosticsUri(IPathConverter::Ptr_t pathConverter) const
{
    wxUnusedVar(pathConverter);
    JSONItem params = Get("params");
    if(!params.isOk()) { return ""; }

    return params.namedObject("uri").toString();
}
