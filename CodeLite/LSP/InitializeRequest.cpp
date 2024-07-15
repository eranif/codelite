#include "InitializeRequest.h"

#include <wx/filesys.h>

LSP::InitializeRequest::InitializeRequest(bool withTokenTypes, const wxString& rootUri)
    : m_withTokenTypes(withTokenTypes)
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
    if (GetRootUri().IsEmpty()) {
        JSON nullItem(cJSON_NULL);
        JSONItem nullObj = nullItem.toElement();
        params.append(nullObj);
        (void)nullItem.release(); // dont delete it on destruction, it is now owned by 'params'
    } else {
        params.addProperty("rootUri", LSP::FileNameToURI(GetRootUri()));
    }

    if (!m_initOptions.empty()) {
        // Parse the JSON string and set it as the 'initializationOptions
        JSON initializationOptions(m_initOptions);
        if (initializationOptions.isOk()) {
            cJSON* pjson = initializationOptions.release();
            params.addProperty(wxString("initializationOptions"), (cJSON*)pjson);
        }
    }

    auto textDocumentCapabilities = params.AddObject("capabilities").AddObject("textDocument");
    auto docFormat =
        textDocumentCapabilities.AddObject("completion").AddObject("completionItem").AddArray("documentationFormat");
    docFormat.arrayAppend("plaintext");
    auto hoverFormat = textDocumentCapabilities.AddObject("hover").AddArray("contentFormat");
    hoverFormat.arrayAppend("markdown");
    hoverFormat.arrayAppend("plaintext");

    if (m_withTokenTypes) {
        auto sematicTokens = textDocumentCapabilities.AddObject("semanticTokens");
        auto tokenTypes = sematicTokens.AddArray("tokenTypes");
        tokenTypes.arrayAppend("type");
        tokenTypes.arrayAppend("class");
        tokenTypes.arrayAppend("enum");
        tokenTypes.arrayAppend("interface");
        tokenTypes.arrayAppend("struct");
        tokenTypes.arrayAppend("typeParameter");
        tokenTypes.arrayAppend("parameter");
        tokenTypes.arrayAppend("variable");
        tokenTypes.arrayAppend("property");
        tokenTypes.arrayAppend("enumMember");
        tokenTypes.arrayAppend("event");
        tokenTypes.arrayAppend("function");
        tokenTypes.arrayAppend("method");
        tokenTypes.arrayAppend("macro");
        tokenTypes.arrayAppend("keyword");
        tokenTypes.arrayAppend("modifier");
        tokenTypes.arrayAppend("comment");
        tokenTypes.arrayAppend("string");
        tokenTypes.arrayAppend("number");
        tokenTypes.arrayAppend("regexp");
        tokenTypes.arrayAppend("operator");

        auto tokenModifiers = sematicTokens.AddArray("tokenModifiers");
        tokenModifiers.arrayAppend("declaration");
        tokenModifiers.arrayAppend("definition");
        tokenModifiers.arrayAppend("readonly");
        tokenModifiers.arrayAppend("static");
        tokenModifiers.arrayAppend("deprecated");
        tokenModifiers.arrayAppend("abstract");
        tokenModifiers.arrayAppend("async");
        tokenModifiers.arrayAppend("modification");
        tokenModifiers.arrayAppend("documentation");
        tokenModifiers.arrayAppend("defaultLibrary");
    }
    return json;
}

void LSP::InitializeRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(response);
    wxUnusedVar(owner);
}
