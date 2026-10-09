#include "InitializeRequest.h"

LSP::InitializeRequest::InitializeRequest(bool withTokenTypes, const wxString& rootUri)
    : m_withTokenTypes(withTokenTypes)
{
    SetMethod("initialize");
    m_processId = ::wxGetProcessId();
    m_rootUri = rootUri;
}

nlohmann::json LSP::InitializeRequest::ToJSON() const
{
    auto json = Request::ToJSON();

    // add the 'params'
    nlohmann::json params = nlohmann::json::object();
    params["processId"] = GetProcessId();
    if (!GetRootUri().IsEmpty()) {
        params["rootUri"] = LSP::FileNameToURI(GetRootUri()).ToStdString(wxConvUTF8);
    }

    if (!m_initOptions.empty()) {
        // Parse the JSON string and set it as the 'initializationOptions
        auto initializationOptions =
            nlohmann::ordered_json::parse(m_initOptions.mb_str(wxConvUTF8).data(), nullptr, false);
        if (!initializationOptions.is_discarded()) {
            params["initializationOptions"] = std::move(initializationOptions);
        }
    }

    auto& capabilities = params["capabilities"] = nlohmann::json::object();
    capabilities["window"] = nlohmann::json{{"workDoneProgress", true}};
    capabilities["workspace"]["applyEdit"] = true;
    // "delete" is not supported: it is not needed by any server we know of
    capabilities["workspace"]["workspaceEdit"] =
        nlohmann::json{{"documentChanges", true}, {"resourceOperations", nlohmann::json::array({"create", "rename"})}};

    auto& textDocumentCapabilities = capabilities["textDocument"];
    textDocumentCapabilities["completion"]["completionItem"]["documentationFormat"] = std::array{"plaintext"};
    textDocumentCapabilities["hover"]["contentFormat"] = {"markdown", "plaintext"};

    auto& codeActionCapabilities = textDocumentCapabilities["codeAction"];
    // Only the kinds that have been tested. Add the other "source.*" kinds once they are tested
    codeActionCapabilities["codeActionLiteralSupport"]["codeActionKind"]["valueSet"] = nlohmann::json::array(
        {"", "quickfix", "refactor.extract", "refactor.inline", "refactor.rewrite", "source.organizeImports"});
    codeActionCapabilities["isPreferredSupport"] = true;
    codeActionCapabilities["disabledSupport"] = true;
    codeActionCapabilities["dataSupport"] = true;
    codeActionCapabilities["resolveSupport"]["properties"] = nlohmann::json::array({"edit"});

    textDocumentCapabilities["formatting"]["dynamicRegistration"] = false;

    if (m_withTokenTypes) {
        textDocumentCapabilities["semanticTokens"]["tokenTypes"] = {
            "type",     "class",    "enum",       "interface", "struct",   "typeParameter", "parameter",
            "variable", "property", "enumMember", "event",     "function", "method",        "macro",
            "keyword",  "modifier", "comment",    "string",    "number",   "regexp",        "operator"};
        textDocumentCapabilities["semanticTokens"]["tokenModifiers"] = {"declaration",
                                                                        "definition",
                                                                        "readonly",
                                                                        "static",
                                                                        "deprecated",
                                                                        "abstract",
                                                                        "async",
                                                                        "modification",
                                                                        "documentation",
                                                                        "defaultLibrary"};
    }

    json["params"] = std::move(params);
    return json;
}

std::optional<LSPEvent> LSP::InitializeRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(response);
    wxUnusedVar(owner);
    return std::nullopt;
}
