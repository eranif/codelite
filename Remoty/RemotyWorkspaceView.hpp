#ifndef REMOTYWORKSPACEVIEW_H
#define REMOTYWORKSPACEVIEW_H

#include "RemotyUI.h"
#include "clRemoteDirCtrl.hpp"
#include "cl_command_event.h"

#include <wx/aui/auibar.h>
#include <wx/stopwatch.h>

class IEditor;
class RemotyWorkspace;

class RemotyWorkspaceView : public RemotyWorkspaceViewBase
{
public:
    RemotyWorkspaceView(wxWindow* parent, RemotyWorkspace* workspace);
    virtual ~RemotyWorkspaceView();

    /**
     * @brief open a workspace on a given remote folder
     */
    void OpenWorkspace(const wxString& path, const wxString& accountName);

    /**
     * @brief close the workspace view
     */
    void CloseWorkspace();

protected:
    void OnDirContextMenu(clContextMenuEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnOpenFindInFilesMatch(clFindInFilesEvent& event);
    void OnRemoteFileSaved(clCommandEvent& event);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnEditWorkspaceSettings(wxCommandEvent& event);
    void OnConfigSelected(wxCommandEvent& event);
    void OnEditCodeLiteRemoteJsonFile(wxCommandEvent& event);
    void OnBuildMenu(wxAuiToolBarEvent& event);
    void SetBuildConfiguration(const wxString& config);
    void BuildTarget(const wxString& name);

    void DoCloseWorkspace();
    void DoReloadWorkspace();
    void UpdateConfigChoices();

    wxString GetRemotePathIsOwnedByWorkspace(IEditor* editor) const;
    size_t GetWorkspaceRemoteFilesOpened(wxArrayString* paths) const;

private:
    clRemoteDirCtrl* m_tree{nullptr};
    RemotyWorkspace* m_workspace{nullptr};
    wxAuiToolBar* m_toolbar{nullptr};
    wxChoice* m_configs{nullptr};
    wxArrayString m_filesToRestore;
};
#endif // REMOTYWORKSPACEVIEW_H
