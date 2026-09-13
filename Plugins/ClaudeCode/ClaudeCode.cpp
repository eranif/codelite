#include "ClaudeCode.hpp"

#include "Platform/Platform.hpp"
#include "clSideBarCtrl.hpp"
#include "globals.h"

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new ClaudeCode(manager); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("ClaudeCode"));
    info.SetDescription(_("Claude Code Integration"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

ClaudeCode::ClaudeCode(IManager* manager)
    : IPlugin(manager)
{
    m_showClaudeCode =
        std::make_shared<std::function<void()>>([this]() { CallAfter(&ClaudeCode::ShowClaudeTerminal); });
    m_mgr->GetLeftSideBarCtrl()->AddActionButton(
        "claude-code", _("Launch Claude Code for the Current Workspace"), m_showClaudeCode);
    EventNotifier::Get()->Bind(wxEVT_NOTIFY_PAGE_CLOSING, &ClaudeCode::OnPageClosing, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &ClaudeCode::OnAllPagesClosed, this);
}

ClaudeCode::~ClaudeCode() { EventNotifier::Get()->Unbind(wxEVT_NOTIFY_PAGE_CLOSING, &ClaudeCode::OnPageClosing, this); }

void ClaudeCode::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }
void ClaudeCode::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }
void ClaudeCode::UnPlug() {}
void ClaudeCode::OnSettings(wxCommandEvent& event) { wxUnusedVar(event); }
void ClaudeCode::ShowClaudeTerminal()
{
    if (m_claudeTerminal) {
        if (!clGetManager()->SelectPage(m_claudeTerminal)) {
            clWARNING() << "Could not select claude tab window" << endl;
        }
        return;
    }

    // TODO: only attempt to locate claude on a local workspace.
    auto claude_exec = ThePlatform->Which("claude");
    if (!claude_exec) {
        wxMessageBox(_("Could not locate claude executable"), "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }
#ifdef __WXMSW__
    const wxString kShellCommand = "CMD";
#else
    const wxString kShellCommand = "/bin/bash --login -i";
#endif

    m_claudeTerminal = clGetManager()->GetTerminalManager()->CreateTerminal(
        clGetManager()->GetMainNotebook(), kShellCommand, "Claude Code", true, false, false, std::nullopt);

    m_claudeTerminal->Bind(wxEVT_TERMINAL_TITLE_CHANGED, [this](wxTerminalEvent& event) {
        wxString new_title = event.GetTitle();
        new_title.Trim().Trim(false);
        auto book = clGetManager()->GetMainNotebook();
        if (new_title.empty()) {
            new_title = _("Terminal");
        }
        int index = book->FindPage(m_claudeTerminal);
        if (index != wxNOT_FOUND) {
            book->SetPageText(index, new_title);
        }
    });

    m_claudeTerminal->Bind(wxEVT_TERMINAL_TERMINATED, [this](wxTerminalEvent& event) {
        m_claudeTerminal = nullptr;
        wxUnusedVar(event);
    });

    // TODO: add support for link clicked (open URLs in default browser or files inside CodeLite).
    // TODO: in case the current workspace is remote -> open claude over the network.
    // TOOD: suggest a "--continue" option to the caller.
    // TOOD: add keyboard shortcut for opening claude-code
    m_claudeTerminal->SendCommand(claude_exec.value());
}

void ClaudeCode::OnPageClosing(wxNotifyEvent& event)
{
    const wxWindow* win = reinterpret_cast<wxWindow*>(event.GetClientData());
    if (win && win == m_claudeTerminal) {
        m_claudeTerminal = nullptr;
        return;
    }
    event.Skip();
}

void ClaudeCode::OnAllPagesClosed(wxCommandEvent& event)
{
    m_claudeTerminal = nullptr;
    event.Skip();
}