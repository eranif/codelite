#include "LSP/basic_types.h"

#include "JSON.h"
#include "clModuleLogger.hpp"
#include "file_logger.h"
#include "fileutils.h"

// our logger object
INITIALISE_MODULE_LOG(LSP_LOG_HANDLER, "LSP", "lsp.log");

namespace LSP
{

clModuleLogger& GetLogHandle() { return LSP_LOG_HANDLER(); }

wxString FileNameToURI(const wxString& filename)
{
    wxString uri;
#ifdef __WXMSW__
    if (filename.StartsWith("/")) {
        // linux format
        uri << "file://" << filename;
    } else {
        uri << wxFileName::FileNameToURL(filename);
    }
#else
    uri << "file://" << filename;
#endif
    return uri;
}

void Initialise() {}

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
void TextDocumentIdentifier::FromJSON(const JSONItem& json)
{
    URI::FromString(json.namedObject("uri").toString(), &m_filename);
}

nlohmann::json TextDocumentIdentifier::ToJSON() const
{ return nlohmann::json{{"uri", GetPathAsURI().ToStdString(wxConvUTF8)}}; }

//===----------------------------------------------------------------------------------
// VersionedTextDocumentIdentifier
//===----------------------------------------------------------------------------------
void VersionedTextDocumentIdentifier::FromJSON(const JSONItem& json)
{
    TextDocumentIdentifier::FromJSON(json);
    m_version = json.namedObject("version").toInt(m_version);
}

nlohmann::json VersionedTextDocumentIdentifier::ToJSON() const
{
    auto json = TextDocumentIdentifier::ToJSON();
    json["version"] = m_version;
    return json;
}

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
void Position::FromJSON(const JSONItem& json)
{
    m_line = json.namedObject("line").toInt(wxNOT_FOUND);
    m_character = json.namedObject("character").toInt(wxNOT_FOUND);
}

nlohmann::json Position::ToJSON() const { return nlohmann::json{{"line", m_line}, {"character", m_character}}; }

//===----------------------------------------------------------------------------------
// TextDocumentItem
//===----------------------------------------------------------------------------------
void TextDocumentItem::FromJSON(const JSONItem& json)
{
    URI::FromString(json.namedObject("uri").toString(), &m_uri);

    m_languageId = json.namedObject("languageId").toString();
    m_version = json.namedObject("version").toInt();
    m_text = json.namedObject("text").toString();
}

nlohmann::json TextDocumentItem::ToJSON() const
{
    return nlohmann::json{{"uri", GetPathAsURI().ToStdString(wxConvUTF8)},
                          {"languageId", GetLanguageId().ToStdString(wxConvUTF8)},
                          {"version", GetVersion()},
                          {"text", GetText().ToStdString(wxConvUTF8)}};
}
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
void TextDocumentContentChangeEvent::FromJSON(const JSONItem& json)
{
    m_text = json.namedObject("text").toString();
    if (json.hasNamedObject("range")) {
        m_range.FromJSON(json["range"]);
    }
}

nlohmann::json TextDocumentContentChangeEvent::ToJSON() const
{
    nlohmann::json json = {{"text", m_text.ToStdString(wxConvUTF8)}};

    if (m_range.IsOk()) {
        json["range"] = m_range.ToJSON();
    }
    return json;
}

void Range::FromJSON(const JSONItem& json)
{
    m_start.FromJSON(json["start"]);
    m_end.FromJSON(json["end"]);
}

nlohmann::json Range::ToJSON() const { return nlohmann::json{{"start", m_start.ToJSON()}, {"end", m_end.ToJSON()}}; }

void Location::FromJSON(const JSONItem& json)
{
    URI::FromString(json.namedObject("uri").toString(), &m_uri);
    m_range.FromJSON(json.namedObject("range"));
    m_pattern = json["pattern"].toString();
    m_name = json["name"].toString();
}

nlohmann::json Location::ToJSON() const
{
    return nlohmann::json{{"uri", GetPathAsURI().ToStdString(wxConvUTF8)},
                          {"range", m_range.ToJSON()},
                          {"pattern", m_pattern.ToStdString(wxConvUTF8)},
                          {"name", m_name.ToStdString(wxConvUTF8)}};
}

void TextEdit::FromJSON(const JSONItem& json)
{
    m_range.FromJSON(json.namedObject("range"));
    m_newText = json.namedObject("newText").toString();
}

nlohmann::json TextEdit::ToJSON() const
{ return nlohmann::json{{"newText", m_newText.ToStdString(wxConvUTF8)}, {"range", m_range.ToJSON()}}; }

void ParameterInformation::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_documentation = json.namedObject("documentation").toString();
}

nlohmann::json ParameterInformation::ToJSON() const
{
    return nlohmann::json{
        {"label", m_label.ToStdString(wxConvUTF8)}, {"documentation", m_documentation.ToStdString(wxConvUTF8)}};
}

void SignatureInformation::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_documentation = json.namedObject("documentation").toString();
    m_parameters.clear();
    if (json.hasNamedObject("parameters")) {
        JSONItem parameters = json.namedObject("parameters");
        const int size = parameters.arraySize();
        if (size > 0) {
            m_parameters.reserve(size);
            for (int i = 0; i < size; ++i) {
                ParameterInformation p;
                p.FromJSON(parameters.arrayItem(i));
                m_parameters.push_back(p);
            }
        }
    }
}

nlohmann::json SignatureInformation::ToJSON() const
{
    nlohmann::json json = {
        {"label", m_label.ToStdString(wxConvUTF8)}, {"documentation", m_documentation.ToStdString(wxConvUTF8)}};
    if (!m_parameters.empty()) {
        nlohmann::json params = nlohmann::json::array();
        for (const auto& paramInfo : m_parameters) {
            params.push_back(paramInfo.ToJSON());
        }
        json["parameters"] = std::move(params);
    }
    return json;
}

void SignatureHelp::FromJSON(const JSONItem& json)
{
    // Read the signatures
    m_signatures.clear();
    JSONItem signatures = json.namedObject("signatures");
    const int count = signatures.arraySize();
    for (int i = 0; i < count; ++i) {
        SignatureInformation si;
        si.FromJSON(signatures.arrayItem(i));
        m_signatures.push_back(si);
    }

    m_activeSignature = json.namedObject("activeSignature").toInt(0);
    m_activeParameter = json.namedObject("activeParameter").toInt(0);
}

nlohmann::json SignatureHelp::ToJSON() const
{
    nlohmann::json signatures = nlohmann::json::array();
    for (const SignatureInformation& si : m_signatures) {
        signatures.push_back(si.ToJSON());
    }
    return nlohmann::json{{"signatures", std::move(signatures)},
                          {"activeSignature", m_activeSignature},
                          {"activeParameter", m_activeParameter}};
}

void MarkupContent::FromJSON(const JSONItem& json)
{
    m_kind = json.namedObject("kind").toString();
    m_value = json.namedObject("value").toString();
}

nlohmann::json MarkupContent::ToJSON() const
{ return nlohmann::json{{"kind", m_kind.ToStdString(wxConvUTF8)}, {"value", m_value.ToStdString(wxConvUTF8)}}; }

void Hover::FromJSON(const JSONItem& json)
{
    m_contents.FromJSON(json.namedObject("contents"));
    m_range.FromJSON(json.namedObject("range"));
}

nlohmann::json Hover::ToJSON() const
{ return nlohmann::json{{"contents", m_contents.ToJSON()}, {"range", m_range.ToJSON()}}; }

///===------------------------------------------------------------------------
/// Diagnostic
///===------------------------------------------------------------------------
void Diagnostic::FromJSON(const JSONItem& json)
{
    m_range.FromJSON(json.namedObject("range"));
    m_message = json.namedObject("message").toString();
    m_severity = json.namedObject("severity").fromNumber(DiagnosticSeverity::Error);
}

nlohmann::json Diagnostic::ToJSON() const
{
    return nlohmann::json{{"range", m_range.ToJSON()},
                          {"message", GetMessage().ToStdString(wxConvUTF8)},
                          {"severity", static_cast<int>(m_severity)}};
}

TextDocumentContentChangeEvent& TextDocumentContentChangeEvent::SetText(const wxString& text)
{
    this->m_text.clear();
    if (!text.empty()) {
        this->m_text.reserve(text.length() + 1); // for the null
        this->m_text.append(text);
    }
    return *this;
}
//===----------------------------------------------------------------------------------
// DocumentSymbol
//===----------------------------------------------------------------------------------
void DocumentSymbol::FromJSON(const JSONItem& json)
{
    name = json["name"].toString();
    detail = json["detail"].toString();
    kind = (eSymbolKind)json["kind"].toInt(0);
    range.FromJSON(json["range"]);
    selectionRange.FromJSON(json["selectionRange"]);

    // read the children
    auto jsonChildren = json["children"];
    int size = jsonChildren.arraySize();
    children.clear();
    children.reserve(size);
    for (int i = 0; i < size; ++i) {
        auto child = jsonChildren[i];
        DocumentSymbol ds;
        ds.FromJSON(child);
        children.push_back(ds);
    }
}

nlohmann::json DocumentSymbol::ToJSON() const
{
    wxASSERT_MSG(false, "DocumentSymbol::ToJSON(): is not implemented");
    return nullptr;
}

//===----------------------------------------------------------------------------------
// DocumentSymbol
//===----------------------------------------------------------------------------------
void SymbolInformation::FromJSON(const JSONItem& json)
{
    name = json["name"].toString();
    containerName = json["containerName"].toString();
    kind = (eSymbolKind)json["kind"].toInt(0);
    location.FromJSON(json["location"]);

    // manipulate the data: if no container exists, extract it from the name
    if (containerName.empty() && !name.empty()) {
        int where = name.rfind("::");
        if (where == wxNOT_FOUND) {
            return;
        }

        wxString shortname = name.Mid(where + 2);
        containerName = name.Mid(0, where);
        name.swap(shortname);
    }
}

nlohmann::json SymbolInformation::ToJSON() const
{
    return nlohmann::json{{"kind", static_cast<int>(kind)},
                          {"containerName", containerName.ToStdString(wxConvUTF8)},
                          {"location", location.ToJSON()},
                          {"name", name.ToStdString(wxConvUTF8)}};
}

SymbolInformation SymbolInformation::From(const CTags::SymbolInfo& ctags_symbol_info)
{
    SymbolInformation si;
    si.name = ctags_symbol_info.name + ctags_symbol_info.signature;
    switch (ctags_symbol_info.kind) {
    case CTags::SymbolKind::kClass:
        si.kind = eSymbolKind::kSK_Class;
        break;
    case CTags::SymbolKind::kFunction:
        si.kind = eSymbolKind::kSK_Method;
        break;
    case CTags::SymbolKind::kGlobalMethod:
        si.kind = eSymbolKind::kSK_Function;
        break;
    case CTags::SymbolKind::kPrototype:
        si.kind = eSymbolKind::kSK_Function;
        break;
    case CTags::SymbolKind::kStruct:
        si.kind = eSymbolKind::kSK_Struct;
        break;
    case CTags::SymbolKind::kTrait:
        si.kind = eSymbolKind::kSK_Interface;
        break;
    default:
        si.kind = eSymbolKind::kSK_Variable;
        break;
    }
    Range range;
    range.SetStart(Position{ctags_symbol_info.line, 0});
    if (ctags_symbol_info.end_line.has_value()) {
        range.SetEnd(Position{ctags_symbol_info.end_line.value(), 0});
    }
    si.location.SetRange(range);
    si.location.SetPath(ctags_symbol_info.file);
    return si;
}

std::vector<SymbolInformation> SymbolInformation::From(const DocumentSymbol& document_symbol,
                                                       const wxString& container_name)
{
    std::vector<SymbolInformation> result;
    SymbolInformation si;
    si.name = document_symbol.GetName();
    si.kind = document_symbol.GetKind();
    si.containerName = container_name;

    Range range =
        document_symbol.GetSelectionRange().IsOk() ? document_symbol.GetSelectionRange() : document_symbol.GetRange();
    si.location.SetRange(range);
    result.push_back(si);
    if (!document_symbol.GetChildren().empty()) {
        wxString scope_name = si.name;
        for (const DocumentSymbol& child : document_symbol.GetChildren()) {
            auto children = From(child, scope_name);
            result.insert(result.end(), children.begin(), children.end());
        }
    }
    return result;
}

const wxString& URI::GetPath() const { return m_path; }
const wxString& URI::GetUrl() const { return m_uri; }
void URI::FromString(const wxString& str, URI* uri)
{
    uri->m_path = FileUtils::FilePathFromURI(str);
    uri->m_uri = FileUtils::FilePathToURI(str);
}

//===----------------------------------------------------------------------------------
// Command
//===----------------------------------------------------------------------------------
void Command::FromJSON(const JSONItem& json)
{
    m_title = json["title"].toString();
    m_command = json["command"].toString();

    // raw JSON
    m_arguments = json["arguments"].format(false);
}

// unimplemented
nlohmann::json Command::ToJSON() const { return nullptr; }

std::unordered_map<wxString, std::vector<LSP::TextEdit>> ParseWorkspaceEdit(const JSONItem& result)
{
    if (!result.isOk()) {
        return {};
    }

    LOG_IF_TRACE { LSP_TRACE() << result.format(false) << endl; }

    std::unordered_map<wxString, std::vector<LSP::TextEdit>> modifications;
    // some LSPs will reply with "changes" and some with "documentChanges" -> we support them both
    if (result.hasNamedObject("changes")) {
        auto changes = result["changes"];
        auto M = changes.GetAsMap();

        modifications.reserve(M.size());
        for (const auto& [filepath, json] : M) {
            int count = json.arraySize();
            std::vector<LSP::TextEdit> file_changes;
            file_changes.reserve(count);
            for (int i = 0; i < count; ++i) {
                auto e = json[i];
                LSP::TextEdit te;
                te.FromJSON(e);
                file_changes.push_back(te);
            }
            wxString path = FileUtils::FilePathFromURI(wxString(filepath.data(), filepath.length()));
            modifications.erase(path);
            modifications.insert({path, file_changes});
        }
    } else if (result.hasNamedObject("documentChanges")) {
        auto documentChanges = result["documentChanges"];
        int files_count = documentChanges.arraySize();
        for (int i = 0; i < files_count; ++i) {
            auto edits = documentChanges[i]["edits"];
            wxString filepath = documentChanges[i]["textDocument"]["uri"].toString();
            filepath = FileUtils::FilePathFromURI(filepath);
            std::vector<LSP::TextEdit> file_changes;
            int edits_count = edits.arraySize();
            file_changes.reserve(edits_count);
            for (int j = 0; j < edits_count; ++j) {
                auto e = edits[j];
                LSP::TextEdit te;
                te.FromJSON(e);
                file_changes.push_back(te);
            }
            modifications.erase(filepath);
            modifications.insert({filepath, file_changes});
        }
    }
    return modifications;
}

// =====---------------
// Progress
// =====---------------

std::optional<Progress> Progress::FromJSON(const JSONItem& json)
{
    if (!json.hasNamedObject("params")) {
        return std::nullopt;
    }

    if (!json["params"].hasNamedObject("value")) {
        return std::nullopt;
    }

    static const std::unordered_map<wxString, ProgressKind> ProgressKindMap{
        {"begin", ProgressKind::begin}, {"report", ProgressKind::report}, {"end", ProgressKind::end}};
    auto kind = json["params"]["value"]["kind"].toString();
    if (!ProgressKindMap.contains(kind)) {
        return std::nullopt;
    }

    Progress result;
    result.m_message = json["params"]["value"]["message"].toString();
    result.m_percentage = json["params"]["value"]["percentage"].toDouble(0.0);
    result.m_token = json["params"]["token"].toString();
    result.m_kind = ProgressKindMap.find(kind)->second;
    return result;
}
} // namespace LSP
