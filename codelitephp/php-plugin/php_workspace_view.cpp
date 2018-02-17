#include "FilesCollector.h"
#include "NewPHPClass.h"
#include "PHPDebugStartDlg.h"
#include "PHPLookupTable.h"
#include "XDebugManager.h"
#include "clFileOrFolderDropTarget.h"
#include "clFileSystemEvent.h"
#include "clWorkspaceView.h"
#include "cl_aui_tool_stickness.h"
#include "file_logger.h"
#include "fileutils.h"
#include "new_file_dlg.h"
#include "php_configuration_data.h"
#include "php_project_settings_dlg.h"
#include "php_strings.h"
#include "php_workspace.h"
#include "php_workspace_view.h"
#include "ssh_workspace_settings.h"
#include "tree_item_data.h"
#include <SFTPBrowserDlg.h>
#include <SSHAccountManagerDlg.h>
#include <bitmap_loader.h>
#include <cl_command_event.h>
#include <editor_config.h>
#include <event_notifier.h>
#include <fileextmanager.h>
#include <globals.h>
#include <imanager.h>
#include <macros.h>
#include <plugin.h>
#include <sftp_settings.h>
#include <ssh_account_info.h>
#include <wx/busyinfo.h>
#include <wx/filedlg.h>
#include <wx/imaglist.h>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>

#define CHECK_ID_FOLDER(id) \
    if(!id->IsFolder()) return
#define CHECK_ID_PROJECT(id) \
    if(!id->IsProject()) return
#define CHECK_ID_FILE(id) \
    if(!id->IsFile()) return

BEGIN_EVENT_TABLE(PHPWorkspaceView, PHPWorkspaceViewBase)
EVT_MENU(XRCID("php_close_workspace"), PHPWorkspaceView::OnCloseWorkspace)
EVT_MENU(XRCID("php_reload_workspace"), PHPWorkspaceView::OnReloadWorkspace)
EVT_MENU(XRCID("php_set_project_active"), PHPWorkspaceView::OnSetProjectActive)
EVT_MENU(XRCID("php_delete_project"), PHPWorkspaceView::OnDeleteProject)
EVT_MENU(XRCID("php_new_folder"), PHPWorkspaceView::OnNewFolder)
EVT_MENU(XRCID("php_rename_folder"), PHPWorkspaceView::OnRenameFolder)
EVT_MENU(XRCID("php_new_class"), PHPWorkspaceView::OnNewClass)
EVT_MENU(XRCID("php_new_file"), PHPWorkspaceView::OnNewFile)
EVT_MENU(XRCID("php_delete_folder"), PHPWorkspaceView::OnDeleteFolder)
EVT_MENU(XRCID("php_open_folder_in_explorer"), PHPWorkspaceView::OnOpenInExplorer)
EVT_MENU(XRCID("php_open_shell"), PHPWorkspaceView::OnOpenShell)
EVT_MENU(XRCID("php_reparse_workspace"), PHPWorkspaceView::OnRetagWorkspace)
EVT_MENU(XRCID("add_existing_project"), PHPWorkspaceView::OnAddExistingProject)
EVT_MENU(XRCID("php_folder_find_in_files"), PHPWorkspaceView::OnFindInFiles)
EVT_MENU(XRCID("php_remove_file"), PHPWorkspaceView::OnRemoveFile)
EVT_MENU(XRCID("php_open_file"), PHPWorkspaceView::OnOpenFile)
EVT_MENU(XRCID("rename_php_file"), PHPWorkspaceView::OnRenameFile)
EVT_MENU(XRCID("rename_php_workspace"), PHPWorkspaceView::OnRenameWorkspace)
EVT_MENU(XRCID("php_project_settings"), PHPWorkspaceView::OnProjectSettings)
EVT_MENU(XRCID("php_run_project"), PHPWorkspaceView::OnRunProject)
EVT_MENU(XRCID("make_index"), PHPWorkspaceView::OnMakeIndexPHP)
EVT_MENU(XRCID("php_synch_with_filesystem"), PHPWorkspaceView::OnSyncWorkspaceWithFileSystem)
EVT_MENU(XRCID("php_sync_project_with_filesystem"), PHPWorkspaceView::OnSyncProjectWithFileSystem)
EVT_MENU(XRCID("php_sync_folder_with_filesystem"), PHPWorkspaceView::OnSyncFolderWithFileSystem)
EVT_MENU(XRCID("php_open_with_default_app"), PHPWorkspaceView::OnOpenWithDefaultApp)
END_EVENT_TABLE()

PHPWorkspaceView::PHPWorkspaceView(wxWindow* parent, IManager* mgr)
    : PHPWorkspaceViewBase(parent)
    , m_mgr(mgr)
    , m_scanInProgress(true)
{
    MSWSetNativeTheme(m_treeCtrlView);
    // SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    // Initialise images map
    BitmapLoader* bmpLoader = m_mgr->GetStdIcons();
    m_bitmaps = bmpLoader->MakeStandardMimeMap();
    EventNotifier::Get()->Connect(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT,
                                  clExecuteEventHandler(PHPWorkspaceView::OnRunActiveProject), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &PHPWorkspaceView::OnStopExecutedProgram, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_IS_PROGRAM_RUNNING, &PHPWorkspaceView::OnIsProgramRunning, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(PHPWorkspaceView::OnEditorChanged),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PHP_FILE_RENAMED, PHPEventHandler(PHPWorkspaceView::OnFileRenamed), NULL, this);
    EventNotifier::Get()->Bind(wxPHP_PARSE_ENDED, &PHPWorkspaceView::OnPhpParserDone, this);
    EventNotifier::Get()->Bind(wxPHP_PARSE_PROGRESS, &PHPWorkspaceView::OnPhpParserProgress, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_WORKSPACE_LOADED, &PHPWorkspaceView::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_WORKSPACE_RENAMED, &PHPWorkspaceView::OnWorkspaceRenamed, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_FIND_IN_FILES_SHOWING, &PHPWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVEAS, &PHPWorkspaceView::OnFileSaveAs, this);
    Bind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPWorkspaceView::OnProjectSyncCompleted, this);

    BitmapLoader* bl = m_mgr->GetStdIcons();
    wxImageList* imageList = bl->MakeStandardMimeImageList();
    m_treeCtrlView->AssignImageList(imageList);

    m_keyboardHelper.reset(new clTreeKeyboardInput(m_treeCtrlView));

    // Allow the PHP view to accepts folders
    m_treeCtrlView->SetDropTarget(new clFileOrFolderDropTarget(this));
    m_treeCtrlView->Bind(wxEVT_TREE_BEGIN_DRAG, &PHPWorkspaceView::OnDragBegin, this);
    m_treeCtrlView->Bind(wxEVT_TREE_END_DRAG, &PHPWorkspaceView::OnDragEnd, this);
    Bind(wxEVT_DND_FOLDER_DROPPED, &PHPWorkspaceView::OnFolderDropped, this);

    // Build the toolbar
    m_auibar29->AddTool(XRCID("ID_PHP_PROJECT_SETTINGS"), _("Open active project settings"), bl->LoadBitmap("cog"),
                        _("Open active project settings"), wxITEM_NORMAL);
    m_auibar29
        ->AddTool(XRCID("ID_PHP_PROJECT_REMOTE_SAVE"), _("Setup automatic upload"), bl->LoadBitmap("remote-folder"),
                  _("Setup automatic upload"), wxITEM_NORMAL)
        ->SetHasDropDown(true);
    m_auibar29->AddSeparator();
    m_auibar29->AddTool(XRCID("ID_TOOL_COLLAPSE"), _("Collapse All"), bl->LoadBitmap("fold"), _("Collapse All"),
                        wxITEM_NORMAL);
    m_auibar29->AddTool(XRCID("ID_TOOL_SYNC_WORKSPACE"), _("Collapse All"), bl->LoadBitmap("debugger_restart"),
                        _("Sync workspace with file system..."), wxITEM_NORMAL);
    m_auibar29->AddSeparator();
    m_auibar29->AddTool(XRCID("ID_TOOL_START_DEBUGGER_LISTENER"), _("Wait for Debugger Connection"),
                        bl->LoadBitmap("debugger_start"), _("Wait for Debugger Connection"));
    m_auibar29->Realize();

    // Bind events
    Bind(wxEVT_MENU, &PHPWorkspaceView::OnActiveProjectSettings, this, XRCID("ID_PHP_PROJECT_SETTINGS"));
    Bind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnActiveProjectSettingsUI, this, XRCID("ID_PHP_PROJECT_SETTINGS"));

#if USE_SFTP
    Bind(wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, &PHPWorkspaceView::OnSetupRemoteUpload, this,
         XRCID("ID_PHP_PROJECT_REMOTE_SAVE"));
#endif
    Bind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnSetupRemoteUploadUI, this, XRCID("ID_PHP_PROJECT_REMOTE_SAVE"));

    Bind(wxEVT_MENU, &PHPWorkspaceView::OnCollapse, this, XRCID("ID_TOOL_COLLAPSE"));
    Bind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnCollapseUI, this, XRCID("ID_TOOL_COLLAPSE"));
    Bind(wxEVT_MENU, &PHPWorkspaceView::OnStartDebuggerListener, this, XRCID("ID_TOOL_START_DEBUGGER_LISTENER"));
    Bind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnStartDebuggerListenerUI, this, XRCID("ID_TOOL_START_DEBUGGER_LISTENER"));

    Bind(wxEVT_PHP_WORKSPACE_FILES_SYNC_START, &PHPWorkspaceView::OnWorkspaceSyncStart, this);
    Bind(wxEVT_PHP_WORKSPACE_FILES_SYNC_END, &PHPWorkspaceView::OnWorkspaceSyncEnd, this);

    Bind(wxEVT_MENU, &PHPWorkspaceView::OnSyncWorkspaceWithFileSystem, this, XRCID("ID_TOOL_SYNC_WORKSPACE"));
    Bind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnSyncWorkspaceWithFileSystemUI, this, XRCID("ID_TOOL_SYNC_WORKSPACE"));
}

PHPWorkspaceView::~PHPWorkspaceView()
{
    EventNotifier::Get()->Disconnect(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT,
                                     clExecuteEventHandler(PHPWorkspaceView::OnRunActiveProject), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &PHPWorkspaceView::OnStopExecutedProgram, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_IS_PROGRAM_RUNNING, &PHPWorkspaceView::OnIsProgramRunning, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(PHPWorkspaceView::OnEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PHP_FILE_RENAMED, PHPEventHandler(PHPWorkspaceView::OnFileRenamed), NULL,
                                     this);
    EventNotifier::Get()->Unbind(wxPHP_PARSE_ENDED, &PHPWorkspaceView::OnPhpParserDone, this);
    EventNotifier::Get()->Unbind(wxPHP_PARSE_PROGRESS, &PHPWorkspaceView::OnPhpParserProgress, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_WORKSPACE_LOADED, &PHPWorkspaceView::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_WORKSPACE_RENAMED, &PHPWorkspaceView::OnWorkspaceRenamed, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_FIND_IN_FILES_SHOWING, &PHPWorkspaceView::OnFindInFilesShowing, this);
    Unbind(wxEVT_DND_FOLDER_DROPPED, &PHPWorkspaceView::OnFolderDropped, this);
    Unbind(wxEVT_PHP_WORKSPACE_FILES_SYNC_START, &PHPWorkspaceView::OnWorkspaceSyncStart, this);
    Unbind(wxEVT_PHP_WORKSPACE_FILES_SYNC_END, &PHPWorkspaceView::OnWorkspaceSyncEnd, this);
    Unbind(wxEVT_MENU, &PHPWorkspaceView::OnStartDebuggerListener, this, XRCID("ID_TOOL_START_DEBUGGER_LISTENER"));
    Unbind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnStartDebuggerListenerUI, this,
           XRCID("ID_TOOL_START_DEBUGGER_LISTENER"));
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVEAS, &PHPWorkspaceView::OnFileSaveAs, this);
    Unbind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPWorkspaceView::OnProjectSyncCompleted, this);

    Unbind(wxEVT_MENU, &PHPWorkspaceView::OnSyncWorkspaceWithFileSystem, this, XRCID("ID_TOOL_SYNC_WORKSPACE"));
    Unbind(wxEVT_UPDATE_UI, &PHPWorkspaceView::OnSyncWorkspaceWithFileSystemUI, this, XRCID("ID_TOOL_SYNC_WORKSPACE"));
}

void PHPWorkspaceView::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    if(folders.GetCount() != 1) {
        ::wxMessageBox(_("Can only import one folder at a time"), "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
        return;
    }

    // If a workspace is already exist at the selected path - load it
    wxArrayString workspaceFiles;
    wxString workspaceFile;
    wxDir::GetAllFiles(folders.Item(0), &workspaceFiles, "*.workspace", wxDIR_FILES);
    // Check the workspace type
    for(size_t i = 0; i < workspaceFiles.size(); ++i) {
        if(FileExtManager::GetType(workspaceFiles.Item(i)) == FileExtManager::TypeWorkspacePHP) {
            workspaceFile = workspaceFiles.Item(i);
            break;
        }
    }

    wxFileName workspaceFileName;
    wxFileName projectFileName(folders.Item(0), "");
    projectFileName.SetName(projectFileName.GetDirs().Last());
    projectFileName.SetExt("phprj");

    if(!PHPWorkspace::Get()->IsOpen()) {
        workspaceFileName = wxFileName(folders.Item(0), "");
        workspaceFileName.SetName(workspaceFileName.GetDirs().Last());
        workspaceFileName.SetExt("workspace");

        if(!workspaceFile.IsEmpty()) {
            workspaceFileName = wxFileName(workspaceFile);
        }

        if(!workspaceFileName.IsDirWritable()) {
            wxString message;
            message << _("Failed to create workspace '") << workspaceFileName.GetFullPath() << "'\n"
                    << _("Permission denied.");
            ::wxMessageBox(message, "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
            return;
        }
        // Create an empty workspace
        if(!PHPWorkspace::Get()->Open(workspaceFileName.GetFullPath(), this, true)) {
            wxString message;
            message << _("Failed to open workspace '") << workspaceFileName.GetFullPath() << "'\n" << _("File exists");
            ::wxMessageBox(message, "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
            return;
        }

        // // We just created and opened a new workspace, add it to the "Recently used"
        // m_mgr->AddWorkspaceToRecentlyUsedList(workspaceFileName);
        LoadWorkspaceView();

        // Ensure that the view is visible
        m_mgr->GetWorkspaceView()->SelectPage(PHPStrings::PHP_WORKSPACE_VIEW_LABEL);

        // If we loaded an already existing workspace, we are done here
        if(!workspaceFile.IsEmpty()) return;

    } else {
        if(!workspaceFile.IsEmpty()) {
            // its the same workspace - do nothing
            if(PHPWorkspace::Get()->GetFilename().GetFullPath() == workspaceFile) return;
            // Different workspaces, prompt the user to close its workspace before continuing
            ::wxMessageBox(
                _("The folder already contains a workspace file\nPlease close the current workspace before continuing"),
                "CodeLite", wxOK | wxICON_WARNING | wxCENTER);
            return;
        }
        workspaceFileName = PHPWorkspace::Get()->GetFilename();
    }

    // Make sure that this folder is not part of any of the existing projects
    if(!PHPWorkspace::Get()->CanCreateProjectAtPath(projectFileName, true)) {
        return;
    }

    // We can safely create the project
    PHPConfigurationData conf;
    const wxString& phpExe = conf.Load().GetPhpExe();

    PHPProject::CreateData cd;
    cd.importFilesUnderPath = true;
    cd.name = projectFileName.GetName();
    cd.path = projectFileName.GetPath();
    cd.phpExe = phpExe;
    cd.projectType = PHPProjectSettingsData::kRunAsCLI;
    CreateNewProject(cd);
}

void PHPWorkspaceView::OnMenu(wxTreeEvent& event)
{
    wxBitmap bmpFiF = m_mgr->GetStdIcons()->LoadBitmap("find_in_files");
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        // Ensure that the item is selected
        m_treeCtrlView->SelectItem(event.GetItem());
        ItemData* data = DoGetItemData(item);
        if(data) {
            wxMenuItem* menuItem = NULL;
            switch(data->GetKind()) {
            case ItemData::Kind_File: {
                wxMenu menu;
                menu.Append(XRCID("php_open_file"), _("Open"));
                menu.Append(XRCID("rename_php_file"), _("Rename"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_remove_file"), _("Delete"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_open_folder_in_explorer"), _("Open Containing Folder"));
                menu.Append(XRCID("php_open_shell"), _("Open Shell"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_open_with_default_app"), _("Open with Default Application"));

                // Allow other plugins to hook their context menu into our
                // file context menu
                clContextMenuEvent menuEvent(wxEVT_CONTEXT_MENU_FILE);
                menuEvent.SetMenu(&menu);
                wxArrayString files;
                DoGetSelectedFiles(files);
                menuEvent.SetStrings(files);
                EventNotifier::Get()->ProcessEvent(menuEvent);

                m_treeCtrlView->PopupMenu(&menu);

            } break;
            case ItemData::Kind_Workspace: {
                wxMenu menu;
                menu.Append(XRCID("php_reload_workspace"), _("Reload workspace"));
                menu.Append(XRCID("php_close_workspace"), _("Close workspace"));
                menu.AppendSeparator();
                menu.Append(XRCID("new_project"), _("Create a new project..."));
                menu.Append(XRCID("add_existing_project"), _("Add an existing project..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_reparse_workspace"), _("Parse workspace"));
                menu.AppendSeparator();
                menu.Append(XRCID("rename_php_workspace"), _("Rename"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_open_folder_in_explorer"), _("Open Containing Folder"));
                menu.Append(XRCID("php_open_shell"), _("Open Shell"));
                menu.AppendSeparator();

                menuItem =
                    new wxMenuItem(NULL, XRCID("php_synch_with_filesystem"), _("Sync workspace with file system..."));
                menuItem->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("debugger_restart"));
                menu.Append(menuItem);
                menu.AppendSeparator();
                menuItem = new wxMenuItem(NULL, XRCID("php_folder_find_in_files"), _("Find In Files"));
                menuItem->SetBitmap(bmpFiF);
                menu.Append(menuItem);
                m_treeCtrlView->PopupMenu(&menu);
            } break;
            case ItemData::Kind_Project: {
                wxMenu menu;
                menu.Append(XRCID("php_set_project_active"), _("Set as active project"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_delete_project"), _("Remove project"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_new_class"), _("New Class..."));
                menu.Append(XRCID("php_new_folder"), _("New Folder..."));
                menu.Append(XRCID("php_new_file"), _("New File..."));
                menu.AppendSeparator();

                menuItem = new wxMenuItem(NULL, XRCID("php_folder_find_in_files"), _("Find In Files"));
                menuItem->SetBitmap(bmpFiF);
                menu.Append(menuItem);
                menu.AppendSeparator();
                menu.Append(XRCID("php_open_folder_in_explorer"), _("Open Containing Folder"));
                menu.Append(XRCID("php_open_shell"), _("Open Shell"));
                menu.AppendSeparator();

                menuItem = new wxMenuItem(NULL, XRCID("php_sync_project_with_filesystem"),
                                          _("Sync project with file system..."));
                menuItem->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("debugger_restart"));
                menu.Append(menuItem);
                menu.AppendSeparator();
                menu.Append(XRCID("php_run_project"), _("Run project..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_project_settings"), _("Project settings..."));

                // Let other plugins add content here
                clContextMenuEvent folderMenuEvent(wxEVT_CONTEXT_MENU_FOLDER);
                folderMenuEvent.SetMenu(&menu);
                folderMenuEvent.SetPath(wxFileName(data->GetFile()).GetPath());
                EventNotifier::Get()->ProcessEvent(folderMenuEvent);

                m_treeCtrlView->PopupMenu(&menu);
            } break;
            case ItemData::Kind_Folder: {
                wxMenu menu;
                menu.Append(XRCID("php_new_class"), _("New Class..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_new_folder"), _("New Folder..."));
                menu.Append(XRCID("php_new_file"), _("New File..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_rename_folder"), _("Rename..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_remove_file"), _("Delete"));
                menu.AppendSeparator();
                menuItem = new wxMenuItem(NULL, XRCID("php_sync_folder_with_filesystem"),
                                          _("Sync folder with file system..."));
                menuItem->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("debugger_restart"));
                menu.Append(menuItem);
                menu.AppendSeparator();
                menu.Append(XRCID("php_open_folder_in_explorer"), _("Open Containing Folder"));
                menu.Append(XRCID("php_open_shell"), _("Open Shell"));
                menu.AppendSeparator();
                menuItem = new wxMenuItem(NULL, XRCID("php_folder_find_in_files"), _("Find In Files"));
                menuItem->SetBitmap(bmpFiF);
                menu.Append(menuItem);

                clContextMenuEvent folderMenuEvent(wxEVT_CONTEXT_MENU_FOLDER);
                folderMenuEvent.SetMenu(&menu);
                folderMenuEvent.SetPath(data->GetFolderPath());
                EventNotifier::Get()->ProcessEvent(folderMenuEvent);

                m_treeCtrlView->PopupMenu(&menu);
            } break;
            default:
                break;
            }
        }
    }
}

void PHPWorkspaceView::LoadWorkspaceView()
{
    m_itemsToSort.Clear();
    m_filesItems.clear();
    m_foldersItems.clear();

    wxString workspaceName;
    workspaceName << PHPWorkspace::Get()->GetFilename().GetName();

    wxWindowUpdateLocker locker(m_treeCtrlView);
    m_treeCtrlView->DeleteAllItems();

    if(m_scanInProgress) {
        m_treeCtrlView->AddRoot("Scanning for workspace files...");
        m_treeCtrlView->Enable(false);
        return;
    }

    // Add the root item
    BitmapLoader* bl = m_mgr->GetStdIcons();

    ItemData* data = new ItemData(ItemData::Kind_Workspace);
    data->SetFile(workspaceName);
    wxTreeItemId root =
        m_treeCtrlView->AddRoot(workspaceName, bl->GetMimeImageId(PHPWorkspace::Get()->GetFilename().GetFullName()),
                                bl->GetMimeImageId(PHPWorkspace::Get()->GetFilename().GetFullName()), data);
    const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();
    m_itemsToSort.PushBack(root, true);

    wxBusyCursor bc;
    wxBusyInfo info(_("Building workspace tree view"), FRAME);
    wxYieldIfNeeded();

    // add projects
    wxTreeItemId activeProjectId;
    wxStringSet_t files;
    PHPProject::Map_t::const_iterator iter_project = projects.begin();
    for(; iter_project != projects.end(); ++iter_project) {
        data = new ItemData(ItemData::Kind_Project);
        data->SetProjectName(iter_project->first);
        data->SetFolderPath(iter_project->second->GetFilename().GetPath());
        data->SetFile(iter_project->second->GetFilename().GetFullPath());
        data->SetActive(iter_project->second->IsActive());

        wxTreeItemId projectItemId = m_treeCtrlView->AppendItem(root, iter_project->second->GetName(),
                                                                bl->GetMimeImageId(FileExtManager::TypeProject),
                                                                bl->GetMimeImageId(FileExtManager::TypeProject), data);
        if(data->IsActive()) {
            m_treeCtrlView->SetItemBold(projectItemId, true);
        }

        // The project is also a folder for the project folder
        m_foldersItems.insert(std::make_pair(iter_project->second->GetFilename().GetPath(), projectItemId));
        m_itemsToSort.PushBack(projectItemId, true);
        DoBuildProjectNode(projectItemId, iter_project->second);
        if(data->IsActive()) {
            activeProjectId = projectItemId;
        }
    }

    if(m_treeCtrlView->HasChildren(root)) {
        m_treeCtrlView->Expand(root);
    }

    if(activeProjectId.IsOk() && m_treeCtrlView->ItemHasChildren(activeProjectId)) {
        m_treeCtrlView->Expand(activeProjectId);
    }
    DoSortItems();
    wxCommandEvent dummy;
    OnEditorChanged(dummy);
}

void PHPWorkspaceView::UnLoadWorkspaceView()
{
    m_treeCtrlView->DeleteAllItems();
    m_scanInProgress = true;
}

void PHPWorkspaceView::CreateNewProject(PHPProject::CreateData cd)
{
    PHPWorkspace::Get()->CreateProject(cd);
    // Update the UI
    PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
}

void PHPWorkspaceView::OnCloseWorkspace(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_scanInProgress = true;
    m_treeCtrlView->DeleteAllItems();
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    event.SetEventObject(wxTheApp->GetTopWindow());
    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
}

void PHPWorkspaceView::OnReloadWorkspace(wxCommandEvent& e)
{
    // sync the file view and rebuild the tree
    PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
}

void PHPWorkspaceView::OnDeleteProject(wxCommandEvent& e)
{
    // Prompt user
    wxTreeItemId selection = m_treeCtrlView->GetFocusedItem();
    if(selection.IsOk()) {
        ItemData* itemData = DoGetItemData(selection);
        if(itemData && itemData->IsProject()) {
            if(wxMessageBox(wxString() << _("Are you sure you want to remove project '") << itemData->GetProjectName()
                                       << "'?",
                            _("CodeLite"), wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT, wxTheApp->GetTopWindow()) == wxYES) {
                PHPWorkspace::Get()->DeleteProject(itemData->GetProjectName());
                m_treeCtrlView->Delete(selection);

                // Highlight the active project (incase we removed the currently active project
                // from the view)
                DoSetProjectActive(PHPWorkspace::Get()->GetActiveProjectName());
            }
        }
    }
}

void PHPWorkspaceView::OnNewFolder(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxString name = wxGetTextFromUser(_("New Folder Name:"), _("New Folder"));
    if(name.IsEmpty()) return;

    wxString project = DoGetSelectedProject();
    if(project.IsEmpty()) return;

    wxTreeItemId parent = m_treeCtrlView->GetFocusedItem();
    CHECK_ITEM_RET(parent);

    ItemData* itemData = DoGetItemData(parent);
    CHECK_PTR_RET(itemData);

    if(!itemData->IsFolder() && !itemData->IsProject()) return;

    PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(proj);

    wxString base;
    if(itemData->IsFolder()) {
        base = itemData->GetFolderPath();
    } else {
        base = proj->GetFilename().GetPath();
    }

    wxFileName newfolder(base, "");
    newfolder.AppendDir(name);

    // Create the folder on the file system
    if(wxFileName::Mkdir(newfolder.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
        proj->FolderAdded(newfolder.GetPath());

        // Update the UI
        wxWindowUpdateLocker locker(m_treeCtrlView);

        // Add folder to the tree view
        int imgId = m_mgr->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
        ItemData* itemData = new ItemData(ItemData::Kind_Folder);
        itemData->SetFolderName(name);
        itemData->SetFolderPath(newfolder.GetPath());
        itemData->SetProjectName(proj->GetName());

        m_itemsToSort.Clear();
        m_itemsToSort.PushBack(parent, true);
        wxTreeItemId folderItem = m_treeCtrlView->AppendItem(parent, name, imgId, imgId, itemData);
        m_itemsToSort.PushBack(folderItem, true);
        DoSortItems();
        // Expand the node
        if(!m_treeCtrlView->IsExpanded(parent)) {
            m_treeCtrlView->Expand(parent);
        }
    }
}

void PHPWorkspaceView::OnSetProjectActive(wxCommandEvent& e)
{
    wxString project = DoGetSelectedProject();
    if(project.IsEmpty()) return;

    DoSetProjectActive(project);
}

wxString PHPWorkspaceView::DoGetSelectedProject()
{
    wxTreeItemId item = m_treeCtrlView->GetFocusedItem();
    if(!item.IsOk()) {
        return wxEmptyString;
    }

    ItemData* id = DoGetItemData(item);
    if(!id) return wxEmptyString;
    return id->GetProjectName();
}

ItemData* PHPWorkspaceView::DoGetItemData(const wxTreeItemId& item)
{
    if(item.IsOk() == false) return NULL;

    wxTreeItemData* data = m_treeCtrlView->GetItemData(item);
    if(!data) {
        return NULL;
    }
    return dynamic_cast<ItemData*>(data);
}

const ItemData* PHPWorkspaceView::DoGetItemData(const wxTreeItemId& item) const
{
    if(item.IsOk() == false) return NULL;

    wxTreeItemData* data = m_treeCtrlView->GetItemData(item);
    if(!data) {
        return NULL;
    }
    return dynamic_cast<ItemData*>(data);
}

void PHPWorkspaceView::OnNewFile(wxCommandEvent& e)
{
    wxTreeItemId folderId = DoGetSingleSelection();
    ItemData* data = DoGetItemData(folderId);
    if(data->IsFolder() || data->IsProject()) {
        wxString filename =
            ::clGetTextFromUser(_("New File"), _("Set the file name:"), "Untitled.php", wxStrlen("Untitled"));
        if(filename.IsEmpty()) return;
        wxFileName fn;
        if(data->IsFolder()) {
            fn = wxFileName(data->GetFolderPath(), filename);
        } else {
            PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(data->GetProjectName());
            CHECK_PTR_RET(proj);

            fn = wxFileName(proj->GetFilename().GetPath(), filename);
        }
        wxTreeItemId fileItem = DoCreateFile(folderId, fn.GetFullPath());
        if(fileItem.IsOk()) {
            if(!m_treeCtrlView->IsExpanded(folderId)) {
                m_treeCtrlView->Expand(folderId);
            }
            CallAfter(&PHPWorkspaceView::DoOpenFile, fileItem);
        }
    }
}

bool PHPWorkspaceView::IsFolderItem(const wxTreeItemId& item)
{
    ItemData* data = DoGetItemData(item);
    if(!data) return false;

    return data->IsFolder();
}

void PHPWorkspaceView::OnDeleteFolder(wxCommandEvent& e)
{
    wxTreeItemId folderItem = DoGetSingleSelection();
    if(!IsFolderItem(folderItem)) return;

    ItemData* data = DoGetItemData(folderItem);
    wxString folder = data->GetFolderPath();
    wxString project = DoGetSelectedProject();
    if(folder.IsEmpty() || project.IsEmpty()) return;

    wxString msg = wxString() << _("Are you sure you want to delete folder '") << folder << _("' and its content?");
    if(wxMessageBox(msg, wxT("CodeLite"), wxYES_NO | wxCANCEL | wxICON_WARNING | wxCENTER) != wxYES) return;

    // Delete the folder from the file system
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(pProject);

    if(wxFileName::Rmdir(folder, wxPATH_RMDIR_RECURSIVE)) {
        pProject->FolderDeleted(folder, true);
        pProject->Save();
        // Update the UI
        m_treeCtrlView->Delete(folderItem);
    }
    PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
}

void PHPWorkspaceView::OnRetagWorkspace(wxCommandEvent& e)
{
    // notify codelite to close the currently opened workspace
    wxCommandEvent retagEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
    retagEvent.SetEventObject(FRAME);
    FRAME->GetEventHandler()->ProcessEvent(retagEvent);
}

int PHPWorkspaceView::DoGetItemImgIdx(const wxString& filename)
{
    int idx = m_mgr->GetStdIcons()->GetMimeImageId(filename);
    if(idx == wxNOT_FOUND) {
        idx = m_mgr->GetStdIcons()->GetMimeImageId(FileExtManager::TypeText);
    }
    return idx;
}

void PHPWorkspaceView::OnOpenFile(wxCommandEvent& e)
{
    e.Skip();
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    if(items.IsEmpty()) return;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        const wxTreeItemId& item = items.Item(i);
        ItemData* itemData = DoGetItemData(item);
        if(itemData->IsFile()) {
            DoOpenFile(item);
        }
    }
}

void PHPWorkspaceView::DoOpenFile(const wxTreeItemId& item)
{
    ItemData* data = DoGetItemData(item);
    if(data && data->IsFile()) {
        m_mgr->OpenFile(data->GetFile());
        if(m_mgr->GetActiveEditor() && m_mgr->GetActiveEditor()->GetFileName().GetFullPath() == data->GetFile()) {
            m_mgr->GetActiveEditor()->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
        }
    }
}

void PHPWorkspaceView::OnRemoveFile(wxCommandEvent& e) { DoDeleteSelectedFileItem(); }

void PHPWorkspaceView::OnRenameFile(wxCommandEvent& e)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    if(items.IsEmpty()) return;

    wxTreeItemId item = items.Item(0);
    CHECK_ITEM_RET(item);

    ItemData* data = DoGetItemData(item);
    CHECK_PTR_RET(data);
    CHECK_ID_FILE(data);

    wxFileName oldFileName = data->GetFile();

    wxString new_name = ::clGetTextFromUser(_("New file name:"), _("Rename file"), oldFileName.GetFullName());
    if(new_name.IsEmpty()) return;
    if(new_name == oldFileName.GetFullName()) return;

    // Get the project that owns this file
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProjectForFile(oldFileName);
    CHECK_PTR_RET(pProject);

    // Check to see if we got a file with the old path opened
    bool reopenFile = false;
    IEditor* editor = m_mgr->FindEditor(oldFileName.GetFullPath());
    if(editor) {
        m_mgr->ClosePage(editor->GetFileName().GetFullName());
        reopenFile = true;
        editor = NULL;
    }

    wxFileName newFileName = oldFileName;
    newFileName.SetFullName(new_name);
    // Rename the file on the file system
    if(::wxRenameFile(oldFileName.GetFullPath(), newFileName.GetFullPath())) {
        pProject->FileRenamed(oldFileName.GetFullPath(), newFileName.GetFullPath(), true);
        pProject->Save();
        m_treeCtrlView->SetItemText(item, new_name);

        // Update the item data
        data->SetFile(newFileName.GetFullPath());

        // Open the file if it was opened earlier
        // old_file_name now contains the new full path to the new file
        if(reopenFile) {
            m_mgr->OpenFile(newFileName.GetFullPath());
        }
    }

    PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
}

void PHPWorkspaceView::DoDeleteSelectedFileItem()
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    if(items.IsEmpty()) return;

    wxString msg;
    msg = wxString::Format(_("This operation will delete the selected items.\nContinue?"), (int)items.GetCount());
    wxStandardID res = ::PromptForYesNoDialogWithCheckbox(msg, "PHPDeleteFiles", _("Yes"), _("No"),
                                                          _("Remember my answer and don't ask me again"),
                                                          wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT);
    if(res != wxID_YES) return;

    wxArrayString removedFiles, removedFolders;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        ItemData* itemData = static_cast<ItemData*>(m_treeCtrlView->GetItemData(items.Item(i)));
        if(!itemData || (!itemData->IsFile() && !itemData->IsFolder())) continue;

        // Folder or file
        if(itemData->IsFolder()) {
            // Prompt for folder message
            msg = wxString::Format(
                _("'%s' is a folder.\nThis operation will delete the folder and its content.\nContinue?"),
                itemData->GetFolderPath());
            res = ::PromptForYesNoDialogWithCheckbox(msg, "PHPDeleteFolder", _("Yes"), _("No"),
                                                     _("Remember my answer and don't ask me again"),
                                                     wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT);
            if(res != wxID_YES) continue; // Don't delete the folder
            removedFolders.Add(itemData->GetFolderPath());
            wxFileName::Rmdir(itemData->GetFolderPath(), wxPATH_RMDIR_RECURSIVE);

        } else {
            clRemoveFile(itemData->GetFile());
            removedFiles.Add(itemData->GetFile());
        }
    }

    // Sync the workspace with the file system
    PHPWorkspace::Get()->SyncWithFileSystemAsync(this);

    // Notify about the file/folder deletion
    {
        clFileSystemEvent evt(wxEVT_FILE_DELETED);
        evt.SetPaths(removedFiles);
        evt.SetEventObject(this);
        EventNotifier::Get()->AddPendingEvent(evt);
    }

    {
        clFileSystemEvent evt(wxEVT_FOLDER_DELETED);
        evt.SetPaths(removedFolders);
        evt.SetEventObject(this);
        EventNotifier::Get()->AddPendingEvent(evt);
    }
}

void PHPWorkspaceView::OnRunProject(wxCommandEvent& e)
{
    // Test which file we want to debug
    PHPDebugStartDlg debugDlg(EventNotifier::Get()->TopFrame(), PHPWorkspace::Get()->GetActiveProject(), m_mgr);
    debugDlg.SetLabel("Run Project");
    if(debugDlg.ShowModal() != wxID_OK) {
        return;
    }

    PHPWorkspace::Get()->RunProject(false, debugDlg.GetPath(), DoGetSelectedProject());
}

wxBitmap PHPWorkspaceView::DoGetBitmapForExt(const wxString& ext) const
{
    wxString filename;
    filename << "dummy"
             << "." << ext;

    FileExtManager::FileType type = FileExtManager::GetType(filename);
    if(type == FileExtManager::TypeOther) {
        type = FileExtManager::TypeText;
    }
    return m_bitmaps.find(type)->second;
}

void PHPWorkspaceView::OnActiveProjectSettings(wxCommandEvent& event)
{
    if(!PHPWorkspace::Get()->GetActiveProject()) {
        ::wxMessageBox(_("No active project is set !?\nPlease set an active project and try again"), "CodeLite",
                       wxICON_ERROR | wxOK | wxCENTER, FRAME);
        return;
    }
    PHPProjectSettingsDlg settingsDlg(FRAME, PHPWorkspace::Get()->GetActiveProjectName());
    if(settingsDlg.ShowModal() == wxID_OK && settingsDlg.IsResyncNeeded()) {
        // Re-sync the project with the file system
        PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
    }
}

void PHPWorkspaceView::OnActiveProjectSettingsUI(wxUpdateUIEvent& event)
{
    event.Enable(PHPWorkspace::Get()->IsOpen());
}

void PHPWorkspaceView::OnProjectSettings(wxCommandEvent& event)
{
    wxString selectedProject = DoGetSelectedProject();
    PHPProjectSettingsDlg settingsDlg(FRAME, selectedProject);
    if(settingsDlg.ShowModal() == wxID_OK && settingsDlg.IsResyncNeeded()) {
        // Re-sync the project with the file system
        PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
    }
}

void PHPWorkspaceView::OnRunActiveProject(clExecuteEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        CL_DEBUG("Running active project...");
        CHECK_COND_RET(PHPWorkspace::Get()->GetActiveProject());
        // Test which file we want to debug
        PHPDebugStartDlg dlg(EventNotifier::Get()->TopFrame(), PHPWorkspace::Get()->GetActiveProject(), m_mgr);
        dlg.SetLabel("Run Project");
        if(dlg.ShowModal() != wxID_OK) {
            return;
        }
        PHPWorkspace::Get()->RunProject(false, dlg.GetPath());

    } else {
        // Must call skip !
        e.Skip();
    }
}

void PHPWorkspaceView::OnIsProgramRunning(clExecuteEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        e.SetAnswer(PHPWorkspace::Get()->IsProjectRunning());

    } else {
        // Must call skip !
        e.Skip();
    }
}

void PHPWorkspaceView::OnStopExecutedProgram(clExecuteEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen() && PHPWorkspace::Get()->IsProjectRunning()) {
        PHPWorkspace::Get()->StopExecutedProgram();

    } else {
        // Must call skip !
        e.Skip();
    }
}

void PHPWorkspaceView::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        DoExpandToActiveEditor();
    }
}

void PHPWorkspaceView::OnFileRenamed(PHPEvent& e) { e.Skip(); }

void PHPWorkspaceView::OnMakeIndexPHP(wxCommandEvent& e)
{
    e.Skip();
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    if(items.GetCount() != 1) return;

    ItemData* itemData = DoGetItemData(items.Item(0));
    CHECK_PTR_RET(itemData);
    CHECK_ID_FILE(itemData);

    wxString projectName = itemData->GetProjectName();
    if(projectName.IsEmpty()) return;
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(projectName);
    CHECK_PTR_RET(pProject);

    PHPProjectSettingsData& settings = pProject->GetSettings();
    settings.SetIndexFile(itemData->GetFile());
    pProject->Save();
}

void PHPWorkspaceView::OnNewClass(wxCommandEvent& e)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);

    if(items.GetCount() != 1) return;

    wxTreeItemId folderId = items.Item(0);

    ItemData* data = DoGetItemData(folderId);
    if(!data->IsFolder() && !data->IsProject()) {
        return;
    }

    PHPProject::Ptr_t pProject;
    if(data->IsFolder()) {
        pProject = PHPWorkspace::Get()->GetProject(data->GetProjectName());
    } else {
        pProject = PHPWorkspace::Get()->GetProject(data->GetProjectName());
    }
    CHECK_PTR_RET(pProject);

    NewPHPClass dlg(wxTheApp->GetTopWindow(), data->GetFolderPath());
    if(dlg.ShowModal() == wxID_OK) {
        PHPClassDetails pcd = dlg.GetDetails();
        wxWindowUpdateLocker locker(m_treeCtrlView);
        m_itemsToSort.Clear();

        wxString fileContent;
        wxString eolString = EditorConfigST::Get()->GetOptions()->GetEOLAsString();
        fileContent << "<?php" << eolString << eolString << pcd.ToString(eolString, "    ");

        // Beautify the file
        clSourceFormatEvent event(wxEVT_FORMAT_STRING);
        event.SetInputString(fileContent);
        // Even if we don't parse the file directly but using a buffer
        // we still need to pass the file name to the formatter will know
        // which formatter tool to use (PHP/JS/C++ etc)
        event.SetFileName(pcd.GetFilepath().GetFullPath());
        EventNotifier::Get()->ProcessEvent(event);
        if(!event.GetFormattedString().IsEmpty()) {
            fileContent = event.GetFormattedString();
        }

        wxTreeItemId fileItem = DoCreateFile(folderId, pcd.GetFilepath().GetFullPath(), fileContent);
        DoSortItems();

        // Set the focus the new class
        if(fileItem.IsOk()) {
            if(!m_treeCtrlView->IsExpanded(folderId)) {
                m_treeCtrlView->Expand(folderId);
            }
            CallAfter(&PHPWorkspaceView::DoOpenFile, fileItem);
        }

        // Trigger parsing
        PHPWorkspace::Get()->ParseWorkspace(false);
    }
}

void PHPWorkspaceView::OnWorkspaceOpenUI(wxUpdateUIEvent& event) { event.Enable(PHPWorkspace::Get()->IsOpen()); }

void PHPWorkspaceView::OnRenameWorkspace(wxCommandEvent& e)
{
    wxString new_name = ::wxGetTextFromUser(_("New workspace name:"), _("Rename workspace"));
    if(!new_name.IsEmpty()) {
        PHPWorkspace::Get()->Rename(new_name);
    }
}

#if USE_SFTP
void PHPWorkspaceView::OnSetupRemoteUpload(wxAuiToolBarEvent& event)
{
    if(!event.IsDropDownClicked()) {
        CallAfter(&PHPWorkspaceView::DoOpenSSHAccountManager);

    } else {
        SSHWorkspaceSettings settings;
        settings.Load();

        wxMenu menu;
        if(!settings.IsRemoteUploadSet()) {
            // We never setup remote upload for this workspace
            menu.AppendCheckItem(ID_TOGGLE_AUTOMATIC_UPLOAD, _("Enable automatic upload"));
            menu.Enable(ID_TOGGLE_AUTOMATIC_UPLOAD, false);
            menu.Check(ID_TOGGLE_AUTOMATIC_UPLOAD, false);

        } else {
            menu.AppendCheckItem(ID_TOGGLE_AUTOMATIC_UPLOAD, _("Enable automatic upload"));
            menu.Check(ID_TOGGLE_AUTOMATIC_UPLOAD, settings.IsRemoteUploadEnabled());
            menu.Connect(ID_TOGGLE_AUTOMATIC_UPLOAD, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(PHPWorkspaceView::OnToggleAutoUpload), NULL, this);
        }

        wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
        if(auibar) {
            clAuiToolStickness ts(auibar, event.GetToolId());
            wxRect rect = auibar->GetToolRect(event.GetId());
            wxPoint pt = auibar->ClientToScreen(rect.GetBottomLeft());
            pt = ScreenToClient(pt);
            PopupMenu(&menu, pt);
        }
    }
}
#endif

void PHPWorkspaceView::OnToggleAutoUpload(wxCommandEvent& e)
{
    SSHWorkspaceSettings settings;
    settings.Load();
    settings.EnableRemoteUpload(e.IsChecked());
    settings.Save();
}

#if USE_SFTP
void PHPWorkspaceView::DoOpenSSHAccountManager()
{
    SSHWorkspaceSettings settings;
    settings.Load();

    SFTPBrowserDlg dlg(EventNotifier::Get()->TopFrame(),
                       _("Select the remote folder corrseponding to the current workspace file"), "",
                       clSFTP::SFTP_BROWSE_FOLDERS); // Browse for folders only
    dlg.Initialize(settings.GetAccount(), settings.GetRemoteFolder());

    if(dlg.ShowModal() == wxID_OK) {
        settings.SetAccount(dlg.GetAccount());
        settings.SetRemoteFolder(dlg.GetPath());
        settings.Save();
    }
}
#endif

void PHPWorkspaceView::ReloadWorkspace(bool saveBeforeReload)
{
    wxFileName fnWorkspace = PHPWorkspace::Get()->GetFilename();
    PHPWorkspace::Get()->Close(saveBeforeReload, true);
    PHPWorkspace::Get()->Open(fnWorkspace.GetFullPath(), this);
    UnLoadWorkspaceView();
    LoadWorkspaceView();
}

void PHPWorkspaceView::ReportParseThreadProgress(size_t curIndex, size_t total)
{
    if(!m_gaugeParseProgress->IsShown()) {
        m_gaugeParseProgress->SetValue(0);
        m_gaugeParseProgress->Show();
        GetSizer()->Layout();
    }

    if(total) {
        int precent = (curIndex * 100) / total;
        m_gaugeParseProgress->SetValue(precent);
    }
}

void PHPWorkspaceView::ReportParseThreadDone()
{
    m_gaugeParseProgress->SetValue(0);
    if(m_gaugeParseProgress->IsShown()) {
        m_gaugeParseProgress->Hide();
        GetSizer()->Layout();
    }
}

wxTreeItemId PHPWorkspaceView::DoGetSingleSelection() { return m_treeCtrlView->GetFocusedItem(); }

void PHPWorkspaceView::OnItemActivated(wxTreeEvent& event)
{
    ItemData* itemData = DoGetItemData(event.GetItem());
    if(itemData && itemData->IsFile()) {
        DoOpenFile(event.GetItem());

    } else {
        event.Skip();
    }
}

void PHPWorkspaceView::OnPhpParserDone(clParseEvent& event)
{
    event.Skip();
    ReportParseThreadDone();
}

void PHPWorkspaceView::OnPhpParserProgress(clParseEvent& event)
{
    event.Skip();
    ReportParseThreadProgress(event.GetCurfileIndex(), event.GetTotalFiles());
}

void PHPWorkspaceView::OnPhpParserStarted(clParseEvent& event) { event.Skip(); }

void PHPWorkspaceView::DoSortItems()
{
    wxOrderedMap<wxTreeItemId, bool>::const_iterator iter = m_itemsToSort.begin();
    for(; iter != m_itemsToSort.end(); ++iter) {
        m_treeCtrlView->SortItem(iter->first);
    }
    m_itemsToSort.Clear();
}

void PHPWorkspaceView::OnSyncWorkspaceWithFileSystem(wxCommandEvent& e)
{
    PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
}

void PHPWorkspaceView::DoBuildProjectNode(const wxTreeItemId& projectItem, PHPProject::Ptr_t project)
{
    const wxArrayString& files = project->GetFiles(NULL);
    for(size_t i = 0; i < files.GetCount(); ++i) {
        const wxString& filename = files.Item(i);
        wxFileName file(filename);
        wxTreeItemId folder = DoAddFolder(project->GetName(), file.GetPath());
        if(folder.IsOk()) {
            // Don't add our folder marker files which are fake items we added
            // so the workspace will also include empty folders
            if(file.GetFullName() != FOLDER_MARKER) {
                // Add the file
                ItemData* itemData = new ItemData(ItemData::Kind_File);
                itemData->SetFile(filename);
                itemData->SetProjectName(project->GetName());
                wxTreeItemId fileItem = m_treeCtrlView->AppendItem(
                    folder, file.GetFullName(), DoGetItemImgIdx(filename), DoGetItemImgIdx(filename), itemData);
                // Cache the result
                m_filesItems.insert(std::make_pair(file.GetFullPath(), fileItem));
            }
        }
    }
}

void PHPWorkspaceView::DoGetSelectedItems(wxArrayTreeItemIds& items) { m_treeCtrlView->GetSelections(items); }

wxTreeItemId PHPWorkspaceView::DoAddFolder(const wxString& project, const wxString& path)
{
    // Check the cache
    if(m_foldersItems.count(path)) {
        return m_foldersItems.find(path)->second;
    }

    wxTreeItemId projectItem = DoGetProject(project);
    if(!projectItem.IsOk()) return wxTreeItemId();

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    if(!pProject) return wxTreeItemId();

    int imgId = m_mgr->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    wxString curpath;
    wxTreeItemId parent = projectItem;
    m_itemsToSort.PushBack(parent, true);
    wxFileName fnFolder(path, "dummy.txt");
    fnFolder.MakeRelativeTo(pProject->GetFilename().GetPath());
    if(fnFolder.GetDirCount() == 0) {
        // the folder is the same as the project, return the project item
        return projectItem;
    }

    const wxArrayString& parts = fnFolder.GetDirs();
    wxFileName curdir = pProject->GetFilename();
    for(size_t i = 0; i < parts.GetCount(); ++i) {
        curdir.AppendDir(parts.Item(i));
        if(m_foldersItems.count(curdir.GetPath()) == 0) {
            ItemData* itemData = new ItemData(ItemData::Kind_Folder);
            itemData->SetFolderPath(curdir.GetPath());
            itemData->SetProjectName(project);
            itemData->SetFolderName(parts.Item(i));
            parent = m_treeCtrlView->AppendItem(parent, parts.Item(i), imgId, imgId, itemData);
            m_foldersItems.insert(std::make_pair(curdir.GetPath(), parent));
            m_itemsToSort.PushBack(parent, true);
        } else {
            parent = m_foldersItems.find(curdir.GetPath())->second;
        }
    }
    return parent;
}

wxTreeItemId PHPWorkspaceView::DoGetProject(const wxString& project)
{
    wxTreeItemId root = m_treeCtrlView->GetRootItem();
    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_treeCtrlView->GetFirstChild(root, cookie);
    while(item.IsOk()) {
        if(m_treeCtrlView->GetItemText(item) == project) {
            return item;
        }
        item = m_treeCtrlView->GetNextChild(root, cookie);
    }
    return wxTreeItemId();
}

void PHPWorkspaceView::OnWorkspaceRenamed(PHPEvent& e)
{
    e.Skip();
    wxFileName newName(e.GetFileName());
    m_treeCtrlView->SetItemText(m_treeCtrlView->GetRootItem(), newName.GetName());
}

wxTreeItemId PHPWorkspaceView::DoCreateFile(const wxTreeItemId& parent, const wxString& fullpath,
                                            const wxString& content)
{
    PHPProject::Ptr_t proj = DoGetProjectForItem(parent);
    if(!proj) return wxTreeItemId();

    m_itemsToSort.Clear();
    wxFileName file(fullpath);
    // Write the file content
    if(FileUtils::WriteFileContent(file, content)) {
        // Update the UI
        // Add the file
        ItemData* itemData = new ItemData(ItemData::Kind_File);
        itemData->SetFile(file.GetFullPath());
        PHPProject::Ptr_t pProj = PHPWorkspace::Get()->GetProjectForFile(file);
        if(pProj) {
            itemData->SetProjectName(pProj->GetName());
        }
        wxTreeItemId fileItem =
            m_treeCtrlView->AppendItem(parent, file.GetFullName(), DoGetItemImgIdx(file.GetFullName()),
                                       DoGetItemImgIdx(file.GetFullName()), itemData);

        // Cache the result
        m_filesItems.insert(std::make_pair(file.GetFullPath(), fileItem));
        m_itemsToSort.PushBack(parent, true);
        proj->FileAdded(file.GetFullPath(), true);
        DoSortItems();
        return fileItem;
    }
    return wxTreeItemId();
}

PHPProject::Ptr_t PHPWorkspaceView::DoGetProjectForItem(const wxTreeItemId& item)
{
    if(m_treeCtrlView->GetRootItem() == item) return PHPProject::Ptr_t(NULL);

    wxTreeItemId cur = item;
    while(cur.IsOk()) {
        if(m_treeCtrlView->GetItemParent(cur) == m_treeCtrlView->GetRootItem()) {
            // we found the project level
            ItemData* itemData = DoGetItemData(cur);
            return PHPWorkspace::Get()->GetProject(itemData->GetProjectName());
        }
        cur = m_treeCtrlView->GetItemParent(cur);
    }
    return PHPProject::Ptr_t(NULL);
}

void PHPWorkspaceView::OnOpenInExplorer(wxCommandEvent& e)
{
    wxFileName file;
    wxTreeItemId item = m_treeCtrlView->GetFocusedItem();
    CHECK_ITEM_RET(item);

    ItemData* itemData = DoGetItemData(item);
    CHECK_PTR_RET(itemData);

    if(itemData->IsProject()) {
        file = itemData->GetFile();
    } else if(itemData->IsFolder()) {
        file = wxFileName(itemData->GetFolderPath(), "");
    } else if(itemData->IsFile()) {
        file = itemData->GetFile();
    } else if(itemData->IsWorkspace()) {
        file = PHPWorkspace::Get()->GetFilename();
    } else {
        return;
    }
    FileUtils::OpenFileExplorerAndSelect(file);
}

void PHPWorkspaceView::OnOpenShell(wxCommandEvent& e)
{
    wxFileName file;
    wxTreeItemId item = m_treeCtrlView->GetFocusedItem();
    CHECK_ITEM_RET(item);

    ItemData* itemData = DoGetItemData(item);
    CHECK_PTR_RET(itemData);

    if(itemData->IsProject()) {
        file = itemData->GetFile();
    } else if(itemData->IsFolder()) {
        file = wxFileName(itemData->GetFolderPath(), "");
    } else if(itemData->IsFile()) {
        file = itemData->GetFile();
    } else if(itemData->IsWorkspace()) {
        file = PHPWorkspace::Get()->GetFilename();
    } else {
        return;
    }
    FileUtils::OpenTerminal(file.GetPath());
}

void PHPWorkspaceView::DoGetSelectedFolders(wxArrayString& paths)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxTreeItemId item = items.Item(i);
        ItemData* itemData = DoGetItemData(item);
        if(itemData->IsFolder()) {
            paths.Add(itemData->GetFolderPath());

        } else if(itemData->IsProject()) {
            wxString projectPath = wxFileName(itemData->GetFile()).GetPath();
            paths.Add(projectPath);

        } else if(itemData->IsWorkspace()) {
            // If the workspace is included, use the project paths and nothing more
            paths.Clear();
            const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();
            PHPProject::Map_t::const_iterator iter = projects.begin();
            for(; iter != projects.end(); ++iter) {
                paths.Add(iter->second->GetFilename().GetPath());
            }
            break;
        }
    }
}

void PHPWorkspaceView::OnFindInFiles(wxCommandEvent& e)
{
    wxArrayString paths;
    DoGetSelectedFolders(paths);
    CHECK_COND_RET(!paths.IsEmpty());

    // Open the find in files dialg for the folder path
    m_mgr->OpenFindInFileForPaths(paths);
}

void PHPWorkspaceView::OnOpenWithDefaultApp(wxCommandEvent& e)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxTreeItemId item = items.Item(i);
        ItemData* itemData = DoGetItemData(item);
        if(itemData->IsFile()) {
            ::wxLaunchDefaultApplication(itemData->GetFile());
        }
    }
}
void PHPWorkspaceView::OnSetupRemoteUploadUI(wxUpdateUIEvent& event)
{
#if USE_SFTP
    event.Enable(PHPWorkspace::Get()->IsOpen());
#else
    event.Enable(false);
#endif
}

void PHPWorkspaceView::DoGetSelectedFiles(wxArrayString& files)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    if(items.IsEmpty()) return;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        const wxTreeItemId& item = items.Item(i);
        ItemData* itemData = DoGetItemData(item);
        if(itemData->IsFile()) {
            files.Add(itemData->GetFile());
        }
    }
}

void PHPWorkspaceView::OnCollapse(wxCommandEvent& event)
{
    // Collapse the projects
    wxWindowUpdateLocker locker(m_treeCtrlView);
    wxTreeItemId root = m_treeCtrlView->GetRootItem();
    DoCollapseItem(root);
    if(m_treeCtrlView->ItemHasChildren(root)) {
        m_treeCtrlView->Expand(root);
        m_treeCtrlView->Collapse(root);
    }
}

void PHPWorkspaceView::OnCollapseUI(wxUpdateUIEvent& event) { event.Enable(PHPWorkspace::Get()->IsOpen()); }
void PHPWorkspaceView::OnStartDebuggerListenerUI(wxUpdateUIEvent& event)
{
    event.Enable(!XDebugManager::Get().IsDebugSessionRunning());
}

void PHPWorkspaceView::DoCollapseItem(wxTreeItemId& item)
{
    if(m_treeCtrlView->ItemHasChildren(item)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeCtrlView->GetFirstChild(item, cookie);
        while(child.IsOk()) {
            DoCollapseItem(child);
            child = m_treeCtrlView->GetNextChild(item, cookie);
        }
        m_treeCtrlView->Collapse(item);
    }
}

void PHPWorkspaceView::OnAddExistingProject(wxCommandEvent& e)
{
    // Prompt user for project path
    const wxString ALL("CodeLite PHP Projects (*.phprj)|*.phprj");
    wxFileDialog dlg(this, _("Open Project"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                     wxDefaultPosition);
    if(dlg.ShowModal() == wxID_OK && !dlg.GetPath().IsEmpty()) {
        wxString projectToAdd = dlg.GetPath();
        wxString errmsg;
        if(!PHPWorkspace::Get()->AddProject(projectToAdd, errmsg)) {
            if(!errmsg.IsEmpty()) {
                ::wxMessageBox(errmsg, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
            }
            return;
        }
        LoadWorkspaceView();
    }
}

void PHPWorkspaceView::DoSetProjectActive(const wxString& projectName)
{
    PHPWorkspace::Get()->SetProjectActive(projectName);
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrlView->GetFirstChild(m_treeCtrlView->GetRootItem(), cookie);
    while(child.IsOk()) {
        ItemData* id = DoGetItemData(child);
        if(id && id->IsProject()) {
            id->SetActive(id->GetProjectName() == projectName);
            m_treeCtrlView->SetItemBold(child, id->IsActive());
        }
        child = m_treeCtrlView->GetNextChild(m_treeCtrlView->GetRootItem(), cookie);
    }
}

void PHPWorkspaceView::OnWorkspaceLoaded(PHPEvent& event)
{
    event.Skip();
    m_mgr->GetWorkspaceView()->SelectPage(PHPStrings::PHP_WORKSPACE_VIEW_LABEL); // Ensure that the PHP view is selected
}

void PHPWorkspaceView::OnRenameFolder(wxCommandEvent& e)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    if(items.IsEmpty()) return;

    wxTreeItemId item = items.Item(0);
    CHECK_ITEM_RET(item);

    ItemData* data = DoGetItemData(item);
    CHECK_PTR_RET(data);
    CHECK_ID_FOLDER(data);

    // Get the project that owns this file
    wxString project = DoGetSelectedProject();
    if(project.IsEmpty()) return;

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(pProject);

    wxString new_name = ::clGetTextFromUser(_("Rename folder"), _("Folder name:"), data->GetFolderName());
    if(new_name.IsEmpty()) return;
    if(new_name == data->GetFolderName()) return;

    wxFileName oldFolderPath(data->GetFolderPath(), "");
    wxFileName newFolderPath(data->GetFolderPath(), "");

    newFolderPath.RemoveLastDir();
    newFolderPath.AppendDir(new_name);

    // Rename the file on the file system
    if(::wxRename(oldFolderPath.GetPath(), newFolderPath.GetPath()) == 0) {
        pProject->SynchWithFileSystem();
        pProject->Save();
        ReloadWorkspace(true);
    }
}

void PHPWorkspaceView::OnFindInFilesShowing(clCommandEvent& e)
{
    e.Skip();

    if(!PHPWorkspace::Get()->IsOpen()) return;
    if(!IsShownOnScreen()) return;

    // Get list of selected folders
    wxArrayString paths;
    DoGetSelectedFolders(paths);
    CHECK_COND_RET(!paths.IsEmpty());

    // PHP workspace is opened and visible
    wxArrayString& outPaths = e.GetStrings();
    outPaths.insert(outPaths.end(), paths.begin(), paths.end());
}

void PHPWorkspaceView::OnWorkspaceSyncStart(clCommandEvent& event)
{
    m_scanInProgress = true;
    CallAfter(&PHPWorkspaceView::DoSetStatusBarText, "Scanning for PHP files...", wxNOT_FOUND);
    m_treeCtrlView->Enable(false);
}

void PHPWorkspaceView::OnWorkspaceSyncEnd(clCommandEvent& event)
{
    m_scanInProgress = false;
    CallAfter(&PHPWorkspaceView::DoSetStatusBarText, "Scanning for PHP files completed", 3);
    PHPWorkspace::Get()->ParseWorkspace(false);
    CallAfter(&PHPWorkspaceView::LoadWorkspaceView);
    m_treeCtrlView->Enable(true);
}

void PHPWorkspaceView::DoSetStatusBarText(const wxString& text, int timeOut)
{
    clGetManager()->GetStatusBar()->SetMessage(text, timeOut);
}

void PHPWorkspaceView::OnFileSaveAs(clFileSystemEvent& event)
{
    event.Skip();
    // File "Save as"
    if(PHPWorkspace::Get()->IsOpen()) {
        PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
    }
}

void PHPWorkspaceView::OnStartDebuggerListener(wxCommandEvent& e) { XDebugManager::Get().StartListener(); }

void PHPWorkspaceView::OnSyncProjectWithFileSystem(wxCommandEvent& e)
{
    wxString project = DoGetSelectedProject();
    if(project.IsEmpty()) return;

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(pProject);

    pProject->SyncWithFileSystemAsync(this);
    m_pendingSync.insert(pProject->GetName());
}

void PHPWorkspaceView::OnProjectSyncCompleted(clCommandEvent& event)
{
    wxString projectName = event.GetString();
    if(m_pendingSync.count(projectName) == 0) return;
    m_pendingSync.erase(projectName);

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(projectName);
    CHECK_PTR_RET(pProject);

    // Update the project files
    pProject->SetFiles(event.GetStrings());

    // Rebuild the project tree node
    wxTreeItemId item = DoGetProject(pProject->GetName());
    CHECK_ITEM_RET(item);

    // Use the UI to fetch the folders and files
    wxArrayString folders, files;
    DoGetFilesAndFolders(pProject->GetName(), folders, files);

    // Clear these items from the cache
    std::for_each(files.begin(), files.end(), [&](const wxString& s) {
        if(m_filesItems.count(s)) {
            m_filesItems.erase(s);
        }
    });

    std::for_each(folders.begin(), folders.end(), [&](const wxString& s) {
        if(m_foldersItems.count(s)) {
            m_foldersItems.erase(s);
        }
    });

    wxWindowUpdateLocker locker(m_treeCtrlView);
    // Now we can delete the subtree
    m_treeCtrlView->DeleteChildren(item);

    // And finally, rebuild the project node
    DoBuildProjectNode(item, pProject);

    DoSortItems();
    m_itemsToSort.Clear();

    DoExpandToActiveEditor();
}

void PHPWorkspaceView::DoGetFilesAndFolders(const wxString& projectName, wxArrayString& folders, wxArrayString& files)
{
    folders.Clear();
    files.Clear();
    wxTreeItemId item = DoGetProject(projectName);
    DoGetFilesAndFolders(item, folders, files);
}

void PHPWorkspaceView::DoGetFilesAndFolders(const wxTreeItemId& item, wxArrayString& folders, wxArrayString& files)
{
    if(m_treeCtrlView->HasChildren(item)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeCtrlView->GetFirstChild(item, cookie);
        while(child.IsOk()) {
            DoGetFilesAndFolders(child, folders, files);
            child = m_treeCtrlView->GetNextChild(item, cookie);
        }
    }

    ItemData* id = DoGetItemData(item);
    CHECK_PTR_RET(id);

    if(id->IsFile()) {
        files.Add(id->GetFile());
    } else if(id->IsFolder()) {
        folders.Add(id->GetFolderPath());
    }
}

void PHPWorkspaceView::DoExpandToActiveEditor()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(m_filesItems.count(editor->GetFileName().GetFullPath())) {
        const wxTreeItemId& item = m_filesItems.find(editor->GetFileName().GetFullPath())->second;
        CHECK_ITEM_RET(item);

        wxArrayTreeItemIds items;
        if(m_treeCtrlView->GetSelections(items)) {
            m_treeCtrlView->UnselectAll();
        }
        m_treeCtrlView->SelectItem(item);
        m_treeCtrlView->EnsureVisible(item);
    }
}

void PHPWorkspaceView::OnSyncFolderWithFileSystem(wxCommandEvent& e)
{
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);

    if(items.GetCount() > 1) return;

    wxTreeItemId item = items.Item(0);
    CHECK_ITEM_RET(item);

    PHPProject::Ptr_t pProject = DoGetProjectForItem(item);
    CHECK_PTR_RET(pProject);

    pProject->SyncWithFileSystemAsync(this);
    m_pendingSync.insert(pProject->GetName());
}

void PHPWorkspaceView::OnSyncWorkspaceWithFileSystemUI(wxUpdateUIEvent& e) { e.Enable(PHPWorkspace::Get()->IsOpen()); }

void PHPWorkspaceView::OnDragBegin(wxTreeEvent& event)
{
    event.Allow();
    m_draggedFiles.clear();
    wxArrayTreeItemIds items;
    DoGetSelectedItems(items);
    for(size_t i = 0; i < items.GetCount(); ++i) {
        ItemData* data = DoGetItemData(items.Item(i));
        if(!data || !data->IsFile()) {
            return;
        }
        m_draggedFiles.Add(data->GetFile());
    }
}

void PHPWorkspaceView::OnDragEnd(wxTreeEvent& event)
{
    if(m_draggedFiles.IsEmpty()) {
        return;
    }
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);

    ItemData* data = DoGetItemData(item);
    if(data) {
        bool syncNeeded = false;
        wxString targetFolder;
        if(data->IsFolder()) {
            targetFolder = data->GetFolderPath();
        } else if(data->IsProject()) {
            targetFolder = wxFileName(data->GetFile()).GetPath();
        }

        if(!targetFolder.IsEmpty()) {
            // Move the files from their current folder to the new folder
            for(size_t i = 0; i < m_draggedFiles.size(); ++i) {
                const wxString& srcfile = m_draggedFiles.Item(i);
                wxFileName newFile = srcfile;
                newFile.SetPath(targetFolder);
                if(newFile.FileExists()) {
                    wxStandardID answer = ::PromptForYesNoDialogWithCheckbox(
                        _("A file with this name already exists in the target folder\nOverwrite it?"), "dndphpview",
                        _("Yes"), _("No"), _("Do this for all files"),
                        wxCANCEL_DEFAULT | wxCENTRE | wxYES_NO | wxCANCEL | wxICON_WARNING, false);
                    if(answer != wxID_YES) {
                        continue;
                    }
                }
                if(::wxCopyFile(srcfile, newFile.GetFullPath(), true)) {
                    if(clRemoveFile(srcfile)) {
                        syncNeeded = true;
                        clFileSystemEvent rmEvent(wxEVT_FILE_DELETED);
                        rmEvent.GetPaths().Add(srcfile);
                        rmEvent.SetEventObject(this);
                        EventNotifier::Get()->AddPendingEvent(rmEvent);
                    }
                }
            }

            if(syncNeeded) {
                PHPWorkspace::Get()->SyncWithFileSystemAsync(this);
            }
        }
    }
    m_draggedFiles.Clear();
}
