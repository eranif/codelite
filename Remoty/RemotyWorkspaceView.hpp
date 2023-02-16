#ifndef REMOTYWORKSPACEVIEW_H
#define REMOTYWORKSPACEVIEW_H

#include "RemotyUI.h"
#include "SFTPClientData.hpp"
#include "asyncprocess.h"
#include "clRemoteDirCtrl.hpp"
#include "clRemoteFinderHelper.hpp"
#include "cl_command_event.h"

#include <wx/stopwatch.h>

class IEditor;
class RemotyWorkspace;
class RemotyWorkspaceView : public RemotyWorkspaceViewBase
{
    clRemoteDirCtrl* m_tree = nullptr;
    RemotyWorkspace* m_workspace = nullptr;
    wxArrayString m_filesToRestore;

protected:
    void OnDirContextMenu(clContextMenuEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnOpenFindInFilesMatch(clFindInFilesEvent& event);
    void OnRemoteFileSaved(clCommandEvent& event);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);

    void SetBuildConfiguration(const wxString& config);
    void BuildTarget(const wxString& name);

    void DoCloseWorkspace();
    void DoReloadWorkspace();

    wxString GetRemotePathIsOwnedByWorkspace(IEditor* editor) const;
    size_t GetWorkspaceRemoteFilesOpened(wxArrayString* paths) const;

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
};
#endif // REMOTYWORKSPACEVIEW_H
