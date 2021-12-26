#include "LSP/json_rpc_params.h"

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
namespace LSP
{
TextDocumentPositionParams::TextDocumentPositionParams() {}

void TextDocumentPositionParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_position.FromJSON(json["position"]);
}

JSONItem TextDocumentPositionParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    json.append(m_position.ToJSON("position"));
    return json;
}
//===----------------------------------------------
// SemanticTokensParams
//===----------------------------------------------
SemanticTokensParams::SemanticTokensParams() {}

void SemanticTokensParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

JSONItem SemanticTokensParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------
DidOpenTextDocumentParams::DidOpenTextDocumentParams() {}

void DidOpenTextDocumentParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

JSONItem DidOpenTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidCloseTextDocumentParams
//===----------------------------------------------------------------------------------
DidCloseTextDocumentParams::DidCloseTextDocumentParams() {}

void DidCloseTextDocumentParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

JSONItem DidCloseTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidChangeTextDocumentParams
//===----------------------------------------------------------------------------------
DidChangeTextDocumentParams::DidChangeTextDocumentParams() {}

void DidChangeTextDocumentParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_contentChanges.clear();
    if(json.hasNamedObject("contentChanges")) {
        JSONItem arr = json.namedObject("contentChanges");
        int count = arr.arraySize();
        for(int i = 0; i < count; ++i) {
            TextDocumentContentChangeEvent c;
            c.FromJSON(arr.arrayItem(i));
            m_contentChanges.push_back(c);
        }
    }
}

JSONItem DidChangeTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    JSONItem arr = JSONItem::createArray("contentChanges");
    for(size_t i = 0; i < m_contentChanges.size(); ++i) {
        arr.arrayAppend(m_contentChanges[i].ToJSON(""));
    }
    json.append(arr);
    return json;
}

//===----------------------------------------------------------------------------------
// DidSaveTextDocumentParams
//===----------------------------------------------------------------------------------
DidSaveTextDocumentParams::DidSaveTextDocumentParams() {}

void DidSaveTextDocumentParams::FromJSON(const JSONItem& json)
{
    m_textDocument.FromJSON(json["textDocument"]);
    m_text = json["text"].toString();
}

JSONItem DidSaveTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    json.addProperty("text", m_text);
    return json;
}

//===----------------------------------------------------------------------------------
// CompletionParams
//===----------------------------------------------------------------------------------
CompletionParams::CompletionParams() {}

void CompletionParams::FromJSON(const JSONItem& json) { TextDocumentPositionParams::FromJSON(json); }

JSONItem CompletionParams::ToJSON(const wxString& name) const
{
    JSONItem json = TextDocumentPositionParams::ToJSON(name);
    return json;
}

//===----------------------------------------------------------------------------------
// DocumentSymbolParams
//===----------------------------------------------------------------------------------
DocumentSymbolParams::DocumentSymbolParams() {}

void DocumentSymbolParams::FromJSON(const JSONItem& json) { m_textDocument.FromJSON(json["textDocument"]); }

JSONItem DocumentSymbolParams::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument"));
    return json;
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

JSONItem ReferenceParams::ToJSON(const wxString& name) const
{
    JSONItem json = TextDocumentPositionParams::ToJSON(name);
    auto context = json.AddObject("context");
    context.addProperty("includeDeclaration", m_includeDeclaration);
    return json;
}
}; // namespace LSP
