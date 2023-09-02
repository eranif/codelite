#include "RenameRequest.hpp"

#include "LSP/LSPEvent.h"
#include "event_notifier.h"

LSP::RenameRequest::RenameRequest(const wxString& new_name, const wxString& filename, size_t line, size_t column)
{
    SetMethod("textDocument/rename");
    m_params.reset(new RenameParams());
    m_params->As<RenameParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<RenameParams>()->SetPosition(Position(line, column));
    m_params->As<RenameParams>()->SetNewName(new_name);
}

LSP::RenameRequest::~RenameRequest() {}

void LSP::RenameRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    LOG_IF_TRACE { LSP_TRACE() << "RenameRequest::OnResponse() is called" << endl; }
    wxUnusedVar(owner);
    JSONItem result = response.Get("result");
    CHECK_EXPECTED_RETURN(result.isOk(), true);
    LOG_IF_DEBUG { LSP_DEBUG() << result.format(false) << endl; }
}
