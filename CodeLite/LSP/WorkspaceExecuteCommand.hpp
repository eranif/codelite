#ifndef WORKSPACEEXECUTECOMMAND_HPP
#define WORKSPACEEXECUTECOMMAND_HPP

#include "LSP/Request.h"

namespace LSP
{
class WXDLLIMPEXP_CL WorkspaceExecuteCommand : public LSP::Request
{
    wxString m_filepath;

public:
    WorkspaceExecuteCommand(const wxString& filepath, const LSP::Command& command);
    virtual ~WorkspaceExecuteCommand();

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};
} // namespace LSP

#endif // WORKSPACEEXECUTECOMMAND_HPP
