#ifndef JSONRPC_PARAMS_H
#define JSONRPC_PARAMS_H

#include "codelite_exports.h"
#include "json_node.h"
#include "json_rpc/JSONObject.h"
#include <wx/sharedptr.h>
#include "basic_types.h"

namespace json_rpc
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

    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON(const wxString& name) const;

    void SetPosition(const Position& position) { this->m_position = position; }
    void SetTextDocument(const TextDocumentIdentifier& textDocument) { this->m_textDocument = textDocument; }
    const Position& GetPosition() const { return m_position; }
    const TextDocumentIdentifier& GetTextDocument() const { return m_textDocument; }
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

    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON(const wxString& name) const;
    
    DidOpenTextDocumentParams& SetTextDocument(const TextDocumentItem& textDocument)
    {
        this->m_textDocument = textDocument;
        return *this;
    }
    const TextDocumentItem& GetTextDocument() const { return m_textDocument; }
};

};     // namespace json_rpc
#endif // JSONRPC_PARAMS_H
