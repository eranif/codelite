#ifndef __wxcrafter_plugin__
#define __wxcrafter_plugin__

#include "NewFormWizard.h"
#include "Notebook.h"
#include "cl_command_event.h" // clDebugEvent
#include "import_dlg.h"
#include "main.h"
#include "plugin.h"

class EventsEditorPane;
class GUICraftMainPanel;
class wxcTreeView;

struct GeneratedClass {
    wxString classname;
    wxFileName derivedSource;
    wxFileName derivedHeader;

    void Clear()
    {
        classname.Clear();
        derivedSource.Clear();
        derivedHeader.Clear();
    }
};

class wxCrafterPlugin : public IPlugin
{
    GUICraftMainPanel* m_mainPanel = nullptr;
    wxcTreeView* m_treeView = nullptr;
    wxMenuItem* m_separatorItem = nullptr;
    GeneratedClass m_generatedClassInfo;
    bool m_allEditorsClosing = false;
    bool m_addFileMenu = false;
    bool m_useFrame = true;
    MainFrame* m_mainFrame = nullptr;
    bool m_serverMode = false;
    wxFileName m_selectedFile;

protected:
    wxMenu* DoCreateFolderMenu();
    wxMenu* DoProjectMenu();
    void DoUpdateDerivedClassEventHandlers();
    bool DoReadFileContentAndPrompt(const wxFileName& fn, wxString& content, IEditor** editor);
    void DoWriteFileContent(const wxFileName& fn, const wxString& content, IEditor* editor);
    void DoGenerateCode(const NewFormDetails& fd);
    /**
     * @brief return true if the tab was created, false otherwise.
     * Note that this function only return true if the tab was allocated
     * by this function
     */
    bool DoShowDesigner(bool createIfNotExist = true);
    bool DoCreateVirtualFolder(const wxString& vdFullPath);
    void DoSelectWorkspaceTab();
    void DoLoadAfterImport(ImportDlg::ImportFileData& data);
    void DoImportFB(const wxString& filename = "");
    void DoInitDone(wxObject* obj = NULL);
    void DoLoadWxcProject(const wxFileName& filename);
    void UpdateFileNameInStatusBar();

protected:
    // Event handlers
    void OnInitDone(wxCommandEvent& e);
    void OnShowDesigner(wxCommandEvent& e);
    void OnReGenerateForProject(wxCommandEvent& e);
    void OnDesignerDelete(wxCommandEvent& e);
    void OnBitmapCodeGenerationCompleted(wxCommandEvent& e);
    void OnNewForm(wxCommandEvent& e);
    void OnOpenFile(clCommandEvent& e);
    void OnPageClosing(wxNotifyEvent& e);
    void OnProjectModified(wxCommandEvent& e);
    void OnProjectSynched(wxCommandEvent& e);
    void OnPageChanged(wxCommandEvent& e);
    void OnWorkspaceTabSelected(wxBookCtrlEvent& e);
    void OnDesignerItemSelected(wxCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnBuildStarting(wxCommandEvent& e);
    void OnAllEditorsClosing(wxCommandEvent& e);
    void OnAllEditorsClosed(wxCommandEvent& e);
    void OnDebugStarting(clDebugEvent& e);
    void OnSave(wxCommandEvent& e);
    void OnSaveAll(clCommandEvent& e);
    void OnSaveProject(wxCommandEvent& e);
    void OnSaveProjectUI(wxUpdateUIEvent& e);
    void OnOpenProject(wxCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnOpenProjectUI(wxUpdateUIEvent& e);
    void OnCloseProject(wxCommandEvent& e);
    void OnCloseProjectUI(wxUpdateUIEvent& e);
    void OnImportwxFBProject(wxCommandEvent& e);
    void OnImportwxSmith(wxCommandEvent& e);
    void OnImportXRC(wxCommandEvent& e);
    void OnDefineCustomControls(wxCommandEvent& e);
    void OnEditCustomControls(wxCommandEvent& e);
    void OnDeleteCustomControls(wxCommandEvent& e);
    void OnAbout(wxCommandEvent& e);
    void OnProjectLoaded(wxCommandEvent& e);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnVirtualFolderContextMenu(clContextMenuEvent& event);
    void OnProjectContextMenu(clContextMenuEvent& event);
    void OnImportFBProject(wxCommandEvent& event);
    void OnOpenWxcpProject(wxCommandEvent& event);
    void OnImportwxSmithProject(wxCommandEvent& event);
    void OnToggleView(clCommandEvent& event);
    void OnSaveUI(clCommandEvent& event);

public:
    wxCrafterPlugin(IManager* manager, bool serverMode);
    ~wxCrafterPlugin();

    MainFrame* GetMainFrame() const { return m_mainFrame; }

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();

    IManager* GetManager() { return m_mgr; }
    bool IsTabMode() const { return !m_mainFrame; }

    bool IsMainViewActive();
};

#endif //__wxcrafter_plugin__
