#include "LSP/basic_types.h"

#include "JSON.h"
#include "clModuleLogger.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"

#include <wx/filesys.h>

// our logger object
INITIALISE_MODULE_LOG(LSP_LOG_HANDLER, "LSP", "lsp.log");

namespace LSP
{

clModuleLogger& GetLogHandle() { return LSP_LOG_HANDLER; }

wxString FileNameToURI(const wxString& filename)
{
    wxString uri;
#ifdef __WXMSW__
    if(filename.StartsWith("/")) {
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

JSONItem TextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", GetPathAsURI());
    return json;
}

//===----------------------------------------------------------------------------------
// VersionedTextDocumentIdentifier
//===----------------------------------------------------------------------------------
void VersionedTextDocumentIdentifier::FromJSON(const JSONItem& json)
{
    TextDocumentIdentifier::FromJSON(json);
    m_version = json.namedObject("version").toInt(m_version);
}

JSONItem VersionedTextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONItem json = TextDocumentIdentifier::ToJSON(name);
    json.addProperty("version", m_version);
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

JSONItem Position::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("line", m_line);
    json.addProperty("character", m_character);
    return json;
}

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

JSONItem TextDocumentItem::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", GetPathAsURI())
        .addProperty("languageId", GetLanguageId())
        .addProperty("version", GetVersion())
        .addProperty("text", GetText());
    return json;
}
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
void TextDocumentContentChangeEvent::FromJSON(const JSONItem& json)
{
    m_text = json.namedObject("text").toString();
    if(json.hasNamedObject("range")) {
        m_range.FromJSON(json["range"]);
    }
}

JSONItem TextDocumentContentChangeEvent::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    if(m_range.IsOk()) {
        json.append(m_range.ToJSON("range"));
    }
    json.addProperty("text", m_text);
    return json;
}

void Range::FromJSON(const JSONItem& json)
{
    m_start.FromJSON(json["start"]);
    m_end.FromJSON(json["end"]);
}

JSONItem Range::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_start.ToJSON("start"));
    json.append(m_end.ToJSON("end"));
    return json;
}

void Location::FromJSON(const JSONItem& json)
{
    URI::FromString(json.namedObject("uri").toString(), &m_uri);
    m_range.FromJSON(json.namedObject("range"));
    m_pattern = json["pattern"].toString();
    m_name = json["name"].toString();
}

JSONItem Location::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", GetPathAsURI());
    json.append(m_range.ToJSON("range"));
    json.addProperty("pattern", m_pattern);
    json.addProperty("name", m_name);
    return json;
}

void TextEdit::FromJSON(const JSONItem& json)
{
    m_range.FromJSON(json.namedObject("range"));
    m_newText = json.namedObject("newText").toString();
}

JSONItem TextEdit::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("newText", m_newText);
    json.append(m_range.ToJSON("range"));
    return json;
}

void ParameterInformation::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_documentation = json.namedObject("documentation").toString();
}

JSONItem ParameterInformation::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    return json;
}

void SignatureInformation::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_documentation = json.namedObject("documentation").toString();
    m_parameters.clear();
    if(json.hasNamedObject("parameters")) {
        JSONItem parameters = json.namedObject("parameters");
        const int size = parameters.arraySize();
        if(size > 0) {
            m_parameters.reserve(size);
            for(int i = 0; i < size; ++i) {
                ParameterInformation p;
                p.FromJSON(parameters.arrayItem(i));
                m_parameters.push_back(p);
            }
        }
    }
}

JSONItem SignatureInformation::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    if(!m_parameters.empty()) {
        JSONItem params = JSONItem::createArray("parameters");
        json.append(params);
        for(size_t i = 0; i < m_parameters.size(); ++i) {
            params.append(m_parameters.at(i).ToJSON(""));
        }
    }
    return json;
}

void SignatureHelp::FromJSON(const JSONItem& json)
{
    // Read the signatures
    m_signatures.clear();
    JSONItem signatures = json.namedObject("signatures");
    const int count = signatures.arraySize();
    for(int i = 0; i < count; ++i) {
        SignatureInformation si;
        si.FromJSON(signatures.arrayItem(i));
        m_signatures.push_back(si);
    }

    m_activeSignature = json.namedObject("activeSignature").toInt(0);
    m_activeParameter = json.namedObject("activeParameter").toInt(0);
}

JSONItem SignatureHelp::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    JSONItem signatures = JSONItem::createArray("signatures");
    json.append(signatures);
    for(const SignatureInformation& si : m_signatures) {
        signatures.arrayAppend(si.ToJSON(""));
    }
    json.addProperty("activeSignature", m_activeSignature);
    json.addProperty("activeParameter", m_activeParameter);
    return json;
}

void MarkupContent::FromJSON(const JSONItem& json)
{
    m_kind = json.namedObject("kind").toString();
    m_value = json.namedObject("value").toString();
}

JSONItem MarkupContent::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("kind", m_kind);
    json.addProperty("value", m_value);
    return json;
}

void Hover::FromJSON(const JSONItem& json)
{
    m_contents.FromJSON(json.namedObject("contents"));
    m_range.FromJSON(json.namedObject("range"));
}

JSONItem Hover::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_contents.ToJSON("contents"));
    json.append(m_range.ToJSON("range"));
    return json;
}

///===------------------------------------------------------------------------
/// Diagnostic
///===------------------------------------------------------------------------
void Diagnostic::FromJSON(const JSONItem& json)
{
    m_range.FromJSON(json.namedObject("range"));
    m_message = json.namedObject("message").toString();
    m_severity = json.namedObject("severity").fromNumber(DiagnosticSeverity::Error);
}

JSONItem Diagnostic::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_range.ToJSON("range"));
    json.addProperty("message", GetMessage());
    json.addProperty("severity", (int)m_severity);
    return json;
}

TextDocumentContentChangeEvent& TextDocumentContentChangeEvent::SetText(const wxString& text)
{
    this->m_text.clear();
    if(!text.empty()) {
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
    for(int i = 0; i < size; ++i) {
        auto child = jsonChildren[i];
        DocumentSymbol ds;
        ds.FromJSON(child);
        children.push_back(ds);
    }
}

JSONItem DocumentSymbol::ToJSON(const wxString& name) const
{
    wxASSERT_MSG(false, "DocumentSymbol::ToJSON(): is not implemented");
    return JSONItem(nullptr);
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
    if(containerName.empty() && !name.empty()) {
        int where = name.rfind("::");
        if(where == wxNOT_FOUND) {
            return;
        }

        wxString shortname = name.Mid(where + 2);
        containerName = name.Mid(0, where);
        name.swap(shortname);
    }
}

JSONItem SymbolInformation::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("kind", (int)kind);
    json.addProperty("containerName", containerName);
    json.append(location.ToJSON("location"));
    json.addProperty("name", this->name);
    return json;
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
JSONItem Command::ToJSON(const wxString& name) const { return {}; }

std::unordered_map<wxString, std::vector<LSP::TextEdit>> ParseWorkspaceEdit(const JSONItem& result)
{
    if(!result.isOk()) {
        return {};
    }

    LOG_IF_TRACE { LSP_TRACE() << result.format(false) << endl; }

    std::unordered_map<wxString, std::vector<LSP::TextEdit>> modifications;
    // some LSPs will reply with "changes" and some with "documentChanges" -> we support them both
    if(result.hasNamedObject("changes")) {
        auto changes = result["changes"];
        auto M = changes.GetAsMap();

        modifications.reserve(M.size());
        for(const auto& [filepath, json] : M) {
            int count = json.arraySize();
            std::vector<LSP::TextEdit> file_changes;
            file_changes.reserve(count);
            for(int i = 0; i < count; ++i) {
                auto e = json[i];
                LSP::TextEdit te;
                te.FromJSON(e);
                file_changes.push_back(te);
            }
            wxString path = FileUtils::FilePathFromURI(wxString(filepath.data(), filepath.length()));
            modifications.erase(path);
            modifications.insert({ path, file_changes });
        }
    } else if(result.hasNamedObject("documentChanges")) {
        auto documentChanges = result["documentChanges"];
        int files_count = documentChanges.arraySize();
        for(int i = 0; i < files_count; ++i) {
            auto edits = documentChanges[i]["edits"];
            wxString filepath = documentChanges[i]["textDocument"]["uri"].toString();
            filepath = FileUtils::FilePathFromURI(filepath);
            std::vector<LSP::TextEdit> file_changes;
            int edits_count = edits.arraySize();
            file_changes.reserve(edits_count);
            for(int j = 0; j < edits_count; ++j) {
                auto e = edits[j];
                LSP::TextEdit te;
                te.FromJSON(e);
                file_changes.push_back(te);
            }
            modifications.erase(filepath);
            modifications.insert({ filepath, file_changes });
        }
    }
    return std::move(modifications);
}
}; // namespace LSP
