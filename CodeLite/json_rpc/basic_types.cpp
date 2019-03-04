#include "basic_types.h"
#include <wx/filesys.h>

//===------------------------------------------------
// Request
//===------------------------------------------------
namespace json_rpc
{

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
void TextDocumentIdentifier::FromJSON(const JSONElement& json)
{
    wxString uri = json.namedObject("uri").toString();
    m_filename = wxFileSystem::URLToFileName(uri);
}

JSONElement TextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(m_filename));
    return json;
}
//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
void Position::FromJSON(const JSONElement& json)
{
    m_line = json.namedObject("line").toInt(0);
    m_character = json.namedObject("character").toInt(0);
}

JSONElement Position::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("line", m_line);
    json.addProperty("character", m_character);
    return json;
}

//===----------------------------------------------------------------------------------
// TextDocumentItem
//===----------------------------------------------------------------------------------
void TextDocumentItem::FromJSON(const JSONElement& json)
{
    m_uri = wxFileSystem::URLToFileName(json.namedObject("uri").toString());
    m_languageId = json.namedObject("languageId").toString();
    m_version = json.namedObject("version").toInt();
    m_text = json.namedObject("text").toString();
}

JSONElement TextDocumentItem::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(GetUri()))
        .addProperty("languageId", GetLanguageId())
        .addProperty("version", GetVersion())
        .addProperty("text", GetText());
    return json;
}
}; // namespace json_rpc
