#pragma once

#include "clFileSystemEvent.h"
#include "codelite_exports.h"

#include <map>
#include <wx/filename.h>
#include <wx/timer.h>

struct WXDLLIMPEXP_CL clWatchedFile {
    wxString m_filename;
    wxEvtHandler* m_owner{nullptr};
    time_t m_lastModified{0};
    size_t m_fileSize{0};

    clWatchedFile(const wxString& filepath, wxEvtHandler* owner)
        : m_filename{filepath}
        , m_owner{owner}
    {
    }

    ~clWatchedFile() = default;
    clWatchedFile() = delete;

    inline bool IsOk() const { return m_owner != nullptr && wxFileExists(m_filename); }
};

using WatchedFilesMap = std::map<wxString, clWatchedFile>;

class WXDLLIMPEXP_CL clLocalFileSystemWatcher : public wxEvtHandler
{
public:
    /**
     * @brief Construct a new file-system watcher (timer-based).
     */
    clLocalFileSystemWatcher();

    /**
     * @brief Destructor. Automatically stops the watcher and unbinds the timer event.
     */
    ~clLocalFileSystemWatcher() override;

    static clLocalFileSystemWatcher& Get();

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

    /**
     * @brief remove file from the watch list
     */
    void RemoveFile(const wxString& filename);

    inline bool Contains(const wxString& filepath) const { return m_files.contains(filepath); }

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

private:
    WatchedFilesMap m_files;
    wxTimer m_timer;
};

struct WXDLLIMPEXP_CL clWatchedFileLocker {
    wxString m_filepath;
    clLocalFileSystemWatcher* m_watcher{nullptr};
    clWatchedFileLocker(const wxString& filepath, wxEvtHandler* handler, clLocalFileSystemWatcher* watcher = nullptr)
        : m_filepath{filepath}
    {
        m_watcher = watcher == nullptr ? &clLocalFileSystemWatcher::Get() : watcher;
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

struct WXDLLIMPEXP_CL clWatchedFileDisabler {
    wxString m_filepath;
    clLocalFileSystemWatcher* m_watcher{nullptr};
    wxEvtHandler* m_handler{nullptr};
    bool m_add{false};
    clWatchedFileDisabler(const wxString& filepath, wxEvtHandler* handler, clLocalFileSystemWatcher* watcher = nullptr)
        : m_filepath{filepath}
        , m_handler{handler}
    {
        m_watcher = watcher == nullptr ? &clLocalFileSystemWatcher::Get() : watcher;
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

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_NOT_FOUND, clFileSystemEvent);
