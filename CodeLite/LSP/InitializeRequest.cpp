#include "InitializeRequest.h"
#include <wx/filesys.h>

LSP::InitializeRequest::InitializeRequest(const wxString& rootUri)
{
    SetMethod("initialize");
    m_processId = ::wxGetProcessId();
    m_rootUri = rootUri;
}

LSP::InitializeRequest::~InitializeRequest() {}

JSONItem LSP::InitializeRequest::ToJSON(const wxString& name) const
{
    JSONItem json = Request::ToJSON(name);

    // add the 'params'
    JSONItem params = JSONItem::createObject("params");
    json.append(params);
    params.addProperty("processId", GetProcessId());
    if(GetRootUri().IsEmpty()) {
        JSON nullItem(cJSON_NULL);
        JSONItem nullObj = nullItem.toElement();
        params.append(nullObj);
        (void)nullItem.release(); // dont delete it on destruction, it is now owned by 'params'
    } else {
        params.addProperty("rootUri", wxFileSystem::FileNameToURL(GetRootUri()));
    }
    JSONItem capabilities = JSONItem::createObject("capabilities");
    params.append(capabilities);
    JSONItem textDocument = JSONItem::createObject("textDocument");
    capabilities.append(textDocument);
    return json;
}

void LSP::InitializeRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(response);
    wxUnusedVar(owner);
}
