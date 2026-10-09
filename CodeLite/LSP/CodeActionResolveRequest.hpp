#ifndef CODEACTIONRESOLVEREQUEST_HPP
#define CODEACTIONRESOLVEREQUEST_HPP

#include "LSP/Request.h"

namespace LSP
{

/// Ask the server to fill in the missing `edit` of a code action
class WXDLLIMPEXP_CL CodeActionResolveRequest : public Request
{
public:
    CodeActionResolveRequest(const wxString& languageServerName,
                             const wxString& filepath,
                             const LSP::CodeAction& codeAction);
    ~CodeActionResolveRequest() override = default;

    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
    void HandleError(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;

private:
    wxString m_languageServerName;
    wxString m_filepath;
};

} // namespace LSP

#endif // CODEACTIONRESOLVEREQUEST_HPP
