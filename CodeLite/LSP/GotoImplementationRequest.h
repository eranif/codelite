#ifndef GOTOIMPLEMENTATIONREQUEST_H
#define GOTOIMPLEMENTATIONREQUEST_H

#include "LSP/Request.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL GotoImplementationRequest : public Request
{
    wxFileName m_filename;
    size_t m_line = 0;
    size_t m_column = 0;

public:
    GotoImplementationRequest(const wxFileName& filename, size_t line, size_t column);
    virtual ~GotoImplementationRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
    bool IsPositionDependantRequest() const { return true; }
    bool IsValidAt(const wxFileName& filename, size_t line, size_t col) const;
};
};     // namespace LSP
#endif // GOTOIMPLEMENTATIONREQUEST_H
