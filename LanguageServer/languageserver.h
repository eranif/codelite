#ifndef __LanguageServerPlugin__
#define __LanguageServerPlugin__

#include "LanguageServerCluster.h"
#include "LanguageServerLogView.h"
#include "clTabTogglerHelper.h"
#include "cl_command_event.h"
#include "detectors/LSPDetector.hpp"
#include "plugin.h"

class LanguageServerPlugin : public IPlugin
{
    std::unique_ptr<LanguageServerCluster> m_servers;
    clTabTogglerHelper::Ptr_t m_tabToggler;
    LanguageServerLogView* m_logView = nullptr;

protected:
    void OnSettings(wxCommandEvent& e);
    void OnRestartLSP(wxCommandEvent& e);
    void OnInitDone(wxCommandEvent& event);
    void OnEditorContextMenu(clContextMenuEvent& event);
    void OnMenuFindSymbol(wxCommandEvent& event);
    void OnMenuRenameSymbol(wxCommandEvent& event);
    void OnMenuFindReferences(wxCommandEvent& event);
    void ConfigureLSPs(const std::vector<LSPDetector::Ptr_t>& lsps);

    void OnLSPStopAll(clLanguageServerEvent& event);
    void OnLSPStartAll(clLanguageServerEvent& event);
    void OnLSPRestartAll(clLanguageServerEvent& event);
    void OnLSPStopOne(clLanguageServerEvent& event);
    void OnLSPStartOne(clLanguageServerEvent& event);
    void OnLSPRestartOne(clLanguageServerEvent& event);
    void OnLSPConfigure(clLanguageServerEvent& event);
    void OnLSPDelete(clLanguageServerEvent& event);
    void OnLSPShowSettingsDlg(clLanguageServerEvent& event);
    void OnLSPEnableServer(clLanguageServerEvent& event);
    void OnLSPDisableServer(clLanguageServerEvent& event);
    wxString GetEditorFilePath(IEditor* editor) const;
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnFixLSPPaths(wxCommandEvent& event);
    // Called by the plugin after load to check that all the configured
    // LSPs are valid
    void CheckServers();
    wxArrayString GetBrokenLSPs() const;

public:
    LanguageServerPlugin(IManager* manager);
    ~LanguageServerPlugin() override = default;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    void CreatePluginMenu(wxMenu* pluginsMenu) override;

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    void UnPlug() override;

    /**
     * @brief log message to the output tab
     */
    void LogMessage(const wxString& server_name, const wxString& message, int log_leve);
};

#endif // LanguageServerPlugin
