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
#include "NodeJSWorkspaceConfiguration.h"
#include "ieditor.h"

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
        wxMenu* menu = event.GetMenu();
        // Remove the 'Close folder' option
        wxMenuItem* item = menu->FindItem(XRCID("tree_ctrl_close_folder"));
        if(item) {
            menu->Remove(item);
            wxMenuItem* sepItem = menu->FindItemByPosition(0);
            if(sepItem) {
                menu->Remove(sepItem);
            }
        }

        int pos = wxNOT_FOUND;
        for(size_t i = 0; i < menu->GetMenuItemCount(); ++i) {
            wxMenuItem* mi = menu->FindItemByPosition(i);
            if(mi && mi->GetId() == wxID_REFRESH) {
                pos = i;
                break;
            }
        }

        if(pos != wxNOT_FOUND) {
            wxMenuItem* showHiddenItem =
                menu->Insert(pos + 2, XRCID("nodejs_show_hidden_files"), _("Show hidden files"), "", wxITEM_CHECK);
            NodeJSWorkspaceConfiguration conf;
            showHiddenItem->Check(conf.Load(NodeJSWorkspace::Get()->GetFilename()).IsShowHiddenFiles());
            menu->InsertSeparator(pos + 3);
            menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnShowHiddenFiles, this, XRCID("nodejs_show_hidden_files"));
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

void NodeJSWorkspaceView::RebuildTree()
{
    wxWindowUpdateLocker locker(this);
    wxArrayString paths;
    wxArrayTreeItemIds items;
    GetTopLevelFolders(paths, items);

    Clear();

    for(size_t i = 0; i < paths.size(); ++i) {
        AddFolder(paths.Item(i));
    }

    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        ExpandToFile(editor->GetFileName());
    }
}

void NodeJSWorkspaceView::ShowHiddenFiles(bool show)
{
    if(show) {
        m_options |= kShowHiddenFiles;
        m_options |= kShowHiddenFolders;
    } else {
        m_options &= ~kShowHiddenFiles;
        m_options &= ~kShowHiddenFolders;
    }
}

void NodeJSWorkspaceView::OnShowHiddenFiles(wxCommandEvent& event)
{
    NodeJSWorkspaceConfiguration conf;
    const wxFileName& filename = NodeJSWorkspace::Get()->GetFilename();
    conf.Load(filename).SetShowHiddenFiles(event.IsChecked()).Save(filename);
    ShowHiddenFiles(event.IsChecked());
    RebuildTree();
}
