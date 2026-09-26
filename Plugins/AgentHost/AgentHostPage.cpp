#include "AgentHostPage.hpp"

#include "ColoursAndFontsManager.h"
#include "Notebook.h"
#include "globals.h"
#include "open_resource_dialog.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include <wx/sizer.h>

namespace
{
#ifdef __WXMSW__
const wxString kShellCommand = "CMD";
#else
const wxString kShellCommand = "/bin/bash --login -i";
#endif
} // namespace

#define CHECK_CAN_HANDLE_EVENT(event)           \
    if (event.GetEventObject() != m_terminal) { \
        event.Skip();                           \
        return;                                 \
    }

AgentHostPage::AgentHostPage(wxBookCtrlBase* parent)
    : AgentHostPageBase(parent)
    , m_book(parent)
{
    // Clicking on the tab label with the mouse does not move the focus to the page (GTK keeps it on the tab),
    // so we set the focus on the terminal when our page becomes the selected one.
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
}

AgentHostPage::~AgentHostPage()
{
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
}

void AgentHostPage::OnThemeChanged(clCommandEvent& event)
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

void AgentHostPage::OnTerminalTitleChanged(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(m_terminal);
    wxString new_title = event.GetString();
    new_title.Trim().Trim(false);
    if (new_title.empty()) {
        new_title = _("Claude Code");
    }

    auto book = clGetManager()->GetMainNotebook();
    int where = book->FindPage(this);
    if (where != wxNOT_FOUND) {
        book->SetPageText(where, new_title);
    }
}

void AgentHostPage::OnTerminalTerminated(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(m_terminal);

    CallAfter([this]() {
        auto book = clGetManager()->GetMainNotebook();
        int where = book->FindPage(this);
        if (where != wxNOT_FOUND) {
            book->DeletePage(where);
        }
    });
}

void AgentHostPage::OnTerminalBell(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(m_terminal);
    clDEBUG() << "Got a bell!" << endl;
}

void AgentHostPage::OnTerminalLink(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(m_terminal);

    wxString trimmed_text = event.GetString();
    while (trimmed_text.EndsWith(".") || trimmed_text.EndsWith(":"))
        trimmed_text.RemoveLast();

    if (trimmed_text.StartsWith("http://") || trimmed_text.StartsWith("https://")) {
        ::wxLaunchDefaultBrowser(trimmed_text);
        return;
    }

    wxString saved_trimmed_text = trimmed_text;
    if (trimmed_text.StartsWith("~/"))
        trimmed_text = wxGetHomeDir() + trimmed_text.Mid(1);

    if (wxFileName::DirExists(trimmed_text)) {
        CallAfter([trimmed_text]() { FileUtils::OpenFileExplorer(trimmed_text); });
        return;
    }

    if (FileUtils::IsBinaryExecutable(trimmed_text)) {
        ::wxLaunchDefaultApplication(trimmed_text);
        return;
    }

    if (clGetManager()->OpenFile(trimmed_text) != nullptr)
        return;

#if USE_SFTP
    // Try a remote file.
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace && workspace->IsRemote() &&
        (clSFTPManager::Get().OpenFile(saved_trimmed_text, workspace->GetSshAccount()) != nullptr))
        return;
#endif

    // Could not resolve it, try the "open resource dialog"
    OpenResourceDialog dlg(EventNotifier::Get()->TopFrame(), clGetManager(), trimmed_text);

    if (dlg.ShowModal() == wxID_OK && !dlg.GetSelections().empty()) {
        std::vector<OpenResourceDialogItemData*> items = dlg.GetSelections();
        for (const auto item : items) {

            // try the plugins first
            clCommandEvent open_resource_event(wxEVT_OPEN_RESOURCE_FILE_SELECTED);
            open_resource_event.SetFileName(item->m_file);
            open_resource_event.SetLineNumber(item->m_line);
            open_resource_event.SetInt(item->m_column); // use the int field for the column

            if (EventNotifier::Get()->ProcessEvent(open_resource_event)) {
                continue;
            }

            // default behaviour
            OpenResourceDialog::OpenSelection(*item, clGetManager());
        }
    }
}

void AgentHostPage::StartAgentHost(const AgentInfo& info)
{
    if (m_terminal)
        return;

    // Remember the label given to the tab, the blink code needs it.
    wxString command = info.executable;
    command.Prepend("\"").Append("\"");
    wxString command_to_run;
    switch (info.agent_type) {
    case AgentType::kClaudeCode:
        command_to_run = wxString::Format("%s --continue || %s", command, command);
        break;
    case AgentType::kKiroCli:
        command_to_run = wxString::Format("%s chat --resume || %s", command, command);
        break;
    }
    if (!info.workingDirectory.empty()) {
        wxString cd_command;
        cd_command << "cd \"" << info.workingDirectory << "\" && ";
        command_to_run.Prepend(cd_command);
    }
    m_terminal = clGetManager()->GetTerminalManager()->OpenNewTerminalTab(
        wxEmptyString, info.sshAccount, wxEmptyString, true, kShellCommand, this);
    GetSizer()->Add(m_terminal, wxSizerFlags(1).Expand());
    GetSizer()->Layout();
    m_terminal->SendCommand(command_to_run);
}

void AgentHostPage::OnFocus(wxFocusEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_terminal);
    m_terminal->SetFocus();
}

void AgentHostPage::OnBookPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_terminal);
    if (m_book->GetCurrentPage() != this) {
        return;
    }

    // The focus is moved to the tab label after the page is changed, so set it once we are done handling the event
    CallAfter([this]() {
        if (m_terminal) {
            m_terminal->SetFocus();
        }
    });
}
