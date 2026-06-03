#pragma once

#include "clFileSystemEvent.h"
#include "codelite_exports.h"

#include <map>
#include <wx/filename.h>
#include <wx/timer.h>

struct WXDLLIMPEXP_CL clWatchedFile {
    wxFileName filename;
    time_t lastModified;
    size_t file_size;
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
    virtual ~clLocalFileSystemWatcher();

    /**
     * @brief Set the event handler that will receive wxEVT_FILE_MODIFIED / wxEVT_FILE_NOT_FOUND events.
     * @param owner the wxEvtHandler to notify, or nullptr
     */
    void SetOwner(wxEvtHandler* owner) { this->m_owner = owner; }

    /**
     * @brief Get the currently assigned event handler.
     * @return the wxEvtHandler that receives file-system events, or nullptr
     */
    wxEvtHandler* GetOwner() { return m_owner; }

    /**
     * @brief Set a single file to watch. Clears any previously watched files.
     * @param filename full path to the file to watch
     */
    void SetFile(const wxString& filename);

    /**
     * @brief Add a file to the watch list. Previously watched files are preserved.
     * @param filename full path to the file to watch. If the file does not exist, it is ignored.
     */
    void AddFile(const wxString& filename);

    /**
     * @brief remove file from the watch list
     */
    void RemoveFile(const wxFileName& filename);

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
    wxEvtHandler* m_owner{nullptr};
    WatchedFilesMap m_files;
    wxTimer m_timer;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_NOT_FOUND, clFileSystemEvent);
