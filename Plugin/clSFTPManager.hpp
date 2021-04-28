#ifndef CLSFTPMANAGER_HPP
#define CLSFTPMANAGER_HPP

#if USE_SFTP
#include "cl_command_event.h"
#include "cl_sftp.h"
#include "codelite_exports.h"
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
    struct save_request {
        clSFTP::Ptr_t conn;
        wxString local_path;
        wxString remote_path;
        bool delete_local_file = false;
        wxEvtHandler* sink = nullptr;              // use standard event methods
        std::function<void()> notify_cb = nullptr; // use notify_cb
    };

protected:
    std::unordered_map<wxString, std::pair<SSHAccountInfo, clSFTP::Ptr_t>> m_connections;
    wxTimer* m_timer = nullptr;
    bool m_eventsConnected = true;
    std::thread* m_saveThread = nullptr;
    wxMessageQueue<save_request*> m_q;
    atomic_bool m_shutdown;
    wxString m_lastError;

protected:
    std::pair<SSHAccountInfo, clSFTP::Ptr_t> GetConnectionPair(const wxString& account) const;
    clSFTP::Ptr_t GetConnectionPtr(const wxString& account) const;
    clSFTP::Ptr_t GetConnectionPtrAddIfMissing(const wxString& account);
    save_request* MakeSaveRequest(clSFTP::Ptr_t conn, const wxString& local_path, const wxString& remote_path,
                                  wxEvtHandler* sink, bool delete_local, std::function<void()> notify_cb);

protected:
    void OnGoingDown(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    SFTPClientData* GetSFTPClientData(IEditor* editor);
    void OnTimer(wxTimerEvent& event);
    bool DoDownload(const wxString& remotePath, const wxString& localPath, const wxString& accountName);
    void StartSaveThread();
    void OnSaveCompleted(clCommandEvent& e);
    void OnSaveError(clCommandEvent& e);
    bool DoSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                    bool delete_local, wxEvtHandler* sink, std::function<void()> notify_cb);

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
     * @return true on success or false
     */
    bool AsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                       wxEvtHandler* sink = nullptr);

    /**
     * @brief write file content. this function is async
     * @param content of the file
     * @param remotePath file path on the remote machine
     * @param accountName the account name to use
     * @param sink callback object for save file events
     * @return true on success or false
     */
    bool AsyncWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName,
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

    /**
     * @brief return the last error occurred
     */
    const wxString& GetLastError() const
    {
        return m_lastError;
    }

    /**
     * @brief clear last error captured
     */
    void ClearLastError()
    {
        m_lastError.clear();
    }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SFTP_ASYNC_SAVE_COMPLETED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SFTP_ASYNC_SAVE_ERROR, clCommandEvent);
#endif
#endif // CLSFTPMANAGER_HPP
