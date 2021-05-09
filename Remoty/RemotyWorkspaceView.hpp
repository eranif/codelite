#ifndef REMOTYWORKSPACEVIEW_H
#define REMOTYWORKSPACEVIEW_H
#include "RemotyUI.h"
#include "asyncprocess.h"
#include "clRemoteDirCtrl.hpp"
#include "clRemoteFinderHelper.hpp"
#include "cl_command_event.h"
#include <wx/stopwatch.h>

class RemotyWorkspace;
class RemotyWorkspaceView : public RemotyWorkspaceViewBase
{
    clRemoteDirCtrl* m_tree = nullptr;
    RemotyWorkspace* m_workspace = nullptr;

protected:
    void OnDirContextMenu(clContextMenuEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnOpenFindInFilesMatch(clFindInFilesEvent& event);
    void SetBuildConfiguration(const wxString& config);
    void BuildTarget(const wxString& name);

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
