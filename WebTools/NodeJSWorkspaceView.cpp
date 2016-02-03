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
#include <wx/wupdlock.h>
#include <wx/msgdlg.h>
#include "NodeJSPackageJSON.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSDebugger.h"
#include "macros.h"
#include "WebToolsConfig.h"
#include "environmentconfig.h"
#include "dirsaver.h"
#include "bitmap_loader.h"

NodeJSWorkspaceView::NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName)
    : clTreeCtrlPanel(parent)
{
    SetNewFileTemplate("Untitled.js", wxStrlen("Untitled"));
    SetViewName(viewName);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &NodeJSWorkspaceView::OnContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &NodeJSWorkspaceView::OnContextMenuFile, this);
    m_keyboardHelper.reset(new clTreeKeyboardInput(GetTreeCtrl()));
}

NodeJSWorkspaceView::~NodeJSWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &NodeJSWorkspaceView::OnContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &NodeJSWorkspaceView::OnContextMenuFile, this);
    m_keyboardHelper.reset(NULL); // destory the keyboard input helper
}

void NodeJSWorkspaceView::OnContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    WebToolsConfig webToolsConf;
    webToolsConf.Load();
    if((event.GetEventObject() == this)) {
        wxMenu* menu = event.GetMenu();

        // Locate the "Close" menu entry
        int pos = wxNOT_FOUND;
        int openShellPos = wxNOT_FOUND;
        wxMenuItem* closeItem = NULL;

        for(size_t i = 0; i < menu->GetMenuItemCount(); ++i) {
            wxMenuItem* mi = menu->FindItemByPosition(i);
            if((pos == wxNOT_FOUND) && mi && (mi->GetId() == XRCID("tree_ctrl_close_folder"))) {
                pos = i;
                closeItem = mi;
            }

            if((openShellPos == wxNOT_FOUND) && mi && (mi->GetId() == XRCID("tree_ctrl_open_shell_folder"))) {
                openShellPos = i;
                ++openShellPos; // insert after
                ++openShellPos; // skip the separator
            }
        }

        if((pos != wxNOT_FOUND) && closeItem) {
            wxMenuItem* showHiddenItem =
                menu->Insert(pos, XRCID("nodejs_show_hidden_files"), _("Show hidden files"), "", wxITEM_CHECK);
            NodeJSWorkspaceConfiguration conf;
            showHiddenItem->Check(conf.Load(NodeJSWorkspace::Get()->GetFilename()).IsShowHiddenFiles());
            menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnShowHiddenFiles, this, XRCID("nodejs_show_hidden_files"));

            menu->InsertSeparator(pos);
            menu->Insert(pos, XRCID("nodejs_close_workspace"), _("Close Workspace"));
            menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnCloseWorkspace, this, XRCID("nodejs_close_workspace"));

            // Remove the 'close' menu item
            menu->Remove(closeItem);
        }

        // Add project related items
        wxArrayString folders, files;
        wxArrayTreeItemIds folderItems, fileItems;
        GetSelections(folders, folderItems, files, fileItems);

        if((openShellPos != wxNOT_FOUND) && (folderItems.size() == 1) && (fileItems.size() == 0)) {
            // only a folder is selected, check to see if this is a project folder
            // we do this by testing for the existence of the file package.json
            // under the folder path
            wxFileName packageJSON(folders.Item(0), "package.json");
            if(packageJSON.FileExists()) {
                // A project folder
                menu->InsertSeparator(openShellPos);
                menu->Insert(openShellPos, XRCID("nodejs_project_settings"), _("Open package.json"));
                menu->Insert(openShellPos, XRCID("nodejs_project_debug"), _("Debug..."));
                menu->Insert(openShellPos, XRCID("nodejs_project_run"), _("Run..."));

                menu->Bind(
                    wxEVT_MENU, &NodeJSWorkspaceView::OnOpenPackageJsonFile, this, XRCID("nodejs_project_settings"));
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnProjectDebug, this, XRCID("nodejs_project_debug"));
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnProjectRun, this, XRCID("nodejs_project_run"));
            } else {
                // Check to see if we got 'npm' detected on this machine
                bool hasNpm = !webToolsConf.GetNpm().IsEmpty();

                // Offer a "npm init" menu entry
                menu->InsertSeparator(openShellPos);
                wxMenuItem* menuItem = new wxMenuItem(NULL, XRCID("nodejs_npm_init"), _("Run 'npm init' here"));
                menu->Insert(openShellPos, menuItem);
                menuItem->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("console"));
                menu->Enable(XRCID("nodejs_npm_init"), hasNpm && !m_terminal.IsRunning());
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnNpmInit, this, XRCID("nodejs_npm_init"));
            }
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

void NodeJSWorkspaceView::OnCloseWorkspace(wxCommandEvent& event)
{
    // Simulate the menu event "Close Workspace"
    wxUnusedVar(event);
    wxCommandEvent eventCloseWorkspace(wxEVT_MENU, XRCID("close_workspace"));
    eventCloseWorkspace.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(eventCloseWorkspace);
}

void NodeJSWorkspaceView::OnContextMenuFile(clContextMenuEvent& event) { event.Skip(); }

void NodeJSWorkspaceView::OnOpenPackageJsonFile(wxCommandEvent& event)
{
    wxString path;
    wxTreeItemId item;
    if(!GetSelectProjectPath(path, item)) return;

    wxFileName packageJson(path, "package.json");
    clGetManager()->OpenFile(packageJson.GetFullPath());
}

void NodeJSWorkspaceView::OnProjectDebug(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteProject(NodeJSDebuggerDlg::kDebug);
}

void NodeJSWorkspaceView::OnProjectRun(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteProject(NodeJSDebuggerDlg::kExecute);
}

bool NodeJSWorkspaceView::GetSelectProjectPath(wxString& path, wxTreeItemId& item)
{
    path.clear();
    wxArrayString folders, files;
    wxArrayTreeItemIds folderItems, fileItems;
    GetSelections(folders, folderItems, files, fileItems);
    if((folders.size() == 1) && (files.IsEmpty())) {
        path = folders.Item(0);
        item = folderItems.Item(0);
        return true;
    }
    return false;
}

void NodeJSWorkspaceView::DoExecuteProject(NodeJSDebuggerDlg::eDialogType type)
{
    wxString path;
    wxTreeItemId item;
    if(!GetSelectProjectPath(path, item)) return;

    NodeJSPackageJSON pj;
    if(!pj.Load(path)) {
        if(!pj.Create(path)) {
            ::wxMessageBox(
                _("Failed to load package.json file from path:\n") + path, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
    }

    // Sanity

    // No debugger?
    if(!NodeJSWorkspace::Get()->GetDebugger()) return;
    // Already running?
    if(NodeJSWorkspace::Get()->GetDebugger()->IsConnected()) return;

    NodeJSDebuggerDlg dlg(EventNotifier::Get()->TopFrame(), type, pj.GetScript(), pj.GetArgs());
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    // store the data for future use
    pj.SetScript(dlg.GetScript());
    pj.SetArgs(dlg.GetArgs());
    pj.Save(path);

    NodeJSWorkspace::Get()->GetDebugger()->StartDebugger(dlg.GetCommand(), dlg.GetWorkingDirectory());
}

void NodeJSWorkspaceView::OnItemExpanding(wxTreeEvent& event)
{
    // Call the parent function to do the actual expansion
    clTreeCtrlPanel::OnItemExpanding(event);

    // Loop over the children of the item and replace the icon for all projects
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);

    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    int imageIndex = m_bmpLoader->GetMimeImageId(FileExtManager::TypeProject);
    CHECK_COND_RET(imageIndex != wxNOT_FOUND);

    {
        // change the icon for the parent folder as well
        wxFileName packageJSON(cd->GetPath(), "package.json");
        if(packageJSON.FileExists()) {
            GetTreeCtrl()->SetItemImage(item, imageIndex);
            GetTreeCtrl()->SetItemImage(item, imageIndex, wxTreeItemIcon_Selected);
        }
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetTreeCtrl()->GetFirstChild(item, cookie);
    while(child.IsOk()) {
        clTreeCtrlData* cd = GetItemData(child);
        if(cd && cd->IsFolder()) {
            wxFileName packageJSON(cd->GetPath(), "package.json");
            if(packageJSON.FileExists()) {
                // A project
                GetTreeCtrl()->SetItemImage(child, imageIndex);
                GetTreeCtrl()->SetItemImage(child, imageIndex, wxTreeItemIcon_Selected);
            }
        }
        child = GetTreeCtrl()->GetNextChild(item, cookie);
    }
}

void NodeJSWorkspaceView::OnNpmInit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_terminal.IsRunning()) return;

    wxString path;
    wxTreeItemId item;
    if(!GetSelectProjectPath(path, item)) return;

    WebToolsConfig conf;
    conf.Load();

#ifdef __WXOSX__
    wxString command;
    command << "cd \"" << path << "\" && " << conf.GetNpm() << " init";
#else
    wxString command;
    command << conf.GetNpm();
    ::WrapWithQuotes(command);
#endif

    command << " init";
    m_terminal.ExecuteConsole(command, path, true, "npm init");
}
