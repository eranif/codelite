#include "NodeJSWorkspaceView.h"
#include "NoteJSWorkspace.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/menu.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include "globals.h"
#include "clWorkspaceView.h"
#include "imanager.h"

NodeJSWorkspaceView::NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName)
    : clTreeCtrlPanel(parent)
{
    SetNewFileTemplate("Untitled.js", wxStrlen("Untitled"));
    SetViewName(viewName);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &NodeJSWorkspaceView::OnContenxtMenu, this);
}

NodeJSWorkspaceView::~NodeJSWorkspaceView()
{
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &NodeJSWorkspaceView::OnContenxtMenu, this);
}

void NodeJSWorkspaceView::OnContenxtMenu(clContextMenuEvent& event)
{
    event.Skip();
    if(event.GetEventObject() == this) {
        // Remove the 'Close folder' option
        event.GetMenu()->Remove(XRCID("tree_ctrl_close_folder"));
        wxMenuItem* sepItem = event.GetMenu()->FindItemByPosition(0);
        if(sepItem) {
            event.GetMenu()->Remove(sepItem);
        }
    }
}

void NodeJSWorkspaceView::OnFolderDropped(clCommandEvent& event)
{
    // Add only non existent folders to the workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.IsEmpty()) return;

    if(!NodeJSWorkspace::Get()->IsOpen()) {
        wxFileName workspaceFile(folders.Item(0), "");
        if(!workspaceFile.GetDirCount()) {
            ::wxMessageBox(
                _("Can not create workspace in the root folder"), _("New Workspace"), wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        workspaceFile.SetName(workspaceFile.GetDirs().Last());
        workspaceFile.SetExt("workspace");
        // Create will fail if a file with this name already exists
        NodeJSWorkspace::Get()->Create(workspaceFile);
        // Load the workspace, again, it will fail if this is not a valid 
        // NodeJS workspace
        NodeJSWorkspace::Get()->Open(workspaceFile);
    }

    if(NodeJSWorkspace::Get()->IsOpen()) {
        wxArrayString& workspaceFolders = NodeJSWorkspace::Get()->GetFolders();
        for(size_t i = 0; i < folders.size(); ++i) {
            if(workspaceFolders.Index(folders.Item(i)) == wxNOT_FOUND) {
                // New folder, add it to the workspace
                workspaceFolders.Add(folders.Item(i));
                AddFolder(folders.Item(i));
            }
        }
        NodeJSWorkspace::Get()->Save();
    }
    ::clGetManager()->GetWorkspaceView()->SelectPage(GetViewName());
}
