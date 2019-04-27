#include "LSP/basic_types.h"
#include <wx/filesys.h>
#include "JSON.h"
#include <wx/filesys.h>

//===------------------------------------------------
// Request
//===------------------------------------------------
namespace LSP
{

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
void TextDocumentIdentifier::FromJSON(const JSONItem& json)
{
    wxString uri = json.namedObject("uri").toString();
    m_filename = wxFileSystem::URLToFileName(uri);
}

JSONItem TextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(m_filename));
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
    m_line = json.namedObject("line").toInt(0);
    m_character = json.namedObject("character").toInt(0);
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
    m_uri = wxFileSystem::URLToFileName(json.namedObject("uri").toString());
    m_uri.Normalize();
    m_languageId = json.namedObject("languageId").toString();
    m_version = json.namedObject("version").toInt();
    m_text = json.namedObject("text").toString();
}

JSONItem TextDocumentItem::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(GetUri()))
        .addProperty("languageId", GetLanguageId())
        .addProperty("version", GetVersion())
        .addProperty("text", GetText());
    return json;
}
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
void TextDocumentContentChangeEvent::FromJSON(const JSONItem& json) { m_text = json.namedObject("text").toString(); }

JSONItem TextDocumentContentChangeEvent::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("text", m_text);
    return json;
}

void Range::FromJSON(const JSONItem& json)
{
    m_start.FromJSON(json.namedObject("start"));
    m_end.FromJSON(json.namedObject("end"));
}

JSONItem Range::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_start.ToJSON("start"));
    json.append(m_start.ToJSON("end"));
    return json;
}

void Location::FromJSON(const JSONItem& json)
{
    wxFileName fn = wxFileSystem::URLToFileName(json.namedObject("uri").toString());
    fn.Normalize();
    m_uri = fn.GetFullPath();
    m_range.FromJSON(json.namedObject("range"));
}

JSONItem Location::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(m_uri));
    json.append(m_range.ToJSON("range"));
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
}; // namespace LSP

void LSP::ParameterInformation::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_documentation = json.namedObject("documentation").toString();
}

JSONItem LSP::ParameterInformation::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    return json;
}

void LSP::SignatureInformation::FromJSON(const JSONItem& json)
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

JSONItem LSP::SignatureInformation::ToJSON(const wxString& name) const
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

void LSP::SignatureHelp::FromJSON(const JSONItem& json)
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

JSONItem LSP::SignatureHelp::ToJSON(const wxString& name) const
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

///===------------------------------------------------------------------------
/// Diagnostic
///===------------------------------------------------------------------------
void LSP::Diagnostic::FromJSON(const JSONItem& json)
{
    m_range.FromJSON(json.namedObject("range"));
    m_message = json.namedObject("message").toString();
}

JSONItem LSP::Diagnostic::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_range.ToJSON("range"));
    json.addProperty("message", GetMessage());
    return json;
}
