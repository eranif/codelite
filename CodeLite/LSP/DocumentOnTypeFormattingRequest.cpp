#include "DocumentOnTypeFormattingRequest.hpp"

#include "LSP/LSPEvent.h"

LSP::DocumentOnTypeFormattingRequest::DocumentOnTypeFormattingRequest(
    const wxString& filepath, const LSP::Position& position, const wxString& ch, size_t tabSize, bool insertSpaces)
    : m_filepath(filepath)
{
    SetMethod("textDocument/onTypeFormatting");
    m_params.reset(new DocumentOnTypeFormattingParams());
    auto params = m_params->As<DocumentOnTypeFormattingParams>();
    params->SetTextDocument(TextDocumentIdentifier(filepath));
    params->SetPosition(position);
    params->SetCh(ch);
    params->SetTabSize(tabSize);
    params->SetInsertSpaces(insertSpaces);
}

std::optional<LSPEvent> LSP::DocumentOnTypeFormattingRequest::OnResponse(const LSP::ResponseMessage& response,
                                                                         wxEvtHandler* owner)
{
    LSP_DEBUG() << "LSP::DocumentOnTypeFormattingRequest::OnResponse()" << endl;

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

    LSPEvent event{wxEVT_LSP_ON_TYPE_FORMATTED};
    event.SetFileName(m_filepath);
    event.SetChanges({change});
    owner->AddPendingEvent(event);
    return event;
}
