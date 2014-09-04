#ifndef __php_workspace_view__
#define __php_workspace_view__

#include "php_ui.h"
#include "php_workspace.h"
#include <set>
#include <bitmap_loader.h>
#include "php_event.h"
#include <cl_command_event.h>

class IManager;
class ItemData;

class PHPWorkspaceView : public PHPWorkspaceViewBase
{
    IManager* m_mgr;
    BitmapLoader::BitmapMap_t m_bitmaps;

private:
    enum {
        ID_TOGGLE_AUTOMATIC_UPLOAD = wxID_HIGHEST +1,
    };

protected:
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnMenu(wxTreeEvent& event);
    virtual void OnSetupRemoteUpload(wxAuiToolBarEvent& event);
    virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);
    virtual void OnActiveProjectSettings(wxCommandEvent& event);
    virtual void OnProjectSettings(wxCommandEvent& event);
    virtual void OnActiveProjectSettingsUI(wxUpdateUIEvent& event);

    // Helpers
    wxTreeItemId              DoGetSingleSelection();
    wxString                  DoGetSelectedProject();
    ItemData* DoGetItemData   (const wxTreeItemId& item);
    const ItemData* DoGetItemData(const wxTreeItemId& item) const;
    bool IsFolderItem    (const wxTreeItemId& item);
    void DoAddFolderFiles(const wxString &project, const wxString &folder, const wxTreeItemId& folderTreeItemId, wxStringSet_t *filesAdded = NULL);
    void DoAddFilesToTreeView(const wxTreeItemId& folderId, PHPProject::Ptr_t pProject, PHPFolder::Ptr_t pFolder, const wxArrayString& files);
    int                       DoGetItemImgIdx(const wxString &filename);
    wxBitmap                  DoGetBitmapForExt(const wxString &ext) const;
    void                      DoDeleteSelectedFileItem();
    void                      DoAddFileWithContent(const wxTreeItemId& folderId, const wxFileName& filename, const wxString& content);
    void                      DoOpenSSHAccountManager();
    wxTreeItemId EnsureFolderExists(const wxTreeItemId& projectItem, const wxString &path, const wxString &project);
    bool HasFolderWithName(const wxTreeItemId& parent, const wxString &name, wxTreeItemId& item) const;
    wxTreeItemId DoGetProjectItem(const wxString& projectName);
    wxTreeItemId DoGetFolderItem(const wxTreeItemId& projectItem, PHPFolder::Ptr_t pFolder);
    /**
     * @brief search for the filename in the given folder (does not work recursively)
     */
    wxTreeItemId DoGetFileItem(const wxTreeItemId& folderItem, const wxString &filename);
    /**
     * @brief construct the project in the tree view
     * @param projectItem
     * @param project
     */
    void DoBuildProjectNode(const wxTreeItemId& projectItem, PHPProject::Ptr_t project);
    /**
     * @brief add folder to the tree view
     * @param parent
     * @param folder
     */
    void DoAddFolder(const wxTreeItemId& parent, PHPFolder::Ptr_t folder, PHPProject::Ptr_t project);
    
    /**
     * @brief open an item into an editor
     */
    void DoOpenFile(const wxTreeItemId& item);
    
    /**
     * @brief add files to the project file + view. 
     * @param paths the paths to add. must be in full path and all must exists under the same directory
     * @param pProject
     * @param notify when set to true, send a wxEVT_PROJ_FILE_ADDED event with the list of files that were added
     */
    void DoAddFilesToFolder(const wxArrayString& paths, PHPProject::Ptr_t pProject, bool notify);
    
protected:
    // Handlers for PHPWorkspaceViewBase events.

    // Menu handlers
    DECLARE_EVENT_TABLE()
    
    void OnCloseWorkspace(wxCommandEvent &e);
    void OnReloadWorkspace(wxCommandEvent &e);
    void OnNewFolder(wxCommandEvent &e);
    void OnNewClass(wxCommandEvent &e);
    void OnDeleteProject(wxCommandEvent &e);
    void OnSetProjectActive(wxCommandEvent &e);
    void OnNewFile(wxCommandEvent &e);
    void OnAddFile(wxCommandEvent &e);
    void OnDeleteFolder(wxCommandEvent &e);
    void OnImportFiles(wxCommandEvent &e);
    void OnRetagWorkspace(wxCommandEvent &e);
    void OnRemoveFile(wxCommandEvent &e);
    void OnOpenFile(wxCommandEvent &e);
    void OnRenameFile(wxCommandEvent &e);
    void OnRenameWorkspace(wxCommandEvent &e);
    void OnRunProject(wxCommandEvent &e);
    void OnMakeIndexPHP(wxCommandEvent &e);
    void OnRunActiveProject(clExecuteEvent& e);
    void OnStopExecutedProgram(wxCommandEvent &e);
    void OnIsProgramRunning(wxCommandEvent &e);
    void OnEditorChanged(wxCommandEvent &e);
    void OnFileRenamed(PHPEvent &e);
    void OnWorkspaceRenamed(PHPEvent &e);
    void OnToggleAutoUpload(wxCommandEvent &e);

public:
    /** Constructor */
    PHPWorkspaceView( wxWindow* parent, IManager* mgr );
    virtual ~PHPWorkspaceView();
    
    /**
     * @brief create a new project
     * @param name
     */
    void CreateNewProject(const wxString& name);
    
    void LoadWorkspace();
    void UnLoadWorkspace();

    void ReportParseThreadProgress(size_t curIndex, size_t total);
    void ReportParseThreadDone();
    void ReloadWorkspace( bool saveBeforeReload );
};

#endif // __php_workspace_view__
