#include "AgentHostPage.hpp"

#include "ColoursAndFontsManager.h"
#include "FileManager.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "ai/LLMManager.hpp"
#include "clStrings.h"
#include "globals.h"
#include "open_resource_dialog.h"
#include "search_thread.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

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
#ifndef __WXMSW__
    // Clicking on the tab label with the mouse does not move the focus to the page (GTK keeps it on the tab),
    // so we set the focus on the terminal when our page becomes the selected one.
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
#endif

    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    if (auto frame = EventNotifier::Get()->TopFrame()) {
        frame->Bind(wxEVT_MENU, &AgentHostPage::OnGrepWorkspace, this, XRCID("grep_current_workspace"));
        frame->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnGrepWorkspaceUI, this, XRCID("grep_current_workspace"));
    }
}

AgentHostPage::~AgentHostPage()
{
#ifndef __WXMSW__
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
#endif
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
    if (auto frame = EventNotifier::Get()->TopFrame()) {
        frame->Unbind(wxEVT_MENU, &AgentHostPage::OnGrepWorkspace, this, XRCID("grep_current_workspace"));
        frame->Unbind(wxEVT_UPDATE_UI, &AgentHostPage::OnGrepWorkspaceUI, this, XRCID("grep_current_workspace"));
    }
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
    OpenText(event.GetString());
}

void AgentHostPage::OpenText(const wxString& text)
{
    wxString trimmed_text = text;
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

    if (auto editor = clGetManager()->OpenFile(trimmed_text); editor != nullptr) {
        editor->SetActive();
        return;
    }

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
    m_agentInfo = info;
    RestartAgentHost();
}

std::optional<wxString> AgentHostPage::BuildSystemPrompt(const AgentInfo& info)
{
    wxBusyCursor bc{};
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr)
        return std::nullopt;

    wxString systemPromptFilePath;
    wxString systemPromptArgs;
    wxString systemPrompt = llm::Manager::GetInstance().GetConfig().GetSystemPrompt();
    if (systemPrompt.empty())
        return std::nullopt;

    switch (info.agent_type) {
    case AgentType::kClaudeCode: {
        systemPromptFilePath = FileManager::GetSettingFileFullPath("SYSTEM_PROMPT.md");
        if (!FileManager::WriteContent(systemPromptFilePath, systemPrompt, true)) {
            clWARNING() << "Failed to write system prompt file:" << systemPromptFilePath << endl;
            return std::nullopt;
        }
        systemPromptArgs << "--append-system-prompt-file " << StringUtils::WrapWithDoubleQuotes(systemPromptFilePath);
    } break;
    case AgentType::kKiroCli: {
        // Kiro does not support using custom files, we need to use .kiro/steering/SYSTEM_PROMPT.md file.
        systemPromptFilePath = FileManager::GetFullPath(".kiro/steering/SYSTEM_PROMPT.md");
        wxString dirpath = systemPromptFilePath.BeforeLast('/');
        clDEBUG() << "Creating rmeote dir:" << dirpath << endl;
        FileManager::CreateDir(dirpath);
        systemPromptArgs.clear();
    } break;
    default:
        return std::nullopt;
    }

    if (!FileManager::WriteContent(systemPromptFilePath, systemPrompt, true)) {
        clWARNING() << "Failed to write system prompt file:" << systemPromptFilePath << endl;
        return std::nullopt;
    }
    clDEBUG() << "Successfully written system prompt file:" << systemPromptFilePath << endl;
    clDEBUG() << "Will add the following args to agent CLI:" << systemPromptArgs << endl;
    return systemPromptArgs;
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

void AgentHostPage::OnContextMenu(wxContextMenuEvent& event)
{
    wxUnusedVar(event);
    wxMenu menu;

    const wxString selection = GetSelectedLine();
    if (m_terminal->CanCopy()) {
        menu.Append(wxID_COPY);
        menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { m_terminal->Copy(); }, wxID_COPY);
    }

    menu.Append(wxID_PASTE);
    menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { m_terminal->Paste(); }, wxID_PASTE);
    menu.AppendSeparator();

    if (!selection.empty()) {
        // Keep long selections readable in the menu
        wxString label = selection;
        if (label.length() > 40) {
            label = label.Left(37) + "...";
        }
        // Escape '&' so it is not used as a mnemonic
        label.Replace("&", "&&");

        const int symbol_id = wxWindow::NewControlId();
        const int search_id = XRCID("grep_current_workspace");
        wxString search_label = wxString::Format(_("Search '%s' in Workspace"), label);

        menu.Append(search_id, search_label);
        menu.Append(symbol_id, wxString::Format(_("Open Symbol '%s'"), label));
        menu.Bind(
            wxEVT_MENU,
            [this, selection](wxCommandEvent&) { CallAfter(&AgentHostPage::SearchInWorkspace, selection); },
            search_id);
        menu.Bind(
            wxEVT_MENU,
            [this, selection](wxCommandEvent&) { CallAfter(&AgentHostPage::OpenText, selection); },
            symbol_id);
        menu.AppendSeparator();
    }
    menu.Append(wxID_REFRESH);
    menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { CallAfter(&AgentHostPage::RestartAgentHost); }, wxID_REFRESH);

    clKeyboardManager::Get()->UpdateMenuShortcuts(menu);
    m_terminal->PopupMenu(&menu);
}

void AgentHostPage::RestartAgentHost()
{
    wxWindowUpdateLocker locker{this};
    if (m_terminal) {
        GetSizer()->Detach(m_terminal);
        wxDELETE(m_terminal);
    }

    // Remember the label given to the tab, the blink code needs it.
    wxString baseCommand = m_agentInfo.executable;
    baseCommand.Prepend("\"").Append("\"");
    wxString command_to_run;

    // Append the system prompt args to the base command.
    auto systemPromptArgs = BuildSystemPrompt(m_agentInfo);
    if (systemPromptArgs && !systemPromptArgs->empty())
        baseCommand << " " << systemPromptArgs.value();

    switch (m_agentInfo.agent_type) {
    case AgentType::kClaudeCode:
        command_to_run = wxString::Format("%s --continue || %s", baseCommand, baseCommand);
        break;
    case AgentType::kKiroCli:
        command_to_run = wxString::Format("%s --resume || %s", baseCommand, baseCommand);
        break;
    }
    if (!m_agentInfo.workingDirectory.empty()) {
        wxString cd_command;
        cd_command << "cd \"" << m_agentInfo.workingDirectory << "\" && ";
        command_to_run.Prepend(cd_command);
    }
    m_terminal = clGetManager()->GetTerminalManager()->OpenNewTerminalTab(
        wxEmptyString, m_agentInfo.sshAccount, wxEmptyString, true, kShellCommand, this);
    GetSizer()->Add(m_terminal, wxSizerFlags(1).Expand());
    GetSizer()->Layout();
    // Hook a custom context menu
    m_terminal->Bind(wxEVT_CONTEXT_MENU, &AgentHostPage::OnContextMenu, this);
    m_terminal->SendCommand(command_to_run);
}

wxString AgentHostPage::GetSelectedLine() const
{
    if (m_terminal == nullptr || !m_terminal->CanCopy()) {
        return wxEmptyString;
    }
    // Use the first line of the selection only
    return m_terminal->GetMouseSelection()
        .value_or(wxEmptyString)
        .BeforeFirst('\n')
        .BeforeFirst('\r')
        .Trim()
        .Trim(false);
}

void AgentHostPage::SearchInWorkspace(const wxString& text)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    auto owner = clGetManager()->BookGetPage(PaneId::BOTTOM_BAR, FIND_IN_FILES_WIN);
    if (workspace == nullptr || owner == nullptr || text.empty()) {
        return;
    }

    wxArrayString files;
    workspace->GetWorkspaceFiles(files);
    SearchThreadST::Get()->GrepWord(owner, files, text);

    if (m_terminal)
        m_terminal->CallAfter(&wxWindow::SetFocus);
}

void AgentHostPage::OnGrepWorkspace(wxCommandEvent& event)
{
    // Handle it only when the focus is in our terminal and we have a selection, otherwise let the frame handle it
    const wxString selection = (wxWindow::FindFocus() == m_terminal) ? GetSelectedLine() : wxString();
    if (selection.empty()) {
        event.Skip();
        return;
    }
    SearchInWorkspace(selection);
}

void AgentHostPage::OnGrepWorkspaceUI(wxUpdateUIEvent& event)
{
    // Enable the item when our terminal has the focus and a selection, otherwise let the frame decide
    if (wxWindow::FindFocus() == m_terminal && !GetSelectedLine().empty() &&
        clWorkspaceManager::Get().GetWorkspace() != nullptr) {
        event.Enable(true);
        return;
    }
    event.Skip();
}
