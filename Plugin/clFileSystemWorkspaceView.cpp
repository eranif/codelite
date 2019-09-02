#include "clFileSystemWorkspaceView.hpp"
#include "globals.h"
#include "clFileSystemWorkspace.hpp"
#include "clWorkspaceView.h"

clFileSystemWorkspaceView::clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName)
    : clTreeCtrlPanel(parent)
    , m_config("fs-workspace-config")
{
    SetConfig(&m_config);
    SetNewFileTemplate("Untitled.cpp", wxStrlen("Untitled"));
    SetViewName(viewName);
    
    // Hide hidden folders and files
    m_options &= ~kShowHiddenFiles;
    m_options &= ~kShowHiddenFolders;
}

clFileSystemWorkspaceView::~clFileSystemWorkspaceView() {}

void clFileSystemWorkspaceView::OnFolderDropped(clCommandEvent& event)
{
    // Add only non existent folders to the workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.size() != 1) { return; }

    clFileSystemWorkspace::Get().New(folders.Item(0));
    ::clGetManager()->GetWorkspaceView()->SelectPage(GetViewName());
}
