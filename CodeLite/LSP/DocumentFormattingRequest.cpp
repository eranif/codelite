#include "DocumentFormattingRequest.hpp"

#include "LSP/LSPEvent.h"
#include "LSP/ResponseError.h"
#include "event_notifier.h"

LSP::DocumentFormattingRequest::DocumentFormattingRequest(const wxString& filepath, size_t tabSize, bool insertSpaces)
    : m_filepath(filepath)
{
    SetMethod("textDocument/formatting");
    m_params.reset(new DocumentFormattingParams());
    m_params->As<DocumentFormattingParams>()->SetTextDocument(TextDocumentIdentifier(filepath));
    m_params->As<DocumentFormattingParams>()->SetTabSize(tabSize);
    m_params->As<DocumentFormattingParams>()->SetInsertSpaces(insertSpaces);
}

std::optional<LSPEvent> LSP::DocumentFormattingRequest::OnResponse(const LSP::ResponseMessage& response,
                                                                   [[maybe_unused]] wxEvtHandler* owner)
{
    LSP_DEBUG() << "LSP::DocumentFormattingRequest::OnResponse()" << endl;

    // the result is `TextEdit[]`, or `null` when there is nothing to change
    LSP::WorkspaceEditChange change;
    change.path = m_filepath;
    auto result = response.Get("result");
    if (result.isArray()) {
        int count = result.arraySize();
        change.edits.reserve(count);
        for (int i = 0; i < count; ++i) {
            LSP::TextEdit edit;
            edit.FromJSON(result[i]);
            if (edit.IsOk()) {
                change.edits.push_back(edit);
            }
        }
    }

    LSPEvent event{wxEVT_LSP_DOCUMENT_FORMATTED};
    event.SetFileName(m_filepath);
    event.SetChanges({change});
    EventNotifier::Get()->AddPendingEvent(event);
    return event;
}

void LSP::DocumentFormattingRequest::HandleError(const LSP::ResponseMessage& response,
                                                 [[maybe_unused]] wxEvtHandler* owner)
{
    LSP::ResponseError errMsg(response.ToString());
    LSP_WARNING() << "textDocument/formatting failed for" << m_filepath << ":" << errMsg.GetMessage() << endl;

    // tell the formatter, so it stops waiting. The message is the error
    LSPEvent event{wxEVT_LSP_DOCUMENT_FORMATTED};
    event.SetFileName(m_filepath);
    event.SetMessage(errMsg.GetMessage());
    EventNotifier::Get()->AddPendingEvent(event);
}
