#ifndef SIGNATUREHELPREQUEST_H
#define SIGNATUREHELPREQUEST_H

#include "LSP/Request.h"
#include "LSP/ResponseMessage.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL SignatureHelpRequest : public LSP::Request
{
    wxFileName m_filename;
    size_t m_line = 0;
    size_t m_column = 0;

public:
    SignatureHelpRequest(const wxFileName& filename, size_t line, size_t column);
    virtual ~SignatureHelpRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner, IPathConverter::Ptr_t pathConverter);
    bool IsPositionDependantRequest() const { return true; }
    bool IsValidAt(const wxFileName& filename, size_t line, size_t col) const;
};
};     // namespace LSP
#endif // SIGNATUREHELPREQUEST_H
