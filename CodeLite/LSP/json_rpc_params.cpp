#include "LSP/json_rpc_params.h"

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
namespace LSP
{

void TextDocumentPositionParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_position.FromJSON(json["position"]);
}

nlohmann::json TextDocumentPositionParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}, {"position", m_position.ToJSON()}};
}

//===----------------------------------------------
// SemanticTokensParams
//===----------------------------------------------

void SemanticTokensParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

nlohmann::json SemanticTokensParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}};
}

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------

void DidOpenTextDocumentParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

nlohmann::json DidOpenTextDocumentParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}};
}

//===----------------------------------------------------------------------------------
// DidCloseTextDocumentParams
//===----------------------------------------------------------------------------------

void DidCloseTextDocumentParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

nlohmann::json DidCloseTextDocumentParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}};
}

//===----------------------------------------------------------------------------------
// DidChangeTextDocumentParams
//===----------------------------------------------------------------------------------

void DidChangeTextDocumentParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_contentChanges.clear();
    if (json.hasNamedObject("contentChanges")) {
        JSONItem arr = json.namedObject("contentChanges");
        int count = arr.arraySize();
        for (int i = 0; i < count; ++i) {
            TextDocumentContentChangeEvent c;
            c.FromJSON(arr.arrayItem(i));
            m_contentChanges.push_back(c);
        }
    }
}

nlohmann::json DidChangeTextDocumentParams::ToJSON() const
{
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& contentChange : m_contentChanges) {
        arr.push_back(contentChange.ToJSON());
    }
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}, {"contentChanges", std::move(arr)}};
}

//===----------------------------------------------------------------------------------
// DidSaveTextDocumentParams
//===----------------------------------------------------------------------------------

void DidSaveTextDocumentParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_text = json["text"].toString();
}

nlohmann::json DidSaveTextDocumentParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}, {"text", m_text.ToStdString(wxConvUTF8)}};
}

//===----------------------------------------------------------------------------------
// CompletionParams
//===----------------------------------------------------------------------------------

void CompletionParams::FromJSON(const JSONItem& json) { TextDocumentPositionParams::FromJSON(json); }

nlohmann::json CompletionParams::ToJSON() const { return TextDocumentPositionParams::ToJSON(); }

//===----------------------------------------------------------------------------------
// CodeActionParams
//===----------------------------------------------------------------------------------
ExecuteCommandParams::ExecuteCommandParams(const wxString& command, const wxString& arguments)
    : m_command(command)
    , m_arguments(arguments)
{
}

void ExecuteCommandParams::FromJSON(const JSONItem& json) { wxUnusedVar(json); }

nlohmann::json ExecuteCommandParams::ToJSON() const
{
    nlohmann::json json = {{"command", m_command.ToStdString(wxConvUTF8)}};

    // parse the "arguments" and add them
    auto argument_json = nlohmann::ordered_json::parse(m_arguments.ToStdString(wxConvUTF8), nullptr, false);
    if (!argument_json.is_discarded()) {
        json["arguments"] = std::move(argument_json);
    }
    return json;
}

//===----------------------------------------------------------------------------------
// CodeActionResolveParams
//===----------------------------------------------------------------------------------
CodeActionResolveParams::CodeActionResolveParams(const wxString& codeAction)
    : m_codeAction(codeAction)
{
}

void CodeActionResolveParams::FromJSON(const JSONItem& json) { m_codeAction = json.format(false); }

nlohmann::json CodeActionResolveParams::ToJSON() const
{
    auto json = nlohmann::json::parse(m_codeAction.ToStdString(wxConvUTF8), nullptr, false);
    if (json.is_discarded()) {
        LSP_WARNING() << "Failed to parse the code action to resolve:" << m_codeAction << endl;
        return nlohmann::json::object();
    }
    return json;
}

//===----------------------------------------------------------------------------------
// CodeActionParams
//===----------------------------------------------------------------------------------

void CodeActionParams::FromJSON(const JSONItem& json) { wxUnusedVar(json); }

nlohmann::json CodeActionParams::ToJSON() const
{
    auto diags_arr = nlohmann::json::array();
    for (const auto& diag : m_diagnostics) {
        diags_arr.push_back(diag.ToJSON());
    }
    nlohmann::json context{{"diagnostics", std::move(diags_arr)}};
    if (!m_only.empty()) {
        auto only_arr = nlohmann::json::array();
        for (const auto& kind : m_only) {
            only_arr.push_back(kind.ToStdString(wxConvUTF8));
        }
        context["only"] = std::move(only_arr);
    }
    return nlohmann::json{
        {"textDocument", m_textDocument.ToJSON()}, {"range", m_range.ToJSON()}, {"context", std::move(context)}};
}

//===----------------------------------------------------------------------------------
// DocumentSymbolParams
//===----------------------------------------------------------------------------------

void DocumentSymbolParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

nlohmann::json DocumentSymbolParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()}};
}

ReferenceParams::ReferenceParams(bool includeDeclaration)
    : m_includeDeclaration(includeDeclaration)
{
}

void ReferenceParams::FromJSON(const JSONItem& json)
{
    TextDocumentPositionParams::FromJSON(json);
    m_includeDeclaration = json["context"]["includeDeclaration"].toBool(m_includeDeclaration);
}

nlohmann::json ReferenceParams::ToJSON() const
{
    auto json = TextDocumentPositionParams::ToJSON();
    json["context"]["includeDeclaration"] = m_includeDeclaration;
    return json;
}

void RenameParams::FromJSON(const JSONItem& json) { wxUnusedVar(json); }

nlohmann::json RenameParams::ToJSON() const
{
    auto json = TextDocumentPositionParams::ToJSON();
    json["newName"] = m_newName.ToStdString(wxConvUTF8);
    return json;
}

//===----------------------------------------------------------------------------------
// DocumentFormattingParams
//===----------------------------------------------------------------------------------
void DocumentFormattingParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_tabSize = json["options"]["tabSize"].toSize_t(m_tabSize);
    m_insertSpaces = json["options"]["insertSpaces"].toBool(m_insertSpaces);
}

nlohmann::json DocumentFormattingParams::ToJSON() const
{
    return nlohmann::json{{"textDocument", m_textDocument.ToJSON()},
                          {"options", {{"tabSize", m_tabSize}, {"insertSpaces", m_insertSpaces}}}};
}

} // namespace LSP
