#include "Message.h"

#include "LSP/basic_types.h"
#include "cl_standard_paths.h"
#include "fileutils.h"

#include <wx/tokenzr.h>

namespace
{
#define HEADER_CONTENT_LENGTH "Content-Length"

bool HasCompleteMessage(const std::string& inbuf, unsigned long content_legnth, unsigned long headers_size)
{
    return inbuf.length() >= (content_legnth + headers_size);
}

int ReadHeaders(const std::string& message, std::unordered_map<std::string, std::string>& headers)
{
    int where = message.find("\r\n\r\n");
    if(where == wxNOT_FOUND) {
        return wxNOT_FOUND;
    }

    auto headerSection = message.substr(0, where); // excluding the "\r\n\r\n"
    wxArrayString lines = ::wxStringTokenize(headerSection, "\n", wxTOKEN_STRTOK);
    for(wxString& header : lines) {
        header.Trim().Trim(false);
        wxString name = header.BeforeFirst(':');
        wxString value = header.AfterFirst(':');
        headers.insert(
            { name.Trim().Trim(false).mb_str(wxConvUTF8).data(), value.Trim().Trim(false).mb_str(wxConvUTF8).data() });
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

std::unique_ptr<JSON> LSP::Message::GetJSONPayload(std::string& network_buffer)
{
    // Strip the headers
    std::unordered_map<std::string, std::string> headers;
    int headersSize = ReadHeaders(network_buffer, headers);
    if(headersSize == wxNOT_FOUND) {
        LSP_WARNING() << "Unable to read headers from buffer:" << endl;
        LSP_WARNING() << network_buffer << endl;
        return nullptr;
    }

    if(headers.count(HEADER_CONTENT_LENGTH) == 0) {
        LSP_WARNING() << "LSP message header does not contain the Content-Length header!" << endl;
        return nullptr;
    }

    unsigned long contentLength = 0;
    wxString contentLengthValue = headers[HEADER_CONTENT_LENGTH];
    if(!contentLengthValue.ToCULong(&contentLength)) {
        LSP_WARNING() << "Failed to convert Content-Length header to number" << endl;
        LSP_WARNING() << "Content-Length:" << headers[HEADER_CONTENT_LENGTH] << endl;
        return nullptr;
    }

    if(!HasCompleteMessage(network_buffer, contentLength, headersSize)) {
        LSP_DEBUG() << "Input buffer is too small" << endl;
        return nullptr;
    }

    // remove the header part
    network_buffer.erase(0, headersSize);

    // remove the payload
    auto payload = network_buffer.substr(0, contentLength);
    network_buffer.erase(0, contentLength);

    // convert payload to wxString
    wxString json_str = wxString::FromUTF8(payload);

    LOG_IF_TRACE
    {
        if(json_str.length() != payload.length()) {
            LSP_TRACE() << "UTF8 chars detected" << endl;
            LSP_TRACE() << "wx.length()=" << json_str.length() << endl;
            LSP_TRACE() << "c.length()=" << payload.length() << endl;
            LSP_TRACE() << HEADER_CONTENT_LENGTH << contentLength << endl;
        }
    }

    if(!json_str.EndsWith("}")) {
        LSP_WARNING() << "JSON payload does not end with '}'" << endl;

        // for debugging purposes, dump the content
        auto wxfile = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "wx", "json");
        FileUtils::WriteFileContent(wxfile, json_str);
        LSP_WARNING() << "wx-content written into:" << wxfile << endl;

        auto cfile = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "cfile", "json");
        FileUtils::WriteFileContentRaw(cfile, payload);
        LSP_WARNING() << "c-content written into:" << cfile << endl;
    }

    std::unique_ptr<JSON> json(new JSON(json_str));
    if(!json->isOk()) {
        LSP_ERROR() << "Unable to parse JSON object from response!" << endl;
        return json;
    }
    return json;
}
