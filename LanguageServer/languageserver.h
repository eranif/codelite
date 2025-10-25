#ifndef __LanguageServerPlugin__
#define __LanguageServerPlugin__

#include "CustomControls/TextGenerationPreviewFrame.hpp"
#include "LSP/LSPManager.hpp"
#include "LSP/LanguageServerLogView.h"
#include "LSP/detectors/LSPDetector.hpp"
#include "clTabTogglerHelper.h"
#include "cl_command_event.h"
#include "plugin.h"

class LanguageServerPlugin : public IPlugin
{
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

protected:
    void OnSettings(wxCommandEvent& e);
    void OnRestartLSP(wxCommandEvent& e);
    void OnInitDone(wxCommandEvent& event);
    void OnEditorContextMenu(clContextMenuEvent& event);

    void OnGenerateDocString(wxCommandEvent& event);
    void OnDocStringGenerationDone();
    void ConfigureLSPs(const std::vector<LSPDetector::Ptr_t>& lsps);

    void OnLSPStopOne(clLanguageServerEvent& event);
    void OnLSPStartOne(clLanguageServerEvent& event);
    void OnLSPRestartOne(clLanguageServerEvent& event);
    void OnLSPConfigure(clLanguageServerEvent& event);
    void OnLSPDelete(clLanguageServerEvent& event);
    void OnLSPShowSettingsDlg(clLanguageServerEvent& event);
    wxString GetEditorFilePath(IEditor* editor) const;
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnFixLSPPaths(wxCommandEvent& event);
    // Called by the plugin after load to check that all the configured
    // LSPs are valid
    void CheckServers();
    wxArrayString GetBrokenLSPs() const;

    clTabTogglerHelper::Ptr_t m_tabToggler;
    LanguageServerLogView* m_logView{nullptr};
    TextGenerationPreviewFrame* m_commentGenerationView{nullptr};
};

#endif // LanguageServerPlugin
