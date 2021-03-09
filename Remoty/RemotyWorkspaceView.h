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
    void Open(const wxString& path, const wxString& accountName);
};
#endif // REMOTYWORKSPACEVIEW_H
