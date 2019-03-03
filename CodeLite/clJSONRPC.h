#ifndef CLJSONRPC_H
#define CLJSONRPC_H

#include "codelite_exports.h"
#include "json_node.h"
#include "cl_command_event.h"

class clJSONRPCRequest;

class WXDLLIMPEXP_CL clJSONRPCResponseEvent : public clCommandEvent
{
    bool m_success = true;
    int m_requestId = wxNOT_FOUND;
    wxString m_result;
    wxString m_errorMessage;

public:
    clJSONRPCResponseEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clJSONRPCResponseEvent(const clJSONRPCResponseEvent& event);
    clJSONRPCResponseEvent& operator=(const clJSONRPCResponseEvent& src);
    virtual ~clJSONRPCResponseEvent();
    virtual wxEvent* Clone() const { return new clJSONRPCResponseEvent(*this); }

    void SetErrorMessage(const wxString& errorMessage) { this->m_errorMessage = errorMessage; }
    void SetRequestId(int requestId) { this->m_requestId = requestId; }
    void SetResult(const wxString& result) { this->m_result = result; }
    void SetSuccess(bool success) { this->m_success = success; }
    const wxString& GetErrorMessage() const { return m_errorMessage; }
    int GetRequestId() const { return m_requestId; }
    const wxString& GetResult() const { return m_result; }
    bool IsSuccess() const { return m_success; }
};

typedef void (wxEvtHandler::*clJSONRPCResponseEventFunction)(clJSONRPCResponseEvent&);
#define clJSONRPCResponseEventHandler(func) wxEVENT_HANDLER_CAST(clJSONRPCResponseEventFunction, func)

//===-----------------------------------------------------------------------------------
// Base type class
//===-----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL clJSONRPC_Type
{
public:
    virtual JSONElement ToJSON(const wxString& name) const = 0;
    virtual void FromJSON(const JSONElement& json) = 0;
};

//===----------------------------------------------------------------------------------
// Params
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL clJSONRPC_Params
{
    JSONElement m_params;

public:
    clJSONRPC_Params();
    virtual ~clJSONRPC_Params() {}
    clJSONRPC_Params& AddParam(const wxString& name, const clJSONRPC_Type& param);
    bool IsOk() const { return m_params.isOk(); }
    JSONElement GetJSON() const { return m_params; }
};

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL clJSONRPC_TextDocumentIdentifier : public clJSONRPC_Type
{
    wxFileName m_filename;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON(const wxString& name) const;

    clJSONRPC_TextDocumentIdentifier() {}
    virtual ~clJSONRPC_TextDocumentIdentifier() {}
    clJSONRPC_TextDocumentIdentifier& SetFilename(const wxFileName& filename)
    {
        this->m_filename = filename;
        return *this;
    }
    const wxFileName& GetFilename() const { return m_filename; }
};

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL clJSONRPC_Position : public clJSONRPC_Type
{
    int m_line;
    int m_character;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON(const wxString& name) const;

    clJSONRPC_Position() {}
    virtual ~clJSONRPC_Position() {}
    clJSONRPC_Position& SetCharacter(int character)
    {
        this->m_character = character;
        return *this;
    }
    clJSONRPC_Position& SetLine(int line)
    {
        this->m_line = line;
        return *this;
    }
    int GetCharacter() const { return m_character; }
    int GetLine() const { return m_line; }
};

//=========================================================================================
//=========================================================================================
//=========================================================================================

class WXDLLIMPEXP_CL clJSONRPCSender : public wxEvtHandler
{
public:
    /**
     * @brief send a JSONRPC request object (represented as a string) over the network
     * The response is received as an event of type clJSONRPCResponseEvent
     */
    virtual void Send(const wxString& message) = 0;
};

class WXDLLIMPEXP_CL clJSONRPCRequest
{
protected:
    wxString m_method;
    int m_id = wxNOT_FOUND;
    clJSONRPC_Params m_params;

protected:
    /**
     * @brief format this object into a string
     */
    wxString ToString() const;

public:
    clJSONRPCRequest();
    virtual ~clJSONRPCRequest();
    void Send(clJSONRPCSender* sender) const;

    // Accessors
    void SetParams(const clJSONRPC_Params& params);
    int GetId() const { return m_id; }
    void SetMethod(const wxString& method) { this->m_method = method; }
    const wxString& GetMethod() const { return m_method; }
};

#endif // CLJSONRPC_H
