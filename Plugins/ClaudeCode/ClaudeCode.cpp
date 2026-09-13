#include "ClaudeCode.hpp"

#include "clSideBarCtrl.hpp"

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
}

void ClaudeCode::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }
void ClaudeCode::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }
void ClaudeCode::UnPlug() {}
void ClaudeCode::OnSettings(wxCommandEvent& event) { wxUnusedVar(event); }
void ClaudeCode::ShowClaudeTerminal() {}
