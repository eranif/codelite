#include "RenameRequest.hpp"

#include "LSP/LSPEvent.h"
#include "LSP/ResponseError.h"
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
    LOG_IF_TRACE { LSP_TRACE() << result.format(false) << endl; }

    std::unordered_map<wxString, std::vector<LSP::TextEdit>> modifications = ParseWorkspaceEdit(result);

    LSPEvent event_edit_files{ wxEVT_LSP_EDIT_FILES };
    event_edit_files.SetAnswer(true); // Prompt the user
    event_edit_files.SetChanges(modifications);
    owner->AddPendingEvent(event_edit_files);

    LOG_IF_DEBUG
    {
        LSP_DEBUG() << "Updating" << modifications.size() << "files:" << endl;
        for(const auto& [filepath, changes] : modifications) {
            LSP_DEBUG() << "  " << filepath << modifications.size() << "changes:" << endl;
            for(const auto& change : changes) {
                LSP_DEBUG() << "    " << change.ToJSON(wxEmptyString).format(false) << endl;
            }
        }
    }
}

void LSP::RenameRequest::OnError(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    // an example for such an error:
    // {"error":{"code":-32001,"message":"invalid name: conflict with the symbol in
    // C:/msys64/home/eran/devl/test_cpp/main.cpp:9:17"},"id":111,"jsonrpc":"2.0"}
    LSP::ResponseError errMsg(response.ToString());
    wxMessageBox(wxString::Format(_("Rename symbol error:\n%s"), errMsg.GetMessage()), "CodeLite",
                 wxICON_ERROR | wxCENTER);
}
