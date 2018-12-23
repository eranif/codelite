//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : sftp.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef __SFTP__
#define __SFTP__

#include "clFileSystemEvent.h"
#include "clSFTPEvent.h"
#include "clTabTogglerHelper.h"
#include "cl_command_event.h"
#include "macros.h"
#include "plugin.h"
#include "remote_file_info.h"
#include "sftp_workspace_settings.h"

class SFTPStatusPage;
class SFTPTreeView;

class SFTPClientData : public wxClientData
{
    wxString localPath;
    wxString remotePath;
    size_t permissions;
    int lineNumber = wxNOT_FOUND;

public:
    SFTPClientData()
        : permissions(0)
    {
    }
    virtual ~SFTPClientData() {}

    void SetLocalPath(const wxString& localPath) { this->localPath = localPath; }
    void SetRemotePath(const wxString& remotePath) { this->remotePath = remotePath; }
    const wxString& GetLocalPath() const { return localPath; }
    const wxString& GetRemotePath() const { return remotePath; }
    void SetPermissions(size_t permissions) { this->permissions = permissions; }
    size_t GetPermissions() const { return permissions; }
    void SetLineNumber(int lineNumber) { this->lineNumber = lineNumber; }
    int GetLineNumber() const { return lineNumber; }
};

class SFTP : public IPlugin
{
    wxFileName m_workspaceFile;
    SFTPWorkspaceSettings m_workspaceSettings;
    SFTPStatusPage* m_outputPane;
    SFTPTreeView* m_treeView;
    RemoteFileInfo::Map_t m_remoteFiles;
    clTabTogglerHelper::Ptr_t m_tabToggler;

public:
    SFTP(IManager* manager);
    ~SFTP();

    void FileDownloadedSuccessfully(const SFTPClientData& cd);
    void OpenWithDefaultApp(const wxString& localFileName);
    void OpenContainingFolder(const wxString& localFileName);
    void AddRemoteFile(const RemoteFileInfo& remoteFile);
    SFTPStatusPage* GetOutputPane() { return m_outputPane; }
    SFTPTreeView* GetTreeView() { return m_treeView; }

    void OpenFile(const wxString& remotePath, int lineNumber = wxNOT_FOUND);

protected:
    void OnReplaceInFiles(clFileSystemEvent& e);
    void OnAccountManager(wxCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnSetupWorkspaceMirroring(wxCommandEvent& e);
    void OnDisableWorkspaceMirroring(wxCommandEvent& e);
    void OnDisableWorkspaceMirroringUI(wxUpdateUIEvent& e);
    void OnWorkspaceOpened(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnFileSaved(clCommandEvent& e);
    void OnFileRenamed(clFileSystemEvent& e);
    void OnFileDeleted(clFileSystemEvent& e);
    void OnEditorClosed(wxCommandEvent& e);
    void MSWInitiateConnection();

    void DoFileSaved(const wxString& filename);

    bool IsWorkspaceOpened() const { return m_workspaceFile.IsOk(); }
    void DoSaveRemoteFile(const RemoteFileInfo& remoteFile);
    bool IsPaneDetached(const wxString& name) const;

    // API calls

    // Save remote file content to match the content of a local file
    // e.GetLocalFile() -> the local file
    // e.GetRemoteFile() -> the target file
    void OnSaveFile(clSFTPEvent& e);

    // Rename a remote file
    // e.GetRemoteFile() -> the "old" remote file path
    // e.GetNewRemoteFile() -> the "new" remote file path
    void OnRenameFile(clSFTPEvent& e);

    // Delete a remote file
    // e.GetRemoteFile() -> the file to be deleted
    void OnDeleteFile(clSFTPEvent& e);

private:
    bool IsCxxWorkspaceMirrorEnabled() const;
    void DoFileDeleted(const wxString& filepath);
    wxString GetRemotePath(const wxString& localpath) const;

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
    IManager* GetManager() { return m_mgr; }

    // Callbacks
    void OnFileWriteOK(const wxString& message);
    void OnFileWriteError(const wxString& errorMessage);
};

#endif // SFTP
