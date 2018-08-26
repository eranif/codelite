#if USE_SFTP

#include "PhpSFTPHandler.h"
#include "clSFTPEvent.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "php_workspace.h"
#include "ssh_workspace_settings.h"
#include "sftp_settings.h"
#include <wx/msgdlg.h>

PhpSFTPHandler::PhpSFTPHandler()
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &PhpSFTPHandler::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_RENAMED, &PhpSFTPHandler::OnFileRenamed, this);
    EventNotifier::Get()->Bind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &PhpSFTPHandler::OnReplaceInFiles, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_DELETED, &PhpSFTPHandler::OnFileDeleted, this);
}

PhpSFTPHandler::~PhpSFTPHandler()
{
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &PhpSFTPHandler::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_RENAMED, &PhpSFTPHandler::OnFileRenamed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &PhpSFTPHandler::OnReplaceInFiles, this);
    EventNotifier::Get()->Unbind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &PhpSFTPHandler::OnReplaceInFiles, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_DELETED, &PhpSFTPHandler::OnFileDeleted, this);
}

void PhpSFTPHandler::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }
    DoSyncFileWithRemote(e.GetFileName());
}

void PhpSFTPHandler::OnReplaceInFiles(clFileSystemEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }
    
    SSHWorkspaceSettings settings;
    settings.Load();
    
    if(!EnsureAccountExists(settings)) { return; }
    
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.size(); ++i) {
        DoSyncFileWithRemote(files.Item(i));
    }
}

void PhpSFTPHandler::OnFileRenamed(clFileSystemEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }

    SSHWorkspaceSettings settings;
    settings.Load();
    
    if(!EnsureAccountExists(settings)) { return; }
    
    wxString oldPath = GetRemotePath(settings, e.GetPath());
    wxString newPath = GetRemotePath(settings, e.GetNewpath());
    if(oldPath.IsEmpty() || newPath.IsEmpty()) { return; }

    clDEBUG() << "PHP SFTP: Renaming:" << oldPath << "->" << newPath;

    // Fire this event, if the sftp plugin is ON, it will handle it
    clSFTPEvent eventRename(wxEVT_SFTP_RENAME_FILE);
    eventRename.SetAccount(settings.GetAccount());
    eventRename.SetRemoteFile(oldPath);
    eventRename.SetNewRemoteFile(newPath);
    EventNotifier::Get()->AddPendingEvent(eventRename);
}

void PhpSFTPHandler::DoSyncFileWithRemote(const wxFileName& localFile)
{
    // Check to see if we got a remote-upload setup
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProjectForFile(localFile);
    if(!pProject) {
        // Not a workspace file
        clDEBUG() << localFile << "is not a PHP workspace file, will not sync it with remote" << clEndl;
        return;
    }

    SSHWorkspaceSettings workspaceSettings;
    workspaceSettings.Load();
    
    if(!EnsureAccountExists(workspaceSettings)) { return; }
    
    // Convert the local path to remote path
    wxString remotePath = GetRemotePath(workspaceSettings, localFile.GetFullPath());
    if(remotePath.IsEmpty()) { return; }

    // Fire this event, if the sftp plugin is ON, it will handle it
    clSFTPEvent eventSave(wxEVT_SFTP_SAVE_FILE);
    eventSave.SetAccount(workspaceSettings.GetAccount());
    eventSave.SetLocalFile(localFile.GetFullPath());
    eventSave.SetRemoteFile(remotePath);
    EventNotifier::Get()->AddPendingEvent(eventSave);
}

wxString PhpSFTPHandler::GetRemotePath(const SSHWorkspaceSettings& sshSettings, const wxString& localpath) const
{
    if(!sshSettings.IsRemoteUploadEnabled() || !sshSettings.IsRemoteUploadEnabled()) { return ""; }
    wxFileName fnLocalFile = localpath;
    fnLocalFile.MakeRelativeTo(PHPWorkspace::Get()->GetFilename().GetPath());
    fnLocalFile.MakeAbsolute(wxFileName(sshSettings.GetRemoteFolder(), "", wxPATH_UNIX).GetPath());
    return fnLocalFile.GetFullPath(wxPATH_UNIX);
}

void PhpSFTPHandler::OnFileDeleted(clFileSystemEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }

    SSHWorkspaceSettings settings;
    settings.Load();
    
    if(!EnsureAccountExists(settings)) { return; }
    
    const wxArrayString& paths = e.GetPaths();
    if(paths.IsEmpty()) { return; }
    
    for(size_t i = 0; i < paths.size(); ++i) {
        wxString remotePath = GetRemotePath(settings, paths.Item(i));
        if(remotePath.IsEmpty()) { return; }
        
        // Fire this event, if the sftp plugin is ON, it will handle it
        clSFTPEvent eventDelete(wxEVT_SFTP_DELETE_FILE);
        eventDelete.SetAccount(settings.GetAccount());
        eventDelete.SetRemoteFile(remotePath);
        EventNotifier::Get()->AddPendingEvent(eventDelete);
    }
}

bool PhpSFTPHandler::EnsureAccountExists(SSHWorkspaceSettings& workspaceSettings)
{
    // Do we need to sync?
    if(!(workspaceSettings.IsRemoteUploadSet() && workspaceSettings.IsRemoteUploadEnabled())) { return false; }
    
    SFTPSettings sftpSettings;
    sftpSettings.Load();
    
    // Try to locate hte SSH account for this workspace
    SSHAccountInfo account;
    if(!sftpSettings.GetAccount(workspaceSettings.GetAccount(), account)) {
        // Failed to locate the SSH account, disable sync
        wxString msg;
        msg << _("Failed to locate SSH account: ") << workspaceSettings.GetAccount() << "\n";
        ::wxMessageBox(msg, _("SFTP"), wxOK | wxICON_ERROR);
        // Clear the sync settings and return
        workspaceSettings.Reset();
        workspaceSettings.Save();
        return false;
    }
    return true;
}

#endif //USE_SFTP
