#include "php_workspace_view.h"
#include "php_strings.h"
#include "php_project_settings_dlg.h"
#include <wx/busyinfo.h>
#include <globals.h>
#include <bitmap_loader.h>
#include <imanager.h>
#include "php_workspace.h"
#include "new_file_dlg.h"
#include <wx/imaglist.h>
#include <plugin.h>
#include "import_files_dlg.h"
#include <fileextmanager.h>
#include <event_notifier.h>
#include "NewPHPClass.h"
#include <editor_config.h>
#include <cl_command_event.h>
#include <ssh_account_info.h>
#include <SSHAccountManagerDlg.h>
#include <sftp_settings.h>
#include <SFTPBrowserDlg.h>
#include "ssh_workspace_settings.h"
#include "cl_aui_tool_stickness.h"
#include "parser_threadui_progress.h"
#include <macros.h>
#include "tree_item_data.h"
#include <bitmap_loader.h>

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
EVT_MENU(XRCID("php_add_folder"), PHPWorkspaceView::OnNewFolder)
EVT_MENU(XRCID("php_new_class"), PHPWorkspaceView::OnNewClass)
EVT_MENU(XRCID("php_new_file"), PHPWorkspaceView::OnNewFile)
EVT_MENU(XRCID("php_add_existing_file"), PHPWorkspaceView::OnAddFile)
EVT_MENU(XRCID("php_delete_folder"), PHPWorkspaceView::OnDeleteFolder)
EVT_MENU(XRCID("php_import_files"), PHPWorkspaceView::OnImportFiles)
EVT_MENU(XRCID("php_reparse_workspace"), PHPWorkspaceView::OnRetagWorkspace)
EVT_MENU(wxID_REMOVE, PHPWorkspaceView::OnRemoveFile)
EVT_MENU(wxID_OPEN, PHPWorkspaceView::OnOpenFile)
EVT_MENU(XRCID("rename_php_file"), PHPWorkspaceView::OnRenameFile)
EVT_MENU(XRCID("rename_php_workspace"), PHPWorkspaceView::OnRenameWorkspace)
EVT_MENU(XRCID("php_project_settings"), PHPWorkspaceView::OnProjectSettings)
EVT_MENU(XRCID("php_run_project"), PHPWorkspaceView::OnRunProject)
EVT_MENU(XRCID("make_index"), PHPWorkspaceView::OnMakeIndexPHP)
END_EVENT_TABLE()

PHPWorkspaceView::PHPWorkspaceView(wxWindow* parent, IManager* mgr)
    : PHPWorkspaceViewBase(parent)
    , m_mgr(mgr)
{
    // Initialise images map
    BitmapLoader* bmpLoader = m_mgr->GetStdIcons();
    m_bitmaps = bmpLoader->MakeStandardMimeMap();
    EventNotifier::Get()->Connect(
        wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, clExecuteEventHandler(PHPWorkspaceView::OnRunActiveProject), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_STOP_EXECUTED_PROGRAM, wxCommandEventHandler(PHPWorkspaceView::OnStopExecutedProgram), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_IS_PROGRAM_RUNNING, wxCommandEventHandler(PHPWorkspaceView::OnIsProgramRunning), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(PHPWorkspaceView::OnEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PHP_FILE_RENAMED, PHPEventHandler(PHPWorkspaceView::OnFileRenamed), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_PHP_WORKSPACE_RENAMED, PHPEventHandler(PHPWorkspaceView::OnWorkspaceRenamed), NULL, this);
    PHPParserThread::Instance()->SetProgress(new ParserThreadUIProgress(this));

    BitmapLoader* bl = m_mgr->GetStdIcons();
    wxImageList* imageList = bl->MakeStandardMimeImageList();
    m_treeCtrlView->AssignImageList(imageList);
}

PHPWorkspaceView::~PHPWorkspaceView()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, clExecuteEventHandler(PHPWorkspaceView::OnRunActiveProject), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_STOP_EXECUTED_PROGRAM, wxCommandEventHandler(PHPWorkspaceView::OnStopExecutedProgram), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_IS_PROGRAM_RUNNING, wxCommandEventHandler(PHPWorkspaceView::OnIsProgramRunning), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(PHPWorkspaceView::OnEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PHP_FILE_RENAMED, PHPEventHandler(PHPWorkspaceView::OnFileRenamed), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PHP_WORKSPACE_RENAMED, PHPEventHandler(PHPWorkspaceView::OnWorkspaceRenamed), NULL, this);
}

void PHPWorkspaceView::OnMenu(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        // Ensure that the item is selected
        m_treeCtrlView->SelectItem(event.GetItem());
        ItemData* data = DoGetItemData(item);
        if(data) {
            switch(data->GetKind()) {
            case ItemData::Kind_File: {
                wxMenu menu;
                menu.Append(wxID_OPEN, _("Open"));
                menu.Append(XRCID("rename_php_file"), _("Rename"));
                menu.AppendSeparator();
                menu.Append(wxID_REMOVE, _("Remove file"));
                menu.AppendSeparator();
                menu.Append(XRCID("make_index"), _("Set as Index file"));
                m_treeCtrlView->PopupMenu(&menu);
            } break;
            case ItemData::Kind_Workspace: {
                wxMenu menu;
                menu.Append(XRCID("php_reload_workspace"), _("Reload workspace"));
                menu.Append(XRCID("php_close_workspace"), _("Close workspace"));
                menu.AppendSeparator();
                menu.Append(XRCID("new_project"),
                            _("Add a new project...")); // This will trigger codelite's "New Project Wizard"
                menu.AppendSeparator();
                menu.Append(XRCID("php_reparse_workspace"), _("Retag workspace..."));
                menu.AppendSeparator();
                menu.Append(XRCID("rename_php_workspace"), _("Rename"));
                m_treeCtrlView->PopupMenu(&menu);
            } break;
            case ItemData::Kind_Project: {
                wxMenu menu;
                menu.Append(XRCID("php_set_project_active"), _("Set as active project"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_delete_project"), _("Delete project"));
                menu.AppendSeparator();
                menu.Append(XRCID("php_add_folder"), _("Add a folder..."));
                menu.Append(XRCID("php_import_files"), _("Import files from a directory..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_run_project"), _("Run project..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_project_settings"), _("Project settings..."));
                m_treeCtrlView->PopupMenu(&menu);
            } break;
            case ItemData::Kind_Folder: {
                wxMenu menu;
                menu.Append(XRCID("php_new_class"), _("New Class..."));
                menu.Append(XRCID("php_new_file"), _("Add a new file..."));
                menu.Append(XRCID("php_add_existing_file"), _("Add an existing file..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_add_folder"), _("Add a folder..."));
                menu.AppendSeparator();
                menu.Append(XRCID("php_delete_folder"), _("Delete this folder"));
                m_treeCtrlView->PopupMenu(&menu);
            } break;
            default:
                break;
            }
        }
    }
}

void PHPWorkspaceView::LoadWorkspace()
{
    wxString workspaceName;
    workspaceName << PHPWorkspace::Get()->GetFilename().GetName();

    wxWindowUpdateLocker locker(m_treeCtrlView);
    m_treeCtrlView->DeleteAllItems();

    // Add the root item
    BitmapLoader* bl = m_mgr->GetStdIcons();

    wxTreeItemId root = m_treeCtrlView->AddRoot(workspaceName,
                                                bl->GetMimeImageId(PHPWorkspace::Get()->GetFilename().GetFullName()),
                                                bl->GetMimeImageId(PHPWorkspace::Get()->GetFilename().GetFullName()),
                                                new ItemData(ItemData::Kind_Workspace));
    const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();

    // add projects
    wxStringSet_t files;
    PHPProject::Map_t::const_iterator iter_project = projects.begin();
    for(; iter_project != projects.end(); ++iter_project) {
        ItemData* data = new ItemData(ItemData::Kind_Project);
        data->SetProjectName(iter_project->first);
        data->SetActive(iter_project->second->IsActive());

        wxTreeItemId projectItemId = m_treeCtrlView->AppendItem(root,
                                                                iter_project->second->GetName(),
                                                                bl->GetMimeImageId(FileExtManager::TypeProject),
                                                                bl->GetMimeImageId(FileExtManager::TypeProject),
                                                                data);
        if(data->IsActive()) {
            m_treeCtrlView->SetItemBold(projectItemId, true);
        }
        DoBuildProjectNode(projectItemId, iter_project->second);
    }

    if(m_treeCtrlView->HasChildren(root)) {
        m_treeCtrlView->Expand(root);
    }
}

void PHPWorkspaceView::UnLoadWorkspace() { m_treeCtrlView->DeleteAllItems(); }

void PHPWorkspaceView::CreateNewProject(const wxString& name)
{
    bool active = PHPWorkspace::Get()->GetProjects().empty();
    PHPWorkspace::Get()->CreateProject(name, active);
    LoadWorkspace();
}

void PHPWorkspaceView::OnCloseWorkspace(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_treeCtrlView->DeleteAllItems();
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    event.SetEventObject(wxTheApp->GetTopWindow());
    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
}

void PHPWorkspaceView::OnReloadWorkspace(wxCommandEvent& e)
{
    wxUnusedVar(e);
    UnLoadWorkspace();
    LoadWorkspace();
}

void PHPWorkspaceView::OnDeleteProject(wxCommandEvent& e)
{
    // Prompt user
    wxTreeItemId selection = m_treeCtrlView->GetSelection();
    if(selection.IsOk()) {
        ItemData* itemData = DoGetItemData(selection);
        if(itemData && itemData->IsProject()) {
            if(wxMessageBox(wxString() << _("Are you sure you want to delete\nproject '") << itemData->GetProjectName()
                                       << "'?",
                            _("CodeLite"),
                            wxYES_NO | wxCANCEL,
                            wxTheApp->GetTopWindow()) == wxYES) {
                PHPWorkspace::Get()->DeleteProject(itemData->GetProjectName());
                m_treeCtrlView->Delete(selection);
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

    wxTreeItemId parent = m_treeCtrlView->GetSelection();
    CHECK_ITEM_RET(parent);

    ItemData* itemData = DoGetItemData(parent);
    CHECK_PTR_RET(itemData);

    if(!itemData->IsFolder() && !itemData->IsProject()) return;

    PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(proj);

    wxString folder;
    if(itemData->IsFolder()) {
        folder << itemData->GetFolderPath() << "/" << name;
    } else {
        folder << name;
    }
    PHPFolder::Ptr_t pFolder = proj->AddFolder(folder);
    proj->Save();

    // Update the UI
    DoAddFolder(parent, pFolder, proj);
}

void PHPWorkspaceView::OnSetProjectActive(wxCommandEvent& e)
{
    wxString project = DoGetSelectedProject();
    if(project.IsEmpty()) return;

    PHPWorkspace::Get()->SetProjectActive(project);
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrlView->GetFirstChild(m_treeCtrlView->GetRootItem(), cookie);
    while(child.IsOk()) {
        ItemData* id = DoGetItemData(child);
        if(id && id->IsProject()) {
            id->SetActive(id->GetProjectName() == project);
            m_treeCtrlView->SetItemBold(child, id->IsActive());
        }
        child = m_treeCtrlView->GetNextChild(m_treeCtrlView->GetRootItem(), cookie);
    }
}

wxString PHPWorkspaceView::DoGetSelectedProject()
{
    wxTreeItemId item = m_treeCtrlView->GetSelection();
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
    if(data->IsFolder()) {
        PHPProject::Ptr_t pProj = PHPWorkspace::Get()->GetProject(data->GetProjectName());
        CHECK_PTR_RET(pProj);

        PHPFolder::Ptr_t pFolder = pProj->Folder(data->GetFolderPath());
        CHECK_PTR_RET(pFolder);

        NewFileDlg dlg(wxTheApp->GetTopWindow(), pFolder->GetFullPath(pProj->GetFilename().GetPath()));
        if(dlg.ShowModal() == wxID_OK) {
            wxFileName filename = dlg.GetPath();
            if(filename.FileExists()) {
                // A file with this name already exists
                wxMessageBox(
                    _("A file with same name already exists!"), wxT("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
                return;
            }
            DoAddFileWithContent(folderId, filename, "");
        }
    }
}

void PHPWorkspaceView::DoAddFolderFiles(const wxString& project,
                                        const wxString& folder,
                                        const wxTreeItemId& folderTreeItemId,
                                        wxStringSet_t* filesAdded)
{
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(pProject);

    PHPFolder::Ptr_t pFolder = pProject->Folder(folder);
    CHECK_PTR_RET(pFolder);

    wxArrayString files;
    pFolder->GetFiles(files, pProject->GetFilename().GetPath());
    files.Sort();

    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxFileName fn(files.Item(i));
        // create the user data
        ItemData* itemData = new ItemData(ItemData::Kind_File);
        itemData->SetFile(fn.GetFullPath());
        itemData->SetFolderPath(folder);
        itemData->SetProjectName(project);

        int imgId = DoGetItemImgIdx(fn.GetFullName());
        m_treeCtrlView->AppendItem(folderTreeItemId, fn.GetFullName(), imgId, imgId, itemData);

        if(filesAdded) {
            filesAdded->insert(fn.GetFullPath());
        }
    }
}

void PHPWorkspaceView::OnAddFile(wxCommandEvent& e)
{
    wxTreeItemId folderItem = DoGetSingleSelection();
    CHECK_ITEM_RET(folderItem);

    ItemData* data = DoGetItemData(folderItem);
    CHECK_PTR_RET(data);

    if(!data->IsFolder()) return;

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(data->GetProjectName());
    CHECK_PTR_RET(pProject);

    PHPFolder::Ptr_t pFolder = pProject->Folder(data->GetFolderPath());
    CHECK_PTR_RET(pFolder);

    wxFileDialog dlg(wxTheApp->GetTopWindow(),
                     _("Add file"),
                     pFolder->GetFullPath(pProject->GetFilename().GetPath()),
                     wxT(""),
                     wxT("PHP Files (*.php)|*.php|PHP include files (*.inc)|*.inc|All files (*)|*"),
                     wxFD_FILE_MUST_EXIST | wxFD_OPEN | wxFD_MULTIPLE);
    if(dlg.ShowModal() == wxID_OK) {

        wxArrayString paths;
        dlg.GetPaths(paths);

        if(paths.IsEmpty()) return;

        // if the file is not under the project's tree - do not add it
        // Use the folder taken from the file's full path and the pFolder as the base folder for adding the file
        if(!paths.Item(0).StartsWith(pProject->GetFilename().GetPath())) {
            ::wxMessageBox(_("Can not add file to project. File must be under the project directory tree"),
                           "CodeLite",
                           wxOK | wxCENTER | wxICON_WARNING);
            return;
        }

        DoAddFilesToFolder(paths, pProject, true);
        pProject->Save();
    }
}

void PHPWorkspaceView::DoAddFilesToFolder(const wxArrayString& paths, PHPProject::Ptr_t pProject, bool notify)
{
    if(paths.IsEmpty()) return;

    CHECK_PTR_RET(pProject);

    wxString folder_path = PHPFolder::GetFolderPathFromFileFullPath(paths.Item(0), pProject->GetFilename().GetPath());
    wxTreeItemId folderItem =
        EnsureFolderExists(DoGetProjectItem(pProject->GetName()), folder_path, pProject->GetName());
    PHPFolder::Ptr_t pFolder = pProject->AddFolder(folder_path);
    CHECK_PTR_RET(pFolder);

    wxArrayString newPaths;
    for(size_t i = 0; i < paths.GetCount(); i++) {
        if(pFolder->AddFile(paths.Item(i))) {
            newPaths.Add(paths.Item(i));
        }
    }
    if(newPaths.IsEmpty()) return;

    if(notify) {
        DoAddFilesToTreeView(folderItem, pProject, pFolder, newPaths);
        // Notify the plugins
        clCommandEvent evtFilesAdded(wxEVT_PROJ_FILE_ADDED);
        evtFilesAdded.SetStrings(newPaths);
        EventNotifier::Get()->AddPendingEvent(evtFilesAdded);
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

    wxString msg = wxString() << _("Are you sure you want to delete folder '") << folder
                              << ("'?\nNote: selecting 'Yes' will NOT remove any file from the file system");
    if(wxMessageBox(msg, wxT("CodeLite"), wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTER) != wxYES) return;

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    CHECK_PTR_RET(pProject);
    pProject->DeleteFolder(folder);
    pProject->Save();

    // Update the UI
    m_treeCtrlView->Delete(folderItem);
}

void PHPWorkspaceView::OnImportFiles(wxCommandEvent& e)
{
    ImportFilesDlg dlg(wxTheApp->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {
        PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(DoGetSelectedProject());
        CHECK_PTR_RET(pProject);

        pProject->ImportDirectory(dlg.GetPath(), dlg.GetFileSpec(), dlg.GetIsRecursive());
        LoadWorkspace();
    }
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
    wxTreeItemId item = DoGetSingleSelection();
    if(item.IsOk() == false) return;
    DoOpenFile(item);
}

void PHPWorkspaceView::DoOpenFile(const wxTreeItemId& item)
{
    ItemData* data = DoGetItemData(item);
    if(data && data->IsFile()) {
        m_mgr->OpenFile(data->GetFile());
    }
}

void PHPWorkspaceView::OnRemoveFile(wxCommandEvent& e) { DoDeleteSelectedFileItem(); }

void PHPWorkspaceView::OnRenameFile(wxCommandEvent& e)
{
    wxTreeItemId item = DoGetSingleSelection();
    CHECK_ITEM_RET(item);

    ItemData* data = DoGetItemData(item);
    CHECK_PTR_RET(data);
    CHECK_ID_FILE(data);

    wxFileName old_file_name = data->GetFile();
    wxString new_name = ::wxGetTextFromUser(_("New file name:"), _("Rename file"), old_file_name.GetFullName());
    if(new_name.IsEmpty()) return;

    if(new_name == old_file_name.GetFullName()) return;

    // Check to see if we got a file with the old path opened
    bool reopenFile = false;
    IEditor* editor = m_mgr->FindEditor(old_file_name.GetFullPath());
    if(editor) {
        m_mgr->ClosePage(editor->GetFileName().GetFullName());
        reopenFile = true;
        editor = NULL;
    }

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(DoGetSelectedProject());
    CHECK_PTR_RET(pProject);

    if(pProject->RenameFile(old_file_name.GetFullPath(), new_name)) {
        // Locate the folder
        pProject->Save();
        m_treeCtrlView->SetItemText(item, new_name);
        // Update the item data
        old_file_name.SetFullName(new_name);
        data->SetFile(old_file_name.GetFullPath());

        // Open the file if it was opened earlier
        // old_file_name now contains the new full path to the new file
        if(reopenFile) {
            m_mgr->OpenFile(old_file_name.GetFullPath());
        }
    }
}

void PHPWorkspaceView::DoDeleteSelectedFileItem()
{
    wxTreeItemId item = DoGetSingleSelection();
    CHECK_ITEM_RET(item);

    ItemData* itemData = DoGetItemData(item);
    if(!itemData || !itemData->IsFile()) return;

    wxString msg, caption;
    caption << _("Removing file '") << wxFileName(itemData->GetFile()).GetFullName() << "'";
    msg << _("Removing file '") << wxFileName(itemData->GetFile()).GetFullName() << "' from project '"
        << itemData->GetProjectName() << "'\n"
        << _("You can select to remove the file from the project and optionally from the file system\n")
        << _("From where should codelite remove the file?");

    wxMessageDialog dlg(EventNotifier::Get()->TopFrame(), msg, caption, wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT);
    dlg.SetYesNoCancelLabels("File system && project", "Project only", "Cancel");

    int answer = dlg.ShowModal();
    if(answer == wxID_YES || answer == wxID_NO) {
        wxString fileName = itemData->GetFile();
        // Remove from from project
        if(PHPWorkspace::Get()->DelFile(itemData->GetProjectName(), itemData->GetFolderPath(), fileName)) {
            m_treeCtrlView->Delete(item);

        } else {
            return;
        }
        if(answer == wxID_YES) {
            ::wxRemoveFile(fileName);
        }
    }
}

void PHPWorkspaceView::OnRunProject(wxCommandEvent& e)
{
    PHPWorkspace::Get()->RunProject(false, DoGetSelectedProject());
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
    PHPProjectSettingsDlg settingsDlg(FRAME, PHPWorkspace::Get()->GetActiveProjectName());
    settingsDlg.ShowModal();
}

void PHPWorkspaceView::OnActiveProjectSettingsUI(wxUpdateUIEvent& event)
{
    event.Enable(PHPWorkspace::Get()->IsOpen());
}

void PHPWorkspaceView::OnProjectSettings(wxCommandEvent& event)
{
    PHPProjectSettingsDlg settingsDlg(FRAME, DoGetSelectedProject());
    settingsDlg.ShowModal();
}

void PHPWorkspaceView::OnRunActiveProject(clExecuteEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        PHPWorkspace::Get()->RunProject(false);

    } else {
        // Must call skip !
        e.Skip();
    }
}

void PHPWorkspaceView::OnIsProgramRunning(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        e.SetInt(PHPWorkspace::Get()->IsProjectRunning() ? 1 : 0);

    } else {
        // Must call skip !
        e.Skip();
    }
}

void PHPWorkspaceView::OnStopExecutedProgram(wxCommandEvent& e)
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
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    wxUnusedVar(editor);

    CHECK_PTR_RET(editor);
    wxFileName filename = editor->GetFileName();

    // FIXME : Load all breakpoints for this file and apply them to the UI

    // Locate the editor for this file
    PHPProject::Ptr_t pProject(NULL);
    const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();
    PHPProject::Map_t::const_iterator iter = projects.begin();
    for(; iter != projects.end(); ++iter) {
        if(filename.GetPath().StartsWith(iter->second->GetFilename().GetPath())) {
            // found our project
            pProject = iter->second;
            break;
        }
    }

    CHECK_PTR_RET(pProject);

    PHPFolder::Ptr_t pFolder = pProject->FolderByFileFullPath(filename.GetFullPath());
    CHECK_PTR_RET(pFolder);

    wxTreeItemId projectItem = DoGetProjectItem(pProject->GetName());
    CHECK_ITEM_RET(projectItem);

    wxTreeItemId folderItem = DoGetFolderItem(projectItem, pFolder);
    CHECK_ITEM_RET(folderItem);

    wxTreeItemId fileItem = DoGetFileItem(folderItem, filename.GetFullPath());
    CHECK_ITEM_RET(fileItem);

    m_treeCtrlView->SelectItem(fileItem);
    m_treeCtrlView->EnsureVisible(fileItem);
}

void PHPWorkspaceView::OnFileRenamed(PHPEvent& e)
{
    e.Skip();
    // FIXME: a file was renamed, update the UI
}

void PHPWorkspaceView::OnMakeIndexPHP(wxCommandEvent& e)
{
    e.Skip();
    wxTreeItemId item = DoGetSingleSelection();
    CHECK_ITEM_RET(item);

    ItemData* itemData = DoGetItemData(item);
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
    wxTreeItemId folderId = DoGetSingleSelection();
    ItemData* data = DoGetItemData(folderId);
    if(!data || !data->IsFolder()) {
        return;
    }

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(data->GetProjectName());
    CHECK_PTR_RET(pProject);

    PHPFolder::Ptr_t pFolder = pProject->Folder(data->GetFolderPath());
    CHECK_PTR_RET(pFolder);

    NewPHPClass dlg(wxTheApp->GetTopWindow(), pFolder->GetFullPath(pProject->GetFilename().GetPath(), false));
    if(dlg.ShowModal() == wxID_OK) {
        PHPClassDetails pcd = dlg.GetDetails();
        pFolder = pProject->AddFolder(PHPFolder::GetFolderPathFromFileFullPath(pcd.GetFilepath().GetFullPath(),
                                                                               pProject->GetFilename().GetPath()));
        CHECK_PTR_RET(pFolder);

        folderId = EnsureFolderExists(
            DoGetProjectItem(pProject->GetName()), pFolder->GetPathRelativeToProject(), pProject->GetName());
        DoAddFileWithContent(
            folderId, pcd.GetFilepath(), pcd.ToString(EditorConfigST::Get()->GetOptions()->GetEOLAsString(), "    "));
    }
}

void PHPWorkspaceView::DoAddFileWithContent(const wxTreeItemId& folderId,
                                            const wxFileName& filename,
                                            const wxString& content)
{
    // file can only be added to a folder
    ItemData* data = DoGetItemData(folderId);
    if(!data || !data->IsFolder()) {
        return;
    }

    if(filename.FileExists()) {
        // a file with this name already exists
        wxMessageBox(_("A file with same name already exists!"), wxT("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    FileExtManager::FileType type = FileExtManager::GetType(filename.GetFullName());

    // Create the file
    const wxString __EOL__ = EditorConfigST::Get()->GetOptions()->GetEOLAsString();

    // Make sure that the path exists
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    wxFFile fp;
    if(fp.Open(filename.GetFullPath(), wxT("w+"))) {

        // if its is a PHP file, write the php tag at to the top of the file
        if(type == FileExtManager::TypePhp) {
            fp.Write(wxString() << "<?php " << __EOL__ << __EOL__ << content);
        }
        fp.Close();
    }

    // add the new file
    wxString project_name = DoGetSelectedProject();
    wxString folder_name = data->GetFolderPath();

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project_name);
    CHECK_PTR_RET(pProject);

    PHPFolder::Ptr_t pFolder = pProject->Folder(folder_name);
    CHECK_PTR_RET(pFolder);

    if(PHPWorkspace::Get()->AddFile(project_name, folder_name, filename.GetFullPath())) {
        wxArrayString filesToAdd;
        filesToAdd.Add(filename.GetFullPath());
        DoAddFilesToTreeView(folderId, pProject, pFolder, filesToAdd);
    }

    // Open the newly added file
    m_mgr->OpenFile(filename.GetFullPath());

    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) {
        editor->SetCaretAt(editor->GetLength());
    }

    // Notify plugins about new files was added to workspace
    wxArrayString files;
    files.Add(filename.GetFullPath());

    // Notify the plugins
    clCommandEvent evtFilesAdded(wxEVT_PROJ_FILE_ADDED);
    evtFilesAdded.SetStrings(files);
    EventNotifier::Get()->AddPendingEvent(evtFilesAdded);
}

void PHPWorkspaceView::OnWorkspaceOpenUI(wxUpdateUIEvent& event) { event.Enable(PHPWorkspace::Get()->IsOpen()); }

void PHPWorkspaceView::OnWorkspaceRenamed(PHPEvent& e)
{
    e.Skip();
    wxFAIL_MSG("PHPWorkspaceView::OnWorkspaceRenamed not implemented just yet...");
}

void PHPWorkspaceView::OnRenameWorkspace(wxCommandEvent& e)
{
    wxString new_name = ::wxGetTextFromUser(_("New workspace name:"), _("Rename workspace"));
    if(!new_name.IsEmpty()) {
        PHPWorkspace::Get()->Rename(new_name);
    }
}
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
            menu.Connect(ID_TOGGLE_AUTOMATIC_UPLOAD,
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(PHPWorkspaceView::OnToggleAutoUpload),
                         NULL,
                         this);
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

void PHPWorkspaceView::OnToggleAutoUpload(wxCommandEvent& e)
{
    SSHWorkspaceSettings settings;
    settings.Load();
    settings.EnableRemoteUpload(e.IsChecked());
    settings.Save();
}

void PHPWorkspaceView::DoOpenSSHAccountManager()
{
    SSHWorkspaceSettings settings;
    settings.Load();

    SFTPBrowserDlg dlg(EventNotifier::Get()->TopFrame(),
                       _("Select the remote folder corrseponding to the current workspace file"),
                       "",
                       clSFTP::SFTP_BROWSE_FOLDERS); // Browse for folders only
    dlg.Initialize(settings.GetAccount(), settings.GetRemoteFolder());

    if(dlg.ShowModal() == wxID_OK) {
        settings.SetAccount(dlg.GetAccount());
        settings.SetRemoteFolder(dlg.GetPath());
        settings.Save();
    }
}

void PHPWorkspaceView::ReloadWorkspace(bool saveBeforeReload)
{
    wxFileName fnWorkspace = PHPWorkspace::Get()->GetFilename();
    PHPWorkspace::Get()->Close(saveBeforeReload);
    PHPWorkspace::Get()->Open(fnWorkspace.GetFullPath());
    UnLoadWorkspace();
    LoadWorkspace();
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

void PHPWorkspaceView::DoBuildProjectNode(const wxTreeItemId& projectItem, PHPProject::Ptr_t project)
{
    // Get the top level folders and add them
    const PHPFolder::Map_t& folders = project->GetFolders();
    PHPFolder::Map_t::const_iterator iter = folders.begin();
    for(; iter != folders.end(); ++iter) {
        DoAddFolder(projectItem, iter->second, project);
    }
}

void PHPWorkspaceView::DoAddFolder(const wxTreeItemId& parent, PHPFolder::Ptr_t folder, PHPProject::Ptr_t project)
{
    // add the folder itself
    ItemData* itemData = new ItemData(ItemData::Kind_Folder);
    itemData->SetFolderPath(folder->GetPathRelativeToProject());
    itemData->SetProjectName(project->GetName());
    itemData->SetFolderName(folder->GetName()); // display name

    wxTreeItemId folderItem = EnsureFolderExists(
        DoGetProjectItem(project->GetName()), folder->GetPathRelativeToProject(), project->GetName());

    // Get list of folders
    const PHPFolder::List_t& children = folder->GetChildren();
    PHPFolder::List_t::const_iterator iter = children.begin();
    for(; iter != children.end(); ++iter) {
        DoAddFolder(folderItem, *iter, project);
    }

    wxArrayString folderFiles;
    folder->GetFiles(folderFiles, project->GetFilename().GetPath(), 0);

    // Add the files
    for(size_t i = 0; i < folderFiles.GetCount(); ++i) {
        ItemData* itemData = new ItemData(ItemData::Kind_File);
        itemData->SetFile(folderFiles.Item(i));
        itemData->SetProjectName(project->GetName());
        itemData->SetFolderPath(folder->GetPathRelativeToProject());
        int imgId = DoGetItemImgIdx(folderFiles.Item(i));
        m_treeCtrlView->AppendItem(folderItem, wxFileName(folderFiles.Item(i)).GetFullName(), imgId, imgId, itemData);
    }
}

wxTreeItemId PHPWorkspaceView::DoGetSingleSelection() { return m_treeCtrlView->GetFocusedItem(); }

void PHPWorkspaceView::DoAddFilesToTreeView(const wxTreeItemId& folderId,
                                            PHPProject::Ptr_t pProject,
                                            PHPFolder::Ptr_t pFolder,
                                            const wxArrayString& files)
{
    for(size_t i = 0; i < files.GetCount(); ++i) {
        ItemData* itemData = new ItemData(ItemData::Kind_File);
        itemData->SetFile(files.Item(i));
        itemData->SetProjectName(pProject->GetName());
        itemData->SetFolderPath(pFolder->GetPathRelativeToProject());

        int imgId = DoGetItemImgIdx(files.Item(i));
        m_treeCtrlView->AppendItem(folderId, wxFileName(files.Item(i)).GetFullName(), imgId, imgId, itemData);
    }
    m_treeCtrlView->SortChildren(folderId);
}

wxTreeItemId
PHPWorkspaceView::EnsureFolderExists(const wxTreeItemId& projectItem, const wxString& path, const wxString& project)
{
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(project);
    wxASSERT_MSG(pProject, wxString::Format("Could not locate project %s", project));

    int imgId = m_mgr->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    wxString curpath;
    wxTreeItemId parent = projectItem;
    wxArrayString parts = ::wxStringTokenize(path, "/", wxTOKEN_STRTOK);
    for(size_t i = 0; i < parts.GetCount(); ++i) {
        if(!curpath.IsEmpty()) {
            curpath << "/";
        }
        curpath << parts.Item(i);

        if(!HasFolderWithName(parent, parts.Item(i), parent)) {
            ItemData* itemData = new ItemData(ItemData::Kind_Folder);
            itemData->SetFolderPath(curpath);
            itemData->SetProjectName(project);
            itemData->SetFolderName(parts.Item(i));
            parent =
                m_treeCtrlView->AppendItem(parent,
                                           parts.Item(i) == "." ? pProject->GetFilename().GetPath() : parts.Item(i),
                                           imgId,
                                           imgId,
                                           itemData);
        }
    }
    m_treeCtrlView->SortChildren(projectItem);
    return parent;
}

bool PHPWorkspaceView::HasFolderWithName(const wxTreeItemId& parent, const wxString& name, wxTreeItemId& item) const
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrlView->GetFirstChild(parent, cookie);
    while(child.IsOk()) {
        const ItemData* itemData = DoGetItemData(child);
        if(itemData && itemData->IsFolder() && itemData->GetFolderName() == name) {
            item = child;
            return true;
        }
        child = m_treeCtrlView->GetNextChild(parent, cookie);
    }
    return false;
}

wxTreeItemId PHPWorkspaceView::DoGetProjectItem(const wxString& projectName)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrlView->GetFirstChild(m_treeCtrlView->GetRootItem(), cookie);
    while(child.IsOk()) {
        if(m_treeCtrlView->GetItemText(child) == projectName) {
            return child;
        }
        child = m_treeCtrlView->GetNextChild(m_treeCtrlView->GetRootItem(), cookie);
    }
    return wxTreeItemId();
}

wxTreeItemId PHPWorkspaceView::DoGetFolderItem(const wxTreeItemId& projectItem, PHPFolder::Ptr_t pFolder)
{
    wxString curpath, path;
    path = pFolder->GetPathRelativeToProject();

    wxTreeItemId parent = projectItem;
    wxArrayString parts = ::wxStringTokenize(path, "/", wxTOKEN_STRTOK);
    for(size_t i = 0; i < parts.GetCount(); ++i) {
        if(!curpath.IsEmpty()) {
            curpath << "/";
        }
        curpath << parts.Item(i);

        if(!HasFolderWithName(parent, parts.Item(i), parent)) {
            return wxTreeItemId();
        }
    }
    return parent;
}

void PHPWorkspaceView::OnItemActivated(wxTreeEvent& event)
{
    ItemData* itemData = DoGetItemData(event.GetItem());
    if(itemData && itemData->IsFile()) {
        DoOpenFile(event.GetItem());

    } else {
        event.Skip();
    }
}

wxTreeItemId PHPWorkspaceView::DoGetFileItem(const wxTreeItemId& folderItem, const wxString& filename)
{
    wxFileName fnFullname(filename);
    wxString fullname = fnFullname.GetFullName();

    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrlView->GetFirstChild(folderItem, cookie);
    while(child.IsOk()) {
        const ItemData* itemData = DoGetItemData(child);
        if(itemData && itemData->IsFile() && wxFileName(itemData->GetFile()).GetFullName() == fullname) {
            return child;
        }
        child = m_treeCtrlView->GetNextChild(folderItem, cookie);
    }
    return wxTreeItemId();
}
