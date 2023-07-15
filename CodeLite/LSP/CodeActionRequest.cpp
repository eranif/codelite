#include "CodeActionRequest.hpp"

#include "LSP/LSPEvent.h"
#include "event_notifier.h"

LSP::CodeActionRequest::CodeActionRequest(const LSP::TextDocumentIdentifier& textDocument, const LSP::Range& range,
                                          const std::vector<LSP::Diagnostic>& diags)
{
    SetMethod("textDocument/codeAction");
    m_params.reset(new CodeActionParams());
    m_params->As<CodeActionParams>()->SetTextDocument(textDocument);
    m_params->As<CodeActionParams>()->SetRange(range);
    m_params->As<CodeActionParams>()->SetDiagnostics(diags);
    LSP_DEBUG() << ToJSON(wxEmptyString).format(true) << endl;
}

LSP::CodeActionRequest::~CodeActionRequest() {}

void LSP::CodeActionRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    LSP_DEBUG() << "LSP::CodeActionRequest::OnResponse()" << endl;
    LSP_DEBUG() << response.ToString() << endl;
    auto result_arr = response.Get("result");
    if(!result_arr.isArray()) {
        LSP_WARNING() << "CodeAction result is expected to be of type array" << endl;
        return;
    }

    // expected array of commands
    size_t count = result_arr.arraySize();

    LSPEvent event{ wxEVT_LSP_CODE_ACTIONS };
    auto& commands = event.GetCommands();
    commands.reserve(count);

    for(size_t i = 0; i < count; ++i) {
        LSP::Command cmd;
        cmd.FromJSON(result_arr[i]);
        commands.push_back(cmd);
    }

    LSP_DEBUG() << "Read" << commands.size() << "code actions" << endl;
    event.SetFileName(m_params->As<CodeActionParams>()->GetTextDocument().GetPath());
    EventNotifier::Get()->AddPendingEvent(event);
}
