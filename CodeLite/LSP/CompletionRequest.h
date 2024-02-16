#ifndef COMPLETIONREQUEST_H
#define COMPLETIONREQUEST_H

#include "LSP/Request.h"
#include "LSP/basic_types.h"

namespace LSP
{

class WXDLLIMPEXP_CL CompletionRequest : public LSP::Request
{
public:
    CompletionRequest(const LSP::TextDocumentIdentifier& textDocument, const LSP::Position& position,
                      bool userTriggered);
    virtual ~CompletionRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
    bool IsPositionDependantRequest() const { return true; }
    bool IsValidAt(const wxString& filename, size_t line, size_t col) const;
    bool IsUserTriggeredRequest() const { return m_userTrigger; }

private:
    bool m_userTrigger = false;
};
};     // namespace LSP
#endif // COMPLETIONREQUEST_H
