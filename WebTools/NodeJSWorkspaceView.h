#ifndef NODEJSWORKSPACEVIEW_H
#define NODEJSWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"

class NodeJSWorkspaceView : public clTreeCtrlPanel
{
protected:
    void OnContenxtMenu(clContextMenuEvent& event);
    void OnFolderDropped(clCommandEvent& event);

public:
    NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~NodeJSWorkspaceView();
};

#endif // NODEJSWORKSPACEVIEW_H
