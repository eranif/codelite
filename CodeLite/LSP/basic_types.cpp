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
}; // namespace LSP
