#ifndef __wxcrafter_plugin__
#define __wxcrafter_plugin__

#include "UI/NewFormWizard.h"
#include "UI/import_dlg.h"
#include "UI/mainFrame.h"
#include "cl_command_event.h" // clDebugEvent
#include "plugin.h"

class EventsEditorPane;
class GUICraftMainPanel;
class wxcTreeView;

class wxCrafterPlugin : public IPlugin
{
    MainFrame* m_mainFrame = nullptr;
    wxFileName m_selectedFile;

protected:
    wxMenu* DoCreateFolderMenu();
    wxMenu* DoProjectMenu();
    void DoShowDesigner();
    void DoInitDone();
    void DoLoadWxcProject(const wxFileName& filename);

protected:
    // Event handlers
    void OnInitDone(wxCommandEvent& e);
    void OnShowDesigner(wxCommandEvent& e);
    void OnReGenerateForProject(wxCommandEvent& e);
    void OnNewForm(wxCommandEvent& e);
    void OnOpenFile(clCommandEvent& e);
    void OnProjectModified(wxCommandEvent& e);
    void OnProjectSynched(wxCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnSave(wxCommandEvent& e);
    void OnSaveAll(clCommandEvent& e);
    void OnSaveProject(wxCommandEvent& e);
    void OnSaveProjectUI(wxUpdateUIEvent& e);
    void OnOpenProject(wxCommandEvent& e);
    void OnOpenProjectUI(wxUpdateUIEvent& e);
    void OnCloseProject(wxCommandEvent& e);
    void OnCloseProjectUI(wxUpdateUIEvent& e);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnVirtualFolderContextMenu(clContextMenuEvent& event);
    void OnProjectContextMenu(clContextMenuEvent& event);
    void OnImportFBProject(wxCommandEvent& event);
    void OnOpenWxcpProject(wxCommandEvent& event);
    void OnImportwxSmithProject(wxCommandEvent& event);
    void OnToggleView(clCommandEvent& event);
    void OnSaveUI(clCommandEvent& event);

public:
    explicit wxCrafterPlugin(IManager* manager);
    ~wxCrafterPlugin() override;

    MainFrame* GetMainFrame() const { return m_mainFrame; }

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void UnPlug() override;

    IManager* GetManager() { return m_mgr; }
};

#endif //__wxcrafter_plugin__
