#include "CompletionRequest.h"

#include "LSP/CompletionItem.h"
#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"

LSP::CompletionRequest::CompletionRequest(const LSP::TextDocumentIdentifier& textDocument,
                                          const LSP::Position& position, bool userTriggered)
{
    SetMethod("textDocument/completion");
    m_userTrigger = userTriggered;
    m_params.reset(new CompletionParams());
    m_params->As<CompletionParams>()->SetPosition(position);
    m_params->As<CompletionParams>()->SetTextDocument(textDocument);
}

LSP::CompletionRequest::~CompletionRequest() {}

void LSP::CompletionRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    JSONItem result = response.Get("result");
    if(!result.isOk()) {
        LSP_WARNING() << "LSP::CompletionRequest::OnResponse(): invalid 'result' object";
        return;
    }

    // We now accept the 'items' array
    JSONItem items = result.namedObject("items");
    if(!items.isOk()) {
        LSP_WARNING() << "LSP::CompletionRequest::OnResponse(): invalid 'items' object";
        // LSP_WARNING() << result.format() << clEndl;
        // return;
    }

    JSONItem* pItems = items.isOk() ? &items : &result;
    if(!pItems->isArray()) {
        LSP_WARNING() << "LSP::CompletionRequest::OnResponse(): items is not of type array";
        return;
    }

    CompletionItem::Vec_t completions;
    const int itemsCount = pItems->arraySize();
    LSP_DEBUG() << "Read" << itemsCount << "completion items";
    for(int i = 0; i < itemsCount; ++i) {
        CompletionItem::Ptr_t completionItem(new CompletionItem());
        completionItem->FromJSON(pItems->arrayItem(i));
        if(completionItem->GetInsertText().IsEmpty()) {
            completionItem->SetInsertText(completionItem->GetLabel());
        }
        completions.push_back(completionItem);
    }

    LSP_DEBUG() << "Received:" << completions.size() << "completion items";
    if(!completions.empty()) {
        LSPEvent event(wxEVT_LSP_COMPLETION_READY);
        event.SetCompletions(completions);
        event.SetTriggerKind(m_userTrigger ? CompletionItem::kTriggerUser : CompletionItem::kTriggerKindInvoked);
        owner->QueueEvent(event.Clone());
    }
}

bool LSP::CompletionRequest::IsValidAt(const wxString& filename, size_t line, size_t col) const
{
    wxString path = m_params->As<CompletionParams>()->GetTextDocument().GetPath();
    size_t calledLine = m_params->As<CompletionParams>()->GetPosition().GetLine();
    size_t calledColumn = m_params->As<CompletionParams>()->GetPosition().GetCharacter();
    return (path == filename) && (calledLine == line) && (calledColumn == col);
}
