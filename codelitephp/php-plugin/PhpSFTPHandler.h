#ifndef PHPSFTPHANDLER_H
#define PHPSFTPHANDLER_H

#if USE_SFTP

#include "clFileSystemEvent.h"
#include "codelite_events.h"
#include "ssh_workspace_settings.h"
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include "ssh_workspace_settings.h"

class PhpSFTPHandler : public wxEvtHandler
{
public:
    typedef wxSharedPtr<PhpSFTPHandler> Ptr_t;

protected:
    void DoSyncFileWithRemote(const wxFileName& localFile);
    wxString GetRemotePath(const SSHWorkspaceSettings& sshSettings, const wxString& localpath) const;
    bool EnsureAccountExists(SSHWorkspaceSettings& workspaceSettings);
    
public:
    PhpSFTPHandler();
    virtual ~PhpSFTPHandler();

    void OnFileSaved(clCommandEvent& e);
    void OnFileRenamed(clFileSystemEvent& e);
    void OnReplaceInFiles(clFileSystemEvent& e);
    void OnFileDeleted(clFileSystemEvent& e);
};

#endif //USE_SFTP

#endif // PHPSFTPHANDLER_H
