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

void DidOpenTextDocumentParams::FromJSON(const JSONElement& json) { m_textDocument.FromJSON(json); }

JSONElement DidOpenTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidCloseTextDocumentParams
//===----------------------------------------------------------------------------------
DidCloseTextDocumentParams::DidCloseTextDocumentParams() {}

void DidCloseTextDocumentParams::FromJSON(const JSONElement& json) { m_textDocument.FromJSON(json); }

JSONElement DidCloseTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidChangeTextDocumentParams
//===----------------------------------------------------------------------------------
DidChangeTextDocumentParams::DidChangeTextDocumentParams() {}

void DidChangeTextDocumentParams::FromJSON(const JSONElement& json)
{
    m_textDocument.FromJSON(json);
    m_contentChanges.clear();
    if(json.hasNamedObject("contentChanges")) {
        JSONElement arr = json.namedObject("contentChanges");
        int count = arr.arraySize();
        for(int i = 0; i < count; ++i) {
            TextDocumentContentChangeEvent c;
            c.FromJSON(arr.arrayItem(i));
            m_contentChanges.push_back(c);
        }
    }
}

JSONElement DidChangeTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    JSONElement arr = JSONElement::createArray("contentChanges");
    for(size_t i = 0; i < m_contentChanges.size(); ++i) {
        arr.arrayAppend(m_contentChanges[i].ToJSON(""));
    }
    json.append(arr);
    return json;
}

}; // namespace json_rpc
