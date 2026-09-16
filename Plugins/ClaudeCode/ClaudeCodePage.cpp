#include "ClaudeCodePage.hpp"

#include "globals.h"

#include <wx/sizer.h>

namespace
{
#ifdef __WXMSW__
const wxString kShellCommand = "CMD";
#else
const wxString kShellCommand = "/bin/bash --login -i";
#endif
} // namespace

ClaudeCodePage::ClaudeCodePage(wxWindow* parent,
                               const wxString& workingDirectory,
                               const std::optional<SSHAccountInfo>& sshAccount)
    : ClaudeCodePageBase(parent)
{
    m_terminal = clGetManager()->GetTerminalManager()->OpenNewTerminalTab(
        workingDirectory, sshAccount, wxEmptyString, true, kShellCommand, this);
    GetSizer()->Add(m_terminal, wxSizerFlags(1).Expand());
    GetSizer()->Layout();
}

ClaudeCodePage::~ClaudeCodePage() {}
