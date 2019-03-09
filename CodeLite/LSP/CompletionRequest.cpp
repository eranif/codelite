#include "CompletionRequest.h"
#include "LSP/CompletionItem.h"
#include "LSP/LSPEvent.h"
#include "file_logger.h"

LSP::CompletionRequest::CompletionRequest(const LSP::TextDocumentIdentifier& textDocument,
                                          const LSP::Position& position)
{
    SetMethod("textDocument/completion");
    m_params.reset(new CompletionParams());
    m_params->As<CompletionParams>()->SetPosition(position);
    m_params->As<CompletionParams>()->SetTextDocument(textDocument);
    SetNeedsReply(true);
}

LSP::CompletionRequest::~CompletionRequest() {}

void LSP::CompletionRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    JSONItem result = response.Get("result");
    if(!result.isOk()) { return; }

    // We now accept the 'items' array
    JSONItem items = result.namedObject("items");
    if(!items.isOk() || !items.isArray()) { return; }

    CompletionItem::Vec_t completions;
    const int itemsCount = items.arraySize();
    for(int i = 0; i < itemsCount; ++i) {
        CompletionItem::Ptr_t completionItem(new CompletionItem());
        completionItem->FromJSON(items.arrayItem(i));
        completions.push_back(completionItem);
    }
    
    clDEBUG() << "Received:" << completions.size() << "completion items";
    if(!completions.empty()) {
        LSPEvent event(wxEVT_LSP_COMPLETION_READY);
        event.SetCompletions(completions);
        owner->AddPendingEvent(event);
    }
}

void LSP::CompletionRequest::BuildUID()
{
    if(!m_uuid.IsEmpty()) { return; }
    m_uuid << GetMethod() << ":" << m_params->As<CompletionParams>()->GetTextDocument().GetFilename().GetFullPath();
}
