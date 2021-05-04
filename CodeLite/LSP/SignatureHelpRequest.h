#ifndef SIGNATUREHELPREQUEST_H
#define SIGNATUREHELPREQUEST_H

#include "LSP/Request.h"
#include "LSP/ResponseMessage.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL SignatureHelpRequest : public LSP::Request
{
    wxString m_filename;
    size_t m_line = 0;
    size_t m_column = 0;

public:
    explicit SignatureHelpRequest(const wxString& filename, size_t line, size_t column);
    virtual ~SignatureHelpRequest();

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
    bool IsPositionDependantRequest() const { return true; }
    bool IsValidAt(const wxString& filename, size_t line, size_t col) const;
};
};     // namespace LSP
#endif // SIGNATUREHELPREQUEST_H
