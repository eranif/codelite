#ifndef JSONRPC_PARAMS_H
#define JSONRPC_PARAMS_H

#include "JSON.h"
#include "LSP/JSONObject.h"
#include "basic_types.h"
#include "codelite_exports.h"

#include <memory>
#include <vector>

namespace LSP
{
//===----------------------------------------------------------------------------------
// Params
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Params : public Serializable
{
public:
    using Ptr_t = std::shared_ptr<Params>;

public:
    Params() = default;
    virtual ~Params() = default;
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
    TextDocumentPositionParams() = default;
    virtual ~TextDocumentPositionParams() = default;

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    void SetPosition(const Position& position) { this->m_position = position; }
    void SetTextDocument(const TextDocumentIdentifier& textDocument) { this->m_textDocument = textDocument; }
    const Position& GetPosition() const { return m_position; }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
};

//===----------------------------------------------------------------------------------
// RenameParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL RenameParams : public TextDocumentPositionParams
{
    /**
     * The new name of the symbol. If the given name is not valid the
     * request must return a [ResponseError](#ResponseError) with an
     * appropriate message set.
     */
    wxString m_newName;

public:
    RenameParams() = default;
    virtual ~RenameParams() = default;

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    void SetNewName(const wxString& newName) { this->m_newName = newName; }
    const wxString& GetNewName() const { return m_newName; }
};

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL ReferenceParams : public TextDocumentPositionParams
{
    bool m_includeDeclaration = true;

public:
    ReferenceParams(bool includeDeclaration);
    virtual ~ReferenceParams() = default;

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
    SemanticTokensParams() = default;
    virtual ~SemanticTokensParams() = default;

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
    DocumentSymbolParams() = default;
    virtual ~DocumentSymbolParams() = default;

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
    CompletionParams() = default;
    virtual ~CompletionParams() = default;

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
};

//===----------------------------------------------------------------------------------
// CompletionParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL ExecuteCommandParams : public Params
{
    wxString m_command;
    wxString m_arguments;

public:
    ExecuteCommandParams(const wxString& command, const wxString& arguments);
    virtual ~ExecuteCommandParams() = default;

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON(const wxString& name) const override;
};

//===----------------------------------------------------------------------------------
// CodeActionParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL CodeActionParams : public Params
{
    TextDocumentIdentifier m_textDocument;
    Range m_range;
    std::vector<LSP::Diagnostic> m_diagnostics;

public:
    CodeActionParams() = default;
    virtual ~CodeActionParams() = default;

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON(const wxString& name) const override;

    void SetTextDocument(const TextDocumentIdentifier& textDocument) { this->m_textDocument = textDocument; }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }

    void SetRange(const Range& range) { this->m_range = range; }
    const Range& GetRange() const { return m_range; }

    void SetDiagnostics(const std::vector<LSP::Diagnostic>& diagnostics) { this->m_diagnostics = diagnostics; }
    const std::vector<LSP::Diagnostic>& GetDiagnostics() const { return this->m_diagnostics; }
};

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL DidOpenTextDocumentParams : public Params
{
    TextDocumentItem m_textDocument;

public:
    DidOpenTextDocumentParams() = default;
    virtual ~DidOpenTextDocumentParams() = default;

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
    DidCloseTextDocumentParams() = default;
    virtual ~DidCloseTextDocumentParams() = default;

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
    DidChangeTextDocumentParams() = default;
    virtual ~DidChangeTextDocumentParams() = default;

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
    DidSaveTextDocumentParams() = default;
    virtual ~DidSaveTextDocumentParams() = default;

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
