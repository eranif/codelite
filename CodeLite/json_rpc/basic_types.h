#ifndef JSONRPC_BASICTYPES_H
#define JSONRPC_BASICTYPES_H

#include "codelite_exports.h"
#include "json_node.h"
#include "JSONObject.h"
#include <wx/sharedptr.h>

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
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL TextDocumentIdentifier : public Serializable
{
    wxFileName m_filename;

public:
    virtual JSONElement ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONElement& json);

    TextDocumentIdentifier() {}
    TextDocumentIdentifier(const wxFileName& filename)
        : m_filename(filename)
    {
    }
    virtual ~TextDocumentIdentifier() {}
    TextDocumentIdentifier& SetFilename(const wxFileName& filename)
    {
        this->m_filename = filename;
        return *this;
    }
    const wxFileName& GetFilename() const { return m_filename; }
};

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Position : public Serializable
{
    int m_line;
    int m_character;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON(const wxString& name) const;

    Position(int line, int col)
        : m_line(line)
        , m_character(col)
    {
    }
    Position() {}
    virtual ~Position() {}
    Position& SetCharacter(int character)
    {
        this->m_character = character;
        return *this;
    }
    Position& SetLine(int line)
    {
        this->m_line = line;
        return *this;
    }
    int GetCharacter() const { return m_character; }
    int GetLine() const { return m_line; }
};

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
};     // namespace json_rpc
#endif // JSONRPC_BASICTYPES_H
