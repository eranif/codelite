#include "NodeJSWorkspaceView.h"
#include "NoteJSWorkspace.h"

NodeJSWorkspaceView::NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName)
    : clTreeCtrlPanel(parent)
{
    SetViewName(viewName);
}

NodeJSWorkspaceView::~NodeJSWorkspaceView()
{
}
