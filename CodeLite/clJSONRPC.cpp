#include "clJSONRPC.h"
#include <wx/filesys.h>

//===------------------------------------------------
// Request
//===------------------------------------------------

clJSONRPCRequest::clJSONRPCRequest() {}

clJSONRPCRequest::~clJSONRPCRequest() {}

clJSONRPCResponseEvent::~clJSONRPCResponseEvent() {}

void clJSONRPCRequest::Send(clJSONRPCSender* sender) const { sender->Send(ToString()); }

int GetNextRequestId()
{
    static int requestId = 0;
    return ++requestId;
}

wxString clJSONRPCRequest::ToString() const
{
    // data
    JSONRoot root(cJSON_Object);
    JSONElement obj = root.toElement();
    obj.addProperty("jsonrpc", wxString("2.0"));
    obj.addProperty("id", GetNextRequestId());
    if(m_params.IsOk()) { obj.append(m_params.GetJSON()); }

    wxString data = obj.format();

    // Detach the 'params' property so it wont get deleted once 'root' goes out of scope
    JSONElement params = obj.detachProperty("params");
    wxUnusedVar(params);

    size_t len = data.length();

    // Build the request
    wxString buffer;
    buffer << "Content-Type: application/jsonrpc; charset=utf8\r\n";
    buffer << "Content-Length: " << len << "\r\n";
    buffer << "\r\n";
    buffer << data;
    return buffer;
}

void clJSONRPCRequest::SetParams(const clJSONRPC_Params& params) { m_params = params; }

//===------------------------------------------------
// Reponse event
//===------------------------------------------------

clJSONRPCResponseEvent::clJSONRPCResponseEvent(const clJSONRPCResponseEvent& event) { *this = event; }

clJSONRPCResponseEvent::clJSONRPCResponseEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clJSONRPCResponseEvent& clJSONRPCResponseEvent::operator=(const clJSONRPCResponseEvent& src)
{
    clCommandEvent::operator=(src);
    m_success = src.m_success;
    m_requestId = src.m_requestId;
    m_result = src.m_result;
    m_errorMessage = src.m_errorMessage;
    return *this;
}

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
void clJSONRPC_TextDocumentIdentifier::FromJSON(const JSONElement& json)
{
    wxString uri = json.namedObject("uri").toString();
    m_filename = wxFileSystem::URLToFileName(uri);
}

JSONElement clJSONRPC_TextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(m_filename));
    return json;
}
//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
void clJSONRPC_Position::FromJSON(const JSONElement& json)
{
    m_line = json.namedObject("line").toInt(0);
    m_character = json.namedObject("character").toInt(0);
}

JSONElement clJSONRPC_Position::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("line", m_line);
    json.addProperty("character", m_character);
    return json;
}

clJSONRPC_Params::clJSONRPC_Params()
    : m_params(NULL)
{
    m_params = JSONElement::createObject("params");
}

clJSONRPC_Params& clJSONRPC_Params::AddParam(const wxString& name, const clJSONRPC_Type& param)
{
    m_params.append(param.ToJSON(name));
}
