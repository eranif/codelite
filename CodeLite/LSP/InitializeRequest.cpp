#include "InitializeRequest.h"
#include <wx/filesys.h>

LSP::InitializeRequest::InitializeRequest(const wxString& rootUri)
{
    SetMethod("initialize");
    m_processId = ::wxGetProcessId();
    m_rootUri = rootUri;
}

LSP::InitializeRequest::~InitializeRequest() {}

JSONItem LSP::InitializeRequest::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = Request::ToJSON(name, pathConverter);

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
    if(!m_initOptions.empty()) {
        // Parse the JSON string and set it as the 'initializationOptions
        JSON initializationOptions(m_initOptions);
        if(initializationOptions.isOk()) {
            cJSON* pjson = initializationOptions.release();
            json.addProperty(wxString("initializationOptions"), (cJSON*)pjson);
        }
    }

    JSONItem capabilities = JSONItem::createObject("capabilities");
    params.append(capabilities);
    JSONItem textDocument = JSONItem::createObject("textDocument");
    capabilities.append(textDocument);
    return json;
}

void LSP::InitializeRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner,
                                        IPathConverter::Ptr_t pathConverter)
{
    wxUnusedVar(response);
    wxUnusedVar(owner);
    wxUnusedVar(pathConverter);
}
