#ifndef REMOTYWORKSPACEVIEW_H
#define REMOTYWORKSPACEVIEW_H
#include "RemotyUI.h"
#include "clRemoteDirCtrl.hpp"

class RemotyWorkspaceView : public RemotyWorkspaceViewBase
{
    clRemoteDirCtrl* m_tree = nullptr;

public:
    RemotyWorkspaceView(wxWindow* parent);
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
