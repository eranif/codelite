#ifndef CLSFTPMANAGER_HPP
#define CLSFTPMANAGER_HPP

#if USE_SFTP
#include "cl_command_event.h"
#include "cl_sftp.h"
#include "codelite_exports.h"
#include <atomic>
#include <thread>
#include <unordered_map>
#include <wx/event.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <wx/timer.h>

class IEditor;
class SFTPClientData;
class WXDLLIMPEXP_SDK clSFTPManager : public wxEvtHandler
{
protected:
    struct save_request {
        clSFTP::Ptr_t conn;
        wxString local_path;
        wxString remote_path;
        wxEvtHandler* sink = nullptr;
    };

protected:
    std::unordered_map<wxString, std::pair<SSHAccountInfo, clSFTP::Ptr_t>> m_connections;
    wxTimer* m_timer = nullptr;
    bool m_eventsConnected = true;
    std::thread* m_saveThread = nullptr;
    wxMessageQueue<save_request*> m_q;
    atomic_bool m_shutdown;

protected:
    std::pair<SSHAccountInfo, clSFTP::Ptr_t> GetConnectionPair(const wxString& account) const;
    clSFTP::Ptr_t GetConnectionPtr(const wxString& account) const;
    clSFTP::Ptr_t GetConnectionPtrAddIfMissing(const wxString& account);
    save_request* MakeSaveRequest(clSFTP::Ptr_t conn, const wxString& local_path, const wxString& remote_path);

protected:
    void OnGoingDown(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    SFTPClientData* GetSFTPClientData(IEditor* editor);
    void OnTimer(wxTimerEvent& event);
    bool DoDownload(const wxString& remotePath, const wxString& localPath, const wxString& accountName);
    void StartSaveThread();
    void OnSaveCompleted(clCommandEvent& e);
    void OnSaveError(clCommandEvent& e);

public:
    clSFTPManager();
    virtual ~clSFTPManager();

    static clSFTPManager& Get();
    void Release();

    /**
     * @brief add new connection to the manager. if a connection for this account already exists and 'replace' is set to
     * true, replace it
     */
    bool AddConnection(const SSHAccountInfo& account, bool replace = false);

    /**
     * @brief open remote file in an editor and return a pointer to the editor
     * @param path file path on the remote machine
     * @param accountName the account name to use
     */
    IEditor* OpenFile(const wxString& path, const wxString& accountName);
    IEditor* OpenFile(const wxString& path, const SSHAccountInfo& accountInfo);

    /**
     * @brief download file, but do not open it in a an editor
     * @return return the *local* file path
     */
    wxFileName Download(const wxString& path, const wxString& accountName);

    /**
     * @brief save file remotely
     * @param localPath file on the local machine
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @return true on success or false
     */
    bool SaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName);

    /**
     * @brief write file content
     * @param content of the file
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @return true on success or false
     */
    bool WriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName);

    /**
     * @brief delete a connection
     * if promptUser is set to true and any un-saved files belonged to the connection
     * are opened, the user is prompted to save them
     */
    bool DeleteConnection(const wxString& accountName, bool promptUser = false);

    /**
     * @brief list entries in a given folder for a given account
     * @param path
     * @return
     */
    SFTPAttribute::List_t List(const wxString& path, const SSHAccountInfo& accountInfo) const;

    /**
     * @brief create new file with a given path
     */
    bool NewFile(const wxString& path, const SSHAccountInfo& accountInfo) const;
    /**
     * @brief create a new folder with a given path
     */
    bool NewFolder(const wxString& path, const SSHAccountInfo& accountInfo) const;

    /**
     * @brief rename a file/directory
     * @param oldpath
     * @param newpath
     */
    bool Rename(const wxString& oldpath, const wxString& newpath, const SSHAccountInfo& accountInfo) const;
    /**
     * @brief delere a directory
     */
    bool DeleteDir(const wxString& fullpath, const SSHAccountInfo& accountInfo) const;

    /**
     * @brief unlink a file
     */
    bool UnlinkFile(const wxString& fullpath, const SSHAccountInfo& accountInfo) const;

    /**
     * @brief check if a file with a given path exists
     */
    bool IsFileExists(const wxString& fullpath, const SSHAccountInfo& accountInfo) const;

    /**
     * @brief check if a directory with a given path exists
     */
    bool IsDirExists(const wxString& fullpath, const SSHAccountInfo& accountInfo) const;

    /**
     * @brief return the remote path for a local file
     */
    bool GetRemotePath(const wxString& local_path, const wxString& accountName, wxString& remote_path) const;

    /**
     * @brief return the local path for a remote path
     */
    bool GetLocalPath(const wxString& remote_path, const wxString& accountName, wxString& local_path) const;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SFTP_ASYNC_SAVE_COMPLETED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SFTP_ASYNC_SAVE_ERROR, clCommandEvent);
#endif
#endif // CLSFTPMANAGER_HPP
