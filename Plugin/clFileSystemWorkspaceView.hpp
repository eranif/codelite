#ifndef CLFILESYSTEMWORKSPACEVIEW_HPP
#define CLFILESYSTEMWORKSPACEVIEW_HPP

#include "clTreeCtrlPanel.h"
#include "cl_config.h"

class WXDLLIMPEXP_SDK clFileSystemWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;

protected:
    void OnFolderDropped(clCommandEvent& event);

public:
    clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~clFileSystemWorkspaceView();
};

#endif // CLFILESYSTEMWORKSPACEVIEW_HPP
