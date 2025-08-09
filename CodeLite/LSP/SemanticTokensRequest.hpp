#ifndef SEMANTICTOKENSREQUEST_HPP
#define SEMANTICTOKENSREQUEST_HPP

#include "LSP/Request.h"
#include "codelite_exports.h"

#include <wx/string.h>

namespace LSP
{
class WXDLLIMPEXP_CL SemanticTokensRequest : public Request
{
    wxString m_filename;

public:
    explicit SemanticTokensRequest(const wxString& filename);
    ~SemanticTokensRequest() override = default;

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};
} // namespace LSP

#endif // SEMANTICTOKENSREQUEST_HPP
