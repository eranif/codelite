#ifndef WORKSPACEEXECUTECOMMAND_HPP
#define WORKSPACEEXECUTECOMMAND_HPP

#include "LSP/Request.h"

namespace LSP
{
class WXDLLIMPEXP_CL WorkspaceExecuteCommand : public LSP::Request
{
public:
    WorkspaceExecuteCommand(const wxString& languageServerName, const wxString& filepath, const LSP::Command& command);
    ~WorkspaceExecuteCommand() override = default;

    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
    void HandleError(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;

private:
    wxString m_languageServerName;
    wxString m_filepath;
};
} // namespace LSP

#endif // WORKSPACEEXECUTECOMMAND_HPP
