#ifndef INITIALIZEREQUEST_H
#define INITIALIZEREQUEST_H

#include "LSP/Request.h"
namespace LSP
{

class WXDLLIMPEXP_CL InitializeRequest : public LSP::Request
{
    int m_processId = wxNOT_FOUND;
    wxString m_rootUri;

public:
    InitializeRequest(const wxString& rootUri = "");
    virtual ~InitializeRequest();
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
    JSONItem ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const;
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner, IPathConverter::Ptr_t pathConverter);
    bool IsPositionDependantRequest() const { return false; }
};
};     // namespace LSP
#endif // INITIALIZEREQUEST_H
