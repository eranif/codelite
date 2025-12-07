#ifndef GOTOIMPLEMENTATIONREQUEST_H
#define GOTOIMPLEMENTATIONREQUEST_H

#include "IPathConverter.hpp"
#include "LSP/Request.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL GotoImplementationRequest : public Request
{
    wxString m_filename;
    size_t m_line = 0;
    size_t m_column = 0;

public:
    explicit GotoImplementationRequest(const wxString& filename, size_t line, size_t column);
    ~GotoImplementationRequest() override = default;

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
    bool IsPositionDependentRequest() const override { return true; }
    bool IsValidAt(const wxString& filename, size_t line, size_t col) const override;
};
};     // namespace LSP
#endif // GOTOIMPLEMENTATIONREQUEST_H
