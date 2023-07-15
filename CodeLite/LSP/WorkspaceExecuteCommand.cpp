#include "WorkspaceExecuteCommand.hpp"

namespace LSP
{
WorkspaceExecuteCommand::WorkspaceExecuteCommand(const wxString& filepath, const LSP::Command& command)
    : m_filepath(filepath)
{
    SetMethod("workspace/executeCommand");
    m_params.reset(new ExecuteCommandParams(command.GetCommand(), command.GetArguments()));
    LSP_DEBUG() << ToJSON(wxEmptyString).format(true) << endl;
}

WorkspaceExecuteCommand::~WorkspaceExecuteCommand() {}

void WorkspaceExecuteCommand::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    wxUnusedVar(response);
    LSP_DEBUG() << "WorkspaceExecuteCommand::OnResponse()" << endl;
    LSP_DEBUG() << response.ToString() << endl;
}
} // namespace LSP
