#ifndef INITIALIZEREQUEST_H
#define INITIALIZEREQUEST_H

#include "LSP/Request.h"
namespace LSP
{

class WXDLLIMPEXP_CL InitializeRequest : public LSP::Request
{
    int m_processId = wxNOT_FOUND;
    wxString m_rootUri;
    wxString m_initOptions;
    bool m_withTokenTypes = false;

public:
    InitializeRequest(bool withTokenTypes, const wxString& rootUri = "");
    ~InitializeRequest() override = default;
    InitializeRequest& SetProcessId(int processId)
    {
        this->m_processId = processId;
        return *this;
    }
    InitializeRequest& SetRootUri(const wxString& rootUri)
    {
        this->m_rootUri = rootUri;
        return *this;
    }
    int GetProcessId() const { return m_processId; }
    const wxString& GetRootUri() const { return m_rootUri; }
    JSONItem ToJSON(const wxString& name) const override;
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
    bool IsPositionDependentRequest() const override { return false; }
    void SetInitOptions(const wxString& initOptions) { this->m_initOptions = initOptions; }
    const wxString& GetInitOptions() const { return m_initOptions; }
};
} // namespace LSP
#endif // INITIALIZEREQUEST_H
