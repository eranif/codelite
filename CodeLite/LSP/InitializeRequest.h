#ifndef INITIALIZEREQUEST_H
#define INITIALIZEREQUEST_H

#include "LSP/RequestMessage.h"
namespace LSP
{

class WXDLLIMPEXP_CL InitializeRequest : public LSP::RequestMessage
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
    JSONItem ToJSON(const wxString& name) const;
    void BuildUID();
};

}; // namespace LSP

#endif // INITIALIZEREQUEST_H
