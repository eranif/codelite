#include "CodeActionRequest.hpp"

#include "LSP/LSPEvent.h"
#include "event_notifier.h"

#include <algorithm>

LSP::CodeActionRequest::CodeActionRequest(const LSP::TextDocumentIdentifier& textDocument,
                                          const LSP::Range& range,
                                          const std::vector<LSP::Diagnostic>& diags,
                                          const wxArrayString& only)
{
    SetMethod("textDocument/codeAction");
    m_params.reset(new CodeActionParams());
    m_params->As<CodeActionParams>()->SetTextDocument(textDocument);
    m_params->As<CodeActionParams>()->SetRange(range);
    m_params->As<CodeActionParams>()->SetDiagnostics(diags);
    m_params->As<CodeActionParams>()->SetOnly(only);
    LSP_DEBUG() << wxString::FromUTF8(ToJSON().dump(2)) << endl;
}

namespace
{
/// `only` is hierarchical: "quickfix" matches "quickfix" and "quickfix.foo". Actions without a kind (for example
/// plain commands) are kept, because we can not tell what they do
bool IsKindRequested(const wxString& kind, const wxArrayString& only)
{
    if (only.empty() || kind.empty()) {
        return true;
    }
    return std::ranges::any_of(
        only, [&](const wxString& requested) { return kind == requested || kind.StartsWith(requested + "."); });
}
} // namespace

std::optional<LSPEvent> LSP::CodeActionRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    LSP_DEBUG() << "LSP::CodeActionRequest::OnResponse()" << endl;
    LSP_DEBUG() << response.ToString() << endl;
    auto result_arr = response.Get("result");
    if (!result_arr.isArray()) {
        LSP_WARNING() << "CodeAction result is expected to be of type array" << endl;
        return std::nullopt;
    }

    // expected array of `CodeAction` or `Command`
    size_t count = result_arr.arraySize();

    LSPEvent event{wxEVT_LSP_CODE_ACTIONS};
    auto& actions = event.GetCodeActions();
    actions.reserve(count);

    // Some servers ignore `context.only`, so filter here as well
    const auto& only = m_params->As<CodeActionParams>()->GetOnly();
    for (size_t i = 0; i < count; ++i) {
        LSP::CodeAction action;
        action.FromJSON(result_arr[i]);
        if (!IsKindRequested(action.GetKind(), only)) {
            LSP_DEBUG() << "Skipping code action of kind" << action.GetKind() << ":" << action.GetTitle() << endl;
            continue;
        }
        actions.push_back(action);
    }

    LSP_DEBUG() << "Read" << actions.size() << "code actions" << endl;
    event.SetFileName(m_params->As<CodeActionParams>()->GetTextDocument().GetPath());
    EventNotifier::Get()->AddPendingEvent(event);
    return event;
}
