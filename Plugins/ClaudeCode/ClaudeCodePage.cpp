#include "ClaudeCodePage.hpp"

#include "ColoursAndFontsManager.h"
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

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &ClaudeCodePage::OnThemeChanged, this);
}

ClaudeCodePage::~ClaudeCodePage()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &ClaudeCodePage::OnThemeChanged, this);
}

void ClaudeCodePage::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_terminal);

    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_COND_RET(lexer);
    auto font = lexer->GetFontForStyle(0, this);
    auto theme = m_terminal->GetTheme();
    theme.font = font;
    m_terminal->SetTheme(theme);
}
