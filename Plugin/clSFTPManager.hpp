#ifndef CLSFTPMANAGER_HPP
#define CLSFTPMANAGER_HPP

#if USE_SFTP
#include "cl_command_event.h"
#include "cl_sftp.h"
#include "codelite_exports.h"
#include "sync_queue.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <wx/event.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/timer.h>

class IEditor;
class SFTPClientData;
class WXDLLIMPEXP_SDK clSFTPManager : public wxEvtHandler
{
protected:
    struct saved_file {
        wxString local_path;
        wxString remote_path;
        wxString account_name;
    };

protected:
    std::unordered_map<wxString, std::pair<SSHAccountInfo, clSFTP::Ptr_t>> m_connections;
    wxTimer* m_timer = nullptr;
    bool m_eventsConnected = true;
    std::thread* m_worker_thread = nullptr;
    SyncQueue<std::function<void()>> m_q;
    atomic_bool m_shutdown;
    wxString m_lastError;
    std::unordered_map<wxString, saved_file> m_downloadedFileToAccount;

protected:
    std::pair<SSHAccountInfo, clSFTP::Ptr_t> GetConnectionPair(const wxString& account) const;
    clSFTP::Ptr_t GetConnectionPtr(const wxString& account) const;
    size_t GetAllConnectionsPtr(std::vector<clSFTP::Ptr_t>& connections) const;
    clSFTP::Ptr_t GetConnectionPtrAddIfMissing(const wxString& account);

protected:
    void OnGoingDown(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    SFTPClientData* GetSFTPClientData(IEditor* editor);
    void OnTimer(wxTimerEvent& event);
    bool DoSyncDownload(const wxString& remotePath, const wxString& localPath, const wxString& accountName);
    void StartWorkerThread();
    void StopWorkerThread();
    void OnSaveCompleted(clCommandEvent& e);
    void OnSaveError(clCommandEvent& e);
    void DoAsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                         bool delete_local, wxEvtHandler* sink);
    bool DoSyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                        bool delete_local);

public:
    clSFTPManager();
    virtual ~clSFTPManager();

    static clSFTPManager& Get();
    void Release();

    /**
     * @brief return true if this file was downloaded via the sftp manager
     * @param filepath path the local file
     * @param account [output] the account this file belongs to
     * @param remote_path [output] the file's remote path
     * @return true if filepath was downloaded via the sftp manager
     */
    bool IsRemoteFile(const wxString& filepath, wxString* account, wxString* remote_path) const;

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
     * @brief download file, but do not open it in a an editor. Optionally, allow the user set the local file name
     * @return return the *local* file path
     */
    wxFileName Download(const wxString& path, const wxString& accountName,
                        const wxString& localFileName = wxEmptyString);

    /**
     * @brief save file remotely. this function is async
     * @param localPath file on the local machine
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @param sink callback object for save file events
     */
    void AsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                       wxEvtHandler* sink = nullptr);

    /**
     * @brief write file content. this function is async
     * @param content of the file
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @param sink callback object for save file events
     */
    void AsyncWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName,
                        wxEvtHandler* sink = nullptr);

    /**
     * @brief save file remotely. this function is sync
     * @param localPath file on the local machine
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @param sink callback object for save file events
     * @return true on success or false
     */
    bool AwaitSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName);

    /**
     * @brief write file content. this function is sync
     * @param content of the file
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @return true on success or false
     */
    bool AwaitWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName);

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
    SFTPAttribute::List_t List(const wxString& path, const SSHAccountInfo& accountInfo);

    /**
     * @brief create new file with a given path
     */
    bool NewFile(const wxString& path, const SSHAccountInfo& accountInfo);
    /**
     * @brief create a new folder with a given path
     */
    bool NewFolder(const wxString& path, const SSHAccountInfo& accountInfo);

    /**
     * @brief rename a file/directory
     * @param oldpath
     * @param newpath
     */
    bool Rename(const wxString& oldpath, const wxString& newpath, const SSHAccountInfo& accountInfo);
    /**
     * @brief delere a directory
     */
    bool DeleteDir(const wxString& fullpath, const SSHAccountInfo& accountInfo);

    /**
     * @brief unlink a file
     */
    bool UnlinkFile(const wxString& fullpath, const SSHAccountInfo& accountInfo);

    /**
     * @brief check if a file with a given path exists
     */
    bool IsFileExists(const wxString& fullpath, const SSHAccountInfo& accountInfo);

    /**
     * @brief check if a directory with a given path exists
     */
    bool IsDirExists(const wxString& fullpath, const SSHAccountInfo& accountInfo);

    /**
     * @brief return the remote path for a local file
     */
    bool GetRemotePath(const wxString& local_path, const wxString& accountName, wxString& remote_path);

    /**
     * @brief return the local path for a remote path
     */
    bool GetLocalPath(const wxString& remote_path, const wxString& accountName, wxString& local_path);

    /**
     * @brief return the last error occurred
     */
    const wxString& GetLastError() const { return m_lastError; }

    /**
     * @brief clear last error captured
     */
    void ClearLastError() { m_lastError.clear(); }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SFTP_ASYNC_SAVE_COMPLETED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SFTP_ASYNC_SAVE_ERROR, clCommandEvent);
#endif
#endif // CLSFTPMANAGER_HPP
