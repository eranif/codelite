#ifndef JSONRPC_BASICTYPES_H
#define JSONRPC_BASICTYPES_H

#include "IPathConverter.hpp"
#include "JSON.h"
#include "JSONObject.h"
#include "codelite_exports.h"
#include "fileutils.h"

#include <vector>
#include <wx/sharedptr.h>

namespace LSP
{
enum eSymbolKind {
    kSK_File = 1,
    kSK_Module = 2,
    kSK_Namespace = 3,
    kSK_Package = 4,
    kSK_Class = 5,
    kSK_Method = 6,
    kSK_Property = 7,
    kSK_Field = 8,
    kSK_Constructor = 9,
    kSK_Enum = 10,
    kSK_Interface = 11,
    kSK_Function = 12,
    kSK_Variable = 13,
    kSK_Constant = 14,
    kSK_String = 15,
    kSK_Number = 16,
    kSK_Boolean = 17,
    kSK_Array = 18,
    kSK_Object = 19,
    kSK_Key = 20,
    kSK_Null = 21,
    kSK_EnumMember = 22,
    kSK_Struct = 23,
    kSK_Event = 24,
    kSK_Operator = 25,
    kSK_TypeParameter = 26,
};

class WXDLLIMPEXP_CL URI
{
    wxString m_path;
    wxString m_uri;

private:
    URI(const wxString& str) = delete;
    URI& operator=(const wxString& str) = delete;

public:
    URI() {}
    ~URI() {}

    static void FromString(const wxString& str, URI* o);

    const wxString& GetPath() const;
    const wxString& GetUrl() const;
};
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL TextDocumentContentChangeEvent : public Serializable
{
    wxString m_text;

public:
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    TextDocumentContentChangeEvent() {}
    TextDocumentContentChangeEvent(const wxString& text)
        : m_text(text)
    {
    }
    virtual ~TextDocumentContentChangeEvent() {}
    TextDocumentContentChangeEvent& SetText(const wxString& text);
    const wxString& GetText() const { return m_text; }
};

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL TextDocumentIdentifier : public Serializable
{
    URI m_filename;

public:
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    TextDocumentIdentifier() {}
    TextDocumentIdentifier(const wxString& filename) { URI::FromString(filename, &m_filename); }

    virtual ~TextDocumentIdentifier() {}
    TextDocumentIdentifier& SetFilename(const wxString& filename)
    {
        URI::FromString(filename, &m_filename);
        return *this;
    }
    const wxString& GetPath() const { return m_filename.GetPath(); }
    const wxString& GetPathAsURI() const { return m_filename.GetUrl(); }
};

//===----------------------------------------------------------------------------------
// VersionedTextDocumentIdentifier
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL VersionedTextDocumentIdentifier : public TextDocumentIdentifier
{
    int m_version = 1;

public:
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    VersionedTextDocumentIdentifier() {}
    VersionedTextDocumentIdentifier(int version)
        : m_version(version)
    {
    }
    virtual ~VersionedTextDocumentIdentifier() {}
    VersionedTextDocumentIdentifier& SetVersion(int version)
    {
        this->m_version = version;
        return *this;
    }
    int GetVersion() const { return m_version; }
};

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Position : public Serializable
{
    int m_line = wxNOT_FOUND;
    int m_character = wxNOT_FOUND;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    Position(int line, int col)
        : m_line(line)
        , m_character(col)
    {
    }
    Position() {}
    virtual ~Position() {}
    bool operator==(const Position& rhs) const
    {
        return this->m_line == rhs.m_line && this->m_character == rhs.m_character;
    }
    bool operator!=(const Position& rhs) const { return !(*this == rhs); }
    bool operator<(const Position& rhs) const
    {
        if(this->m_line == rhs.m_line) {
            return this->m_character < rhs.m_character;
        } else {
            return this->m_line < rhs.m_line;
        }
    }
    bool operator>(const Position& rhs) const { return rhs < *this; }
    bool operator<=(const Position& rhs) const { return !(*this > rhs); }
    bool operator>=(const Position& rhs) const { return !(*this < rhs); }
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
    bool IsOk() const { return m_line != wxNOT_FOUND && m_character != wxNOT_FOUND; }
};

//===----------------------------------------------------------------------------------
// Range
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Range : public Serializable
{
    Position m_start;
    Position m_end;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    Range(const Position& start, const Position& end)
        : m_start(start)
        , m_end(end)
    {
    }
    Range() {}
    virtual ~Range() {}
    Range& SetEnd(const Position& end)
    {
        this->m_end = end;
        return *this;
    }
    Range& SetStart(const Position& start)
    {
        this->m_start = start;
        return *this;
    }
    const Position& GetEnd() const { return m_end; }
    const Position& GetStart() const { return m_start; }
    bool IsOk() const { return m_start.IsOk() && m_end.IsOk(); }
};

//===----------------------------------------------------------------------------------
// TextEdit
//===----------------------------------------------------------------------------------

class WXDLLIMPEXP_CL TextEdit : public LSP::Serializable
{
    Range m_range;
    wxString m_newText;

public:
    TextEdit() {}
    virtual ~TextEdit() {}
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    void SetNewText(const wxString& newText) { this->m_newText = newText; }
    void SetRange(const Range& range) { this->m_range = range; }
    const wxString& GetNewText() const { return m_newText; }
    const Range& GetRange() const { return m_range; }
    bool IsOk() const { return m_range.IsOk(); }
};

class WXDLLIMPEXP_CL Location : public Serializable
{
    URI m_uri;
    Range m_range;
    wxString m_pattern;
    wxString m_name;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    Location(const wxString& uri, const Range& range)
        : m_range(range)
    {
        URI::FromString(uri, &m_uri);
    }

    Location() {}
    virtual ~Location() {}
    Location& SetRange(const Range& range)
    {
        this->m_range = range;
        return *this;
    }
    Location& SetPath(const wxString& path)
    {
        URI::FromString(path, &m_uri);
        return *this;
    }

    const Range& GetRange() const { return m_range; }
    const wxString& GetPathAsURI() const { return m_uri.GetUrl(); }
    const wxString& GetPath() const { return m_uri.GetPath(); }
    void SetPattern(const wxString& pattern) { this->m_pattern = pattern; }
    const wxString& GetPattern() const { return m_pattern; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
};

//===----------------------------------------------------------------------------------
// TextDocumentItem
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL TextDocumentItem : public Serializable
{
    URI m_uri;
    wxString m_languageId;
    wxString m_text;
    int m_version = 1;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    TextDocumentItem(const wxString& uri, const wxString& langId, const wxString& text, int version = 1)
        : m_languageId(langId)
        , m_text(text)
        , m_version(version)
    {
        URI::FromString(uri, &m_uri);
    }

    TextDocumentItem() {}
    virtual ~TextDocumentItem() {}
    TextDocumentItem& SetLanguageId(const wxString& languageId)
    {
        this->m_languageId = languageId;
        return *this;
    }
    TextDocumentItem& SetText(const std::string& text)
    {
        this->m_text = text;
        return *this;
    }
    TextDocumentItem& SetUri(const wxString& uri)
    {
        URI::FromString(uri, &m_uri);
        return *this;
    }

    TextDocumentItem& SetVersion(int version)
    {
        this->m_version = version;
        return *this;
    }
    const wxString& GetLanguageId() const { return m_languageId; }
    const wxString& GetText() const { return m_text; }
    const wxString& GetPathAsURI() const { return m_uri.GetUrl(); }
    const wxString& GetPath() const { return m_uri.GetPath(); }
    int GetVersion() const { return m_version; }
};

class WXDLLIMPEXP_CL ParameterInformation : public LSP::Serializable
{
    wxString m_label;
    wxString m_documentation;

public:
    ParameterInformation() {}
    virtual ~ParameterInformation() {}
    ParameterInformation& SetDocumentation(const wxString& documentation)
    {
        this->m_documentation = documentation;
        return *this;
    }
    ParameterInformation& SetLabel(const wxString& label)
    {
        this->m_label = label;
        return *this;
    }
    const wxString& GetDocumentation() const { return m_documentation; }
    const wxString& GetLabel() const { return m_label; }
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    typedef std::vector<ParameterInformation> Vec_t;
};

class WXDLLIMPEXP_CL SignatureInformation : public LSP::Serializable
{
    wxString m_label;
    wxString m_documentation;
    ParameterInformation::Vec_t m_parameters;

public:
    typedef std::vector<LSP::SignatureInformation> Vec_t;

public:
    SignatureInformation() {}
    virtual ~SignatureInformation() {}
    SignatureInformation& SetParameters(const ParameterInformation::Vec_t& parameters)
    {
        this->m_parameters = parameters;
        return *this;
    }
    const ParameterInformation::Vec_t& GetParameters() const { return m_parameters; }
    SignatureInformation& SetDocumentation(const wxString& documentation)
    {
        this->m_documentation = documentation;
        return *this;
    }
    SignatureInformation& SetLabel(const wxString& label)
    {
        this->m_label = label;
        return *this;
    }
    const wxString& GetDocumentation() const { return m_documentation; }
    const wxString& GetLabel() const { return m_label; }
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
};

class WXDLLIMPEXP_CL SignatureHelp : public LSP::Serializable
{
    SignatureInformation::Vec_t m_signatures;
    int m_activeSignature = 0;
    int m_activeParameter = 0;

public:
    SignatureHelp() {}
    virtual ~SignatureHelp() {}

    SignatureHelp& SetActiveParameter(int activeParameter)
    {
        this->m_activeParameter = activeParameter;
        return *this;
    }
    SignatureHelp& SetActiveSignature(int activeSignature)
    {
        this->m_activeSignature = activeSignature;
        return *this;
    }
    SignatureHelp& SetSignatures(const SignatureInformation::Vec_t& signatures)
    {
        this->m_signatures = signatures;
        return *this;
    }
    int GetActiveParameter() const { return m_activeParameter; }
    int GetActiveSignature() const { return m_activeSignature; }
    const SignatureInformation::Vec_t& GetSignatures() const { return m_signatures; }
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
};

class WXDLLIMPEXP_CL MarkupContent : public LSP::Serializable
{
    wxString m_kind;
    wxString m_value;

public:
    MarkupContent() {}
    virtual ~MarkupContent() {}
    MarkupContent& SetKind(const wxString& kind)
    {
        this->m_kind = kind;
        return *this;
    }
    const wxString& GetKind() const { return m_kind; }
    MarkupContent& SetValue(const wxString& value)
    {
        this->m_value = value;
        return *this;
    }
    const wxString& GetValue() const { return m_value; }
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
};

class WXDLLIMPEXP_CL Hover : public LSP::Serializable
{
    MarkupContent m_contents;
    Range m_range;

public:
    Hover() {}
    virtual ~Hover() {}
    Hover& SetContents(const MarkupContent& contents)
    {
        this->m_contents = contents;
        return *this;
    }
    const MarkupContent& GetContents() const { return m_contents; }
    Hover& SetRange(const Range& range)
    {
        this->m_range = range;
        return *this;
    }
    const Range& GetRange() const { return m_range; }
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
};

class WXDLLIMPEXP_CL Diagnostic : public Serializable
{
    Range m_range;
    wxString m_message;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    Diagnostic(const Range& range, const wxString& message)
        : m_range(range)
        , m_message(message)
    {
    }
    Diagnostic() {}
    virtual ~Diagnostic() {}
    Diagnostic& SetRange(const Range& range)
    {
        this->m_range = range;
        return *this;
    }
    const Range& GetRange() const { return m_range; }
    Diagnostic& SetMessage(const wxString& message)
    {
        this->m_message = message;
        return *this;
    }
    const wxString& GetMessage() const { return m_message; }
};

class WXDLLIMPEXP_CL DocumentSymbol : public Serializable
{
    /**
     * The name of this symbol. Will be displayed in the user interface and therefore must not be
     * an empty string or a string only consisting of white spaces.
     */
    wxString name;

    /**
     * More detail for this symbol, e.g the signature of a function.
     */
    wxString detail;

    /**
     * The kind of this symbol.
     */
    eSymbolKind kind;

    /**
     * The range enclosing this symbol not including leading/trailing whitespace but everything else
     * like comments. This information is typically used to determine if the clients cursor is
     * inside the symbol to reveal in the symbol in the UI.
     */
    Range range;

    /**
     * The range that should be selected and revealed when this symbol is being picked, e.g the name of a function.
     * Must be contained by the `range`.
     */
    Range selectionRange;

    /**
     * Children of this symbol, e.g. properties of a class.
     */
    std::vector<DocumentSymbol> children;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    DocumentSymbol() {}
    virtual ~DocumentSymbol() {}

    void SetChildren(const std::vector<DocumentSymbol>& children) { this->children = children; }
    void SetDetail(const wxString& detail) { this->detail = detail; }
    void SetKind(const eSymbolKind& kind) { this->kind = kind; }
    void SetName(const wxString& name) { this->name = name; }
    void SetRange(const Range& range) { this->range = range; }
    void SetSelectionRange(const Range& selectionRange) { this->selectionRange = selectionRange; }
    const std::vector<DocumentSymbol>& GetChildren() const { return children; }
    const wxString& GetDetail() const { return detail; }
    const eSymbolKind& GetKind() const { return kind; }
    const wxString& GetName() const { return name; }
    const Range& GetRange() const { return range; }
    const Range& GetSelectionRange() const { return selectionRange; }
};

class WXDLLIMPEXP_CL SymbolInformation : public Serializable
{
    /**
     * The name of this symbol.
     */
    wxString name;

    /**
     * The kind of this symbol.
     */
    eSymbolKind kind;

    /**
     * The location of this symbol. The location's range is used by a tool
     * to reveal the location in the editor. If the symbol is selected in the
     * tool the range's start information is used to position the cursor. So
     * the range usually spans more then the actual symbol's name and does
     * normally include things like visibility modifiers.
     *
     * The range doesn't have to denote a node range in the sense of a abstract
     * syntax tree. It can therefore not be used to re-construct a hierarchy of
     * the symbols.
     */
    Location location;

    /**
     * The name of the symbol containing this symbol. This information is for
     * user interface purposes (e.g. to render a qualifier in the user interface
     * if necessary). It can't be used to re-infer a hierarchy for the document
     * symbols.
     */
    wxString containerName;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    SymbolInformation() {}
    virtual ~SymbolInformation() {}

    void SetContainerName(const wxString& containerName) { this->containerName = containerName; }
    void SetKind(const eSymbolKind& kind) { this->kind = kind; }
    void SetLocation(const Location& location) { this->location = location; }
    void SetName(const wxString& name) { this->name = name; }
    const wxString& GetContainerName() const { return containerName; }
    const eSymbolKind& GetKind() const { return kind; }
    const Location& GetLocation() const { return location; }
    const wxString& GetName() const { return name; }
};

};     // namespace LSP
#endif // JSONRPC_BASICTYPES_H
