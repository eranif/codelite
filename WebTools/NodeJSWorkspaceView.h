#ifndef NODEJSWORKSPACEVIEW_H
#define NODEJSWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"

class NodeJSWorkspaceView : public clTreeCtrlPanel
{
public:
    NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~NodeJSWorkspaceView();
};

#endif // NODEJSWORKSPACEVIEW_H
