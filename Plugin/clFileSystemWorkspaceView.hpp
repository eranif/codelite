#ifndef CLFILESYSTEMWORKSPACEVIEW_HPP
#define CLFILESYSTEMWORKSPACEVIEW_HPP

#include "clTreeCtrlPanel.h"
#include "cl_config.h"
#include "cl_command_event.h"

class WXDLLIMPEXP_SDK clFileSystemWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;

protected:
    void OnFolderDropped(clCommandEvent& event);
    void OnContextMenu(clContextMenuEvent& event);
    void OnCloseFolder(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);

public:
    clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~clFileSystemWorkspaceView();
};

#endif // CLFILESYSTEMWORKSPACEVIEW_HPP
