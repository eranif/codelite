#ifndef RESPONSEERROR_H
#define RESPONSEERROR_H

#include "codelite_exports.h"
#include "JSON.h"
#include "LSP/Message.h"

namespace LSP
{

class WXDLLIMPEXP_CL ResponseError : public Message
{
    int m_errorCode = wxNOT_FOUND;
    wxString m_message;
    IPathConverter::Ptr_t m_pathConverter;
    
public:
    enum eErrorCodes {
        kErrorCodeParseError = -32700,
        kErrorCodeInvalidRequest = -32600,
        kErrorCodeMethodNotFound = -32601,
        kErrorCodeInvalidParams = -32602,
        kErrorCodeInternalError = -32603,
        kErrorCodeserverErrorStart = -32099,
        kErrorCodeserverErrorEnd = -32000,
        kErrorCodeServerNotInitialized = -32002,
        kErrorCodeUnknownErrorCode = -32001,
        kErrorCodeRequestCancelled = -32800,
        kErrorCodeContentModified = -32801,
    };

public:
    ResponseError(const wxString& message, IPathConverter::Ptr_t pathConverter);
    ResponseError();
    virtual ~ResponseError();
    void FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter);
    JSONItem ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const;
    virtual std::string ToString(IPathConverter::Ptr_t pathConverter) const;
    void SetErrorCode(int errorCode) { this->m_errorCode = errorCode; }
    void SetMessage(const wxString& message) { this->m_message = message; }
    int GetErrorCode() const { return m_errorCode; }
    const wxString& GetMessage() const { return m_message; }
    bool IsOk() const { return m_errorCode != wxNOT_FOUND; }
};

}; // namespace LSP

#endif // RESPONSEERROR_H
