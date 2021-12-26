#ifndef JSONRPC_PARAMS_H
#define JSONRPC_PARAMS_H

#include "IPathConverter.hpp"
#include "JSON.h"
#include "LSP/JSONObject.h"
#include "basic_types.h"
#include "codelite_exports.h"

#include <vector>
#include <wx/sharedptr.h>

namespace LSP
{
//===----------------------------------------------------------------------------------
// Params
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Params : public Serializable
{
public:
    typedef wxSharedPtr<Params> Ptr_t;

public:
    Params() {}
    virtual ~Params() {}
    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Params*>(this)); }
};

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL TextDocumentPositionParams : public Params
{
    TextDocumentIdentifier m_textDocument;
    Position m_position;

public:
    TextDocumentPositionParams();
    virtual ~TextDocumentPositionParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    void SetPosition(const Position& position) { this->m_position = position; }
    void SetTextDocument(const TextDocumentIdentifier& textDocument) { this->m_textDocument = textDocument; }
    const Position& GetPosition() const { return m_position; }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
};

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL ReferenceParams : public TextDocumentPositionParams
{
    bool m_includeDeclaration = true;

public:
    ReferenceParams(bool includeDeclaration);
    virtual ~ReferenceParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    void SetIncludeDeclaration(bool includeDeclaration) { this->m_includeDeclaration = includeDeclaration; }
    bool IsIncludeDeclaration() const { return m_includeDeclaration; }
};

//===----------------------------------------------------------------------------------
// SemanticTokensParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL SemanticTokensParams : public Params
{
    TextDocumentIdentifier m_textDocument;

public:
    SemanticTokensParams();
    virtual ~SemanticTokensParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    void SetTextDocument(const TextDocumentIdentifier& textDocument) { this->m_textDocument = textDocument; }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
};

struct WXDLLIMPEXP_CL SemanticTokenRange {
    int line = wxNOT_FOUND;
    int column = wxNOT_FOUND;
    int length = wxNOT_FOUND;
    int token_type = wxNOT_FOUND;
};

//===----------------------------------------------------------------------------------
// DocumentSymbolParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL DocumentSymbolParams : public Params
{
    TextDocumentIdentifier m_textDocument;

public:
    DocumentSymbolParams();
    virtual ~DocumentSymbolParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    void SetTextDocument(const TextDocumentIdentifier& textDocument) { this->m_textDocument = textDocument; }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
};

//===----------------------------------------------------------------------------------
// CompletionParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL CompletionParams : public TextDocumentPositionParams
{
public:
    CompletionParams();
    virtual ~CompletionParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
};

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL DidOpenTextDocumentParams : public Params
{
    TextDocumentItem m_textDocument;

public:
    DidOpenTextDocumentParams();
    virtual ~DidOpenTextDocumentParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    DidOpenTextDocumentParams& SetTextDocument(const TextDocumentItem& textDocument)
    {
        this->m_textDocument = textDocument;
        return *this;
    }
    const TextDocumentItem& GetTextDocument() const { return m_textDocument; }
};

//===----------------------------------------------------------------------------------
// DidCloseTextDocumentParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL DidCloseTextDocumentParams : public Params
{
    TextDocumentIdentifier m_textDocument;

public:
    DidCloseTextDocumentParams();
    virtual ~DidCloseTextDocumentParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    DidCloseTextDocumentParams& SetTextDocument(const TextDocumentIdentifier& textDocument)
    {
        this->m_textDocument = textDocument;
        return *this;
    }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
};

//===----------------------------------------------------------------------------------
// DidChangeTextDocumentParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL DidChangeTextDocumentParams : public Params
{
    VersionedTextDocumentIdentifier m_textDocument;
    std::vector<TextDocumentContentChangeEvent> m_contentChanges;

public:
    DidChangeTextDocumentParams();
    virtual ~DidChangeTextDocumentParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    DidChangeTextDocumentParams& SetContentChanges(const std::vector<TextDocumentContentChangeEvent>& contentChanges)
    {
        this->m_contentChanges = contentChanges;
        return *this;
    }
    DidChangeTextDocumentParams& SetTextDocument(const VersionedTextDocumentIdentifier& textDocument)
    {
        this->m_textDocument = textDocument;
        return *this;
    }
    const std::vector<TextDocumentContentChangeEvent>& GetContentChanges() const { return m_contentChanges; }
    const VersionedTextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
};

//===----------------------------------------------------------------------------------
// DidSaveTextDocumentParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL DidSaveTextDocumentParams : public Params
{
    TextDocumentIdentifier m_textDocument;
    wxString m_text;

public:
    DidSaveTextDocumentParams();
    virtual ~DidSaveTextDocumentParams() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    DidSaveTextDocumentParams& SetTextDocument(const TextDocumentIdentifier& textDocument)
    {
        this->m_textDocument = textDocument;
        return *this;
    }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
    DidSaveTextDocumentParams& SetText(const wxString& text)
    {
        this->m_text = text;
        return *this;
    }
    const wxString& GetText() const { return m_text; }
};

};     // namespace LSP
#endif // JSONRPC_PARAMS_H
