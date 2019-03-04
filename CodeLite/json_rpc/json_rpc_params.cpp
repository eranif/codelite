#include "json_rpc/json_rpc_params.h"

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
namespace json_rpc
{
TextDocumentPositionParams::TextDocumentPositionParams() {}

void TextDocumentPositionParams::FromJSON(const JSONElement& json)
{
    m_textDocument.FromJSON(json);
    m_position.FromJSON(json);
}

JSONElement TextDocumentPositionParams::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    json.append(m_position.ToJSON("position"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------
DidOpenTextDocumentParams::DidOpenTextDocumentParams() {}

void DidOpenTextDocumentParams::FromJSON(const JSONElement& json)
{
    m_textDocument.FromJSON(json);
}

JSONElement DidOpenTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
}

}; // namespace json_rpc