#include "WorkspaceExecuteCommand.hpp"

#include "LSP/ResponseError.h"

#include <wx/msgdlg.h>

namespace LSP
{
WorkspaceExecuteCommand::WorkspaceExecuteCommand(const wxString& filepath, const LSP::Command& command)
    : m_filepath(filepath)
{
    SetMethod("workspace/executeCommand");
    m_params.reset(new ExecuteCommandParams(command.GetCommand(), command.GetArguments()));
    LSP_DEBUG() << wxString::FromUTF8(ToJSON().dump(2)) << endl;
}

std::optional<LSPEvent> WorkspaceExecuteCommand::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    wxUnusedVar(response);
    LSP_DEBUG() << "WorkspaceExecuteCommand::OnResponse()" << endl;
    LSP_DEBUG() << response.ToString() << endl;
    return std::nullopt;
}

void WorkspaceExecuteCommand::HandleError(const LSP::ResponseMessage& response,
                                          [[maybe_unused]] wxEvtHandler* owner) /* override */
{
    // an example for such an error:
    // {"error":{"code":-32001,
    //  "message":"Cannot rename symbol: symbol is not a supported kind (e.g. namespace, macro)"},
    //  "id":42,"jsonrpc":"2.0"}
    LSP::ResponseError errMsg(response.ToString());
    wxMessageBox(
        wxString::Format(_("Language server error:\n%s"), errMsg.GetMessage()), "CodeLite", wxICON_ERROR | wxCENTER);
}
} // namespace LSP
