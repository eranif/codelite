#include "LSP/json_rpc_params.h"

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
namespace LSP
{
TextDocumentPositionParams::TextDocumentPositionParams() {}

void TextDocumentPositionParams::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_textDocument.FromJSON(json, pathConverter);
    m_position.FromJSON(json, pathConverter);
}

JSONItem TextDocumentPositionParams::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument", pathConverter));
    json.append(m_position.ToJSON("position", pathConverter));
    return json;
}

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------
DidOpenTextDocumentParams::DidOpenTextDocumentParams() {}

void DidOpenTextDocumentParams::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_textDocument.FromJSON(json, pathConverter);
}

JSONItem DidOpenTextDocumentParams::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument", pathConverter));
    return json;
}

//===----------------------------------------------------------------------------------
// DidCloseTextDocumentParams
//===----------------------------------------------------------------------------------
DidCloseTextDocumentParams::DidCloseTextDocumentParams() {}

void DidCloseTextDocumentParams::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_textDocument.FromJSON(json, pathConverter);
}

JSONItem DidCloseTextDocumentParams::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument", pathConverter));
    return json;
}

//===----------------------------------------------------------------------------------
// DidChangeTextDocumentParams
//===----------------------------------------------------------------------------------
DidChangeTextDocumentParams::DidChangeTextDocumentParams() {}

void DidChangeTextDocumentParams::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_textDocument.FromJSON(json, pathConverter);
    m_contentChanges.clear();
    if(json.hasNamedObject("contentChanges")) {
        JSONItem arr = json.namedObject("contentChanges");
        int count = arr.arraySize();
        for(int i = 0; i < count; ++i) {
            TextDocumentContentChangeEvent c;
            c.FromJSON(arr.arrayItem(i), pathConverter);
            m_contentChanges.push_back(c);
        }
    }
}

JSONItem DidChangeTextDocumentParams::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument", pathConverter));
    JSONItem arr = JSONItem::createArray("contentChanges");
    for(size_t i = 0; i < m_contentChanges.size(); ++i) {
        arr.arrayAppend(m_contentChanges[i].ToJSON("", pathConverter));
    }
    json.append(arr);
    return json;
}

//===----------------------------------------------------------------------------------
// DidSaveTextDocumentParams
//===----------------------------------------------------------------------------------
DidSaveTextDocumentParams::DidSaveTextDocumentParams() {}

void DidSaveTextDocumentParams::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    m_textDocument.FromJSON(json, pathConverter);
    m_text = json.namedObject("text").toString();
}

JSONItem DidSaveTextDocumentParams::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = JSONItem::createObject(name);
    json.append(m_textDocument.ToJSON("textDocument", pathConverter));
    json.addProperty("text", m_text);
    return json;
}

//===----------------------------------------------------------------------------------
// CompletionParams
//===----------------------------------------------------------------------------------
CompletionParams::CompletionParams() {}

void CompletionParams::FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter)
{
    TextDocumentPositionParams::FromJSON(json, pathConverter);
}

JSONItem CompletionParams::ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const
{
    JSONItem json = TextDocumentPositionParams::ToJSON(name, pathConverter);
    return json;
}
}; // namespace LSP
