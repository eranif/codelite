#pragma once

#include "clFileSystemEvent.h"
#include "codelite_exports.h"

#include <map>
#include <wx/filename.h>
#include <wx/timer.h>

constexpr int MAX_REMOTE_FILE_RETRIES = 3;

struct WXDLLIMPEXP_SDK clWatchedFile {
    wxString m_filename;
    wxEvtHandler* m_owner{nullptr};
    time_t m_lastModified{0};
    size_t m_fileSize{0};
    wxString m_remoteAccount;
    int m_consecutiveFailures{0};

    clWatchedFile(const wxString& filepath, wxEvtHandler* owner)
        : m_filename{filepath}
        , m_owner{owner}
    {
    }

    clWatchedFile(const wxString& filepath, wxEvtHandler* owner, const wxString& remoteAccount)
        : m_filename{filepath}
        , m_owner{owner}
        , m_remoteAccount{remoteAccount}
    {
    }

    ~clWatchedFile() = default;
    clWatchedFile() = delete;

    bool IsOk() const
    {
        if (m_owner == nullptr) {
            // Must provide an owner
            return false;
        }

        if (m_remoteAccount.empty() && !wxFileExists(m_filename)) {
            // Local file does not exist.
            return false;
        }
        return true;
    }

    inline bool IsRemote() const { return !m_remoteAccount.empty(); }
};

using WatchedFilesMap = std::map<wxString, clWatchedFile>;

class WXDLLIMPEXP_SDK clFileSystemWatcher : public wxEvtHandler
{
public:
    /**
     * @brief Construct a new file-system watcher (timer-based).
     */
    clFileSystemWatcher();

    /**
     * @brief Destructor. Automatically stops the watcher and unbinds the timer event.
     */
    ~clFileSystemWatcher() override;

    static clFileSystemWatcher& Get();

    /**
     * @brief Set a single file to watch. Clears any previously watched files.
     * @param filename full path to the file to watch
     */
    void SetFile(clWatchedFile&& file);
    void SetFile(const wxString& filepath, wxEvtHandler* handler)
    {
        clWatchedFile w{filepath, handler};
        return SetFile(std::move(w));
    }

    /**
     * @brief Add a file to the watch list. Previously watched files are preserved.
     * @param filename full path to the file to watch. If the file does not exist, it is ignored.
     */
    void AddFile(clWatchedFile&& file);
    void AddFile(const wxString& filepath, wxEvtHandler* handler)
    {
        clWatchedFile w{filepath, handler};
        return AddFile(std::move(w));
    }

    void AddFile(const wxString& filepath, const wxString& accountName, wxEvtHandler* handler)
    {
        clWatchedFile w{filepath, handler};
        w.m_remoteAccount = accountName;
        return AddFile(std::move(w));
    }

    /**
     * @brief remove file from the watch list
     */
    void RemoveFile(const wxString& filename);

    inline bool Contains(const wxString& filepath) const
    {
        return m_files.contains(filepath) || m_inFlightChecks.contains(filepath);
    }

    /**
     * @brief start to watching list of files.
     * This object fires the following events (clFileSystemEvent):
     * wxEVT_FILE_MODIFIED, wxEVT_FILE_DELETED
     */
    void Start();

    /**
     * @brief stop watching the list of files
     */
    void Stop();

    /**
     * @brief clear the list of files to watch and stop the watcher
     */
    void Clear();

    /**
     * @brief is the watcher running?
     */
    bool IsRunning() const;

protected:
    void OnTimer(wxTimerEvent& event);
    void HandleLocalFiles();
    void HandleRemoteFiles();

private:
    WatchedFilesMap m_files;
    WatchedFilesMap m_inFlightChecks; // When SFTP is not used, this is always empty
    wxTimer m_timer;
};

struct WXDLLIMPEXP_SDK clWatchedFileLocker {
    wxString m_filepath;
    clFileSystemWatcher* m_watcher{nullptr};
    clWatchedFileLocker(const wxString& filepath, wxEvtHandler* handler, clFileSystemWatcher* watcher = nullptr)
        : m_filepath{filepath}
    {
        m_watcher = watcher == nullptr ? &clFileSystemWatcher::Get() : watcher;
        clWatchedFile watchedFile{m_filepath, handler};
        if (!watchedFile.IsOk()) {
            m_filepath.clear();
            m_watcher = nullptr;
            return;
        }
        m_watcher->AddFile(std::move(watchedFile));
    }

    ~clWatchedFileLocker()
    {
        if (m_watcher && !m_filepath.empty()) {
            m_watcher->RemoveFile(m_filepath);
        }
    }
};

struct WXDLLIMPEXP_SDK clWatchedFileDisabler {
    wxString m_filepath;
    clFileSystemWatcher* m_watcher{nullptr};
    wxEvtHandler* m_handler{nullptr};
    bool m_add{false};
    clWatchedFileDisabler(const wxString& filepath, wxEvtHandler* handler, clFileSystemWatcher* watcher = nullptr)
        : m_filepath{filepath}
        , m_handler{handler}
    {
        m_watcher = watcher == nullptr ? &clFileSystemWatcher::Get() : watcher;
        clWatchedFile watchedFile{m_filepath, m_handler};
        if (!watchedFile.IsOk() || !m_watcher->Contains(m_filepath)) {
            return;
        }

        m_add = true;
        m_watcher->RemoveFile(m_filepath);
    }

    ~clWatchedFileDisabler()
    {
        if (m_add) {
            m_watcher->AddFile(m_filepath, m_handler);
        }
    }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FILE_NOT_FOUND, clFileSystemEvent);
