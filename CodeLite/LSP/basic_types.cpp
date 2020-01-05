#include "JSON.h"
#include "LSP/basic_types.h"
#include "file_logger.h"
#include <wx/filesys.h>

//===------------------------------------------------
// Request
//===------------------------------------------------
namespace LSP
{

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
void TextDocumentIdentifier::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    wxString uri = json.namedObject("uri").toString();
    m_filename = pathConverter->ConvertFrom(uri);
}

JSONItem TextDocumentIdentifier::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", pathConverter->ConvertTo(m_filename.GetFullPath()));
    return json;
}

//===----------------------------------------------------------------------------------
// VersionedTextDocumentIdentifier
//===----------------------------------------------------------------------------------
void VersionedTextDocumentIdentifier::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    TextDocumentIdentifier::FromJSON(json, pathConverter);
    m_version = json.namedObject("version").toInt(m_version);
}

JSONItem VersionedTextDocumentIdentifier::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = TextDocumentIdentifier::ToJSON(name, pathConverter);
    json.addProperty("version", m_version);
    return json;
}

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
void Position::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_line = json.namedObject("line").toInt(0);
    m_character = json.namedObject("character").toInt(0);
}

JSONItem Position::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("line", m_line);
    json.addProperty("character", m_character);
    return json;
}

//===----------------------------------------------------------------------------------
// TextDocumentItem
//===----------------------------------------------------------------------------------
void TextDocumentItem::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_uri = pathConverter->ConvertFrom(json.namedObject("uri").toString());
    m_uri.Normalize();
    m_languageId = json.namedObject("languageId").toString();
    m_version = json.namedObject("version").toInt();
    m_text = json.namedObject("text").toString();
}

JSONItem TextDocumentItem::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", pathConverter->ConvertTo(GetUri().GetFullPath()))
        .addProperty("languageId", GetLanguageId())
        .addProperty("version", GetVersion())
        .addProperty("text", GetText());
    return json;
}
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
void TextDocumentContentChangeEvent::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_text = json.namedObject("text").toString();
}

JSONItem TextDocumentContentChangeEvent::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("text", m_text);
    return json;
}

void Range::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_start.FromJSON(json.namedObject("start"), pathConverter);
    m_end.FromJSON(json.namedObject("end"), pathConverter);
}

JSONItem Range::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_start.ToJSON("start", pathConverter));
    json.append(m_start.ToJSON("end", pathConverter));
    return json;
}

void Location::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    wxFileName fn = pathConverter->ConvertFrom(json.namedObject("uri").toString());
    fn.Normalize();
    m_uri = fn.GetFullPath();
    m_range.FromJSON(json.namedObject("range"), pathConverter);
}

JSONItem Location::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("uri", pathConverter->ConvertTo(m_uri));
    json.append(m_range.ToJSON("range", pathConverter));
    return json;
}

void TextEdit::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_range.FromJSON(json.namedObject("range"), pathConverter);
    m_newText = json.namedObject("newText").toString();
}

JSONItem TextEdit::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("newText", m_newText);
    json.append(m_range.ToJSON("range", pathConverter));
    return json;
}
}; // namespace LSP

void LSP::ParameterInformation::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_label = json.namedObject("label").toString();
    m_documentation = json.namedObject("documentation").toString();
}

JSONItem LSP::ParameterInformation::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    return json;
}

void LSP::SignatureInformation::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
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
                p.FromJSON(parameters.arrayItem(i), pathConverter);
                m_parameters.push_back(p);
            }
        }
    }
}

JSONItem LSP::SignatureInformation::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    if(!m_parameters.empty()) {
        JSONItem params = JSONItem::createArray("parameters");
        json.append(params);
        for(size_t i = 0; i < m_parameters.size(); ++i) {
            params.append(m_parameters.at(i).ToJSON("", pathConverter));
        }
    }
    return json;
}

void LSP::SignatureHelp::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    // Read the signatures
    m_signatures.clear();
    JSONItem signatures = json.namedObject("signatures");
    const int count = signatures.arraySize();
    for(int i = 0; i < count; ++i) {
        SignatureInformation si;
        si.FromJSON(signatures.arrayItem(i), pathConverter);
        m_signatures.push_back(si);
    }

    m_activeSignature = json.namedObject("activeSignature").toInt(0);
    m_activeParameter = json.namedObject("activeParameter").toInt(0);
}

JSONItem LSP::SignatureHelp::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    JSONItem signatures = JSONItem::createArray("signatures");
    json.append(signatures);
    for(const SignatureInformation& si : m_signatures) {
        signatures.arrayAppend(si.ToJSON("", pathConverter));
    }
    json.addProperty("activeSignature", m_activeSignature);
    json.addProperty("activeParameter", m_activeParameter);
    return json;
}

///===------------------------------------------------------------------------
/// Diagnostic
///===------------------------------------------------------------------------
void LSP::Diagnostic::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_range.FromJSON(json.namedObject("range"), pathConverter);
    m_message = json.namedObject("message").toString();
}

JSONItem LSP::Diagnostic::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_range.ToJSON("range", pathConverter));
    json.addProperty("message", GetMessage());
    return json;
}

LSP::TextDocumentContentChangeEvent& LSP::TextDocumentContentChangeEvent::SetText(const std::string& text)
{
    this->m_text.clear();
    if(!text.empty()) {
        this->m_text.reserve(text.length() + 1); // for the null
        this->m_text.append(text);
    }
    return *this;
}
