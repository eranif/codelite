#ifndef REMOTYWORKSPACEVIEW_H
#define REMOTYWORKSPACEVIEW_H
#include "RemotyUI.h"
#include "clRemoteDirCtrl.hpp"

class RemotyWorkspace;
class RemotyWorkspaceView : public RemotyWorkspaceViewBase
{
    clRemoteDirCtrl* m_tree = nullptr;
    RemotyWorkspace* m_workspace = nullptr;

protected:
    void OnDirContextMenu(clContextMenuEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);

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
