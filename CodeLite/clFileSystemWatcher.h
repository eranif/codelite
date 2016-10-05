#ifndef CLFILESYSTEMWATCHER_H
#define CLFILESYSTEMWATCHER_H

#include "codelite_exports.h"
#include "clFileSystemEvent.h"
#include <map>
#include <wx/timer.h>
#include <wx/filename.h>

#ifdef __WXMSW__
#define CL_FSW_USE_TIMER 1
#else
#define CL_FSW_USE_TIMER 1
#endif

#if !CL_FSW_USE_TIMER
#include <wx/fswatcher.h>
#endif

class WXDLLIMPEXP_CL clFileSystemWatcher : public wxEvtHandler
{
public:
    struct File {
        wxFileName filename;
        time_t lastModified;
        size_t file_size;
        typedef std::map<wxString, File> Map_t;
    };

    wxEvtHandler* m_owner;
#if CL_FSW_USE_TIMER
    clFileSystemWatcher::File::Map_t m_files;
    wxTimer* m_timer;
#else
    wxFileSystemWatcher m_watcher;
    wxFileName m_watchedFile;
#endif

public:
    typedef wxSharedPtr<clFileSystemWatcher> Ptr_t;

protected:
#if CL_FSW_USE_TIMER
    void OnTimer(wxTimerEvent& event);
#else
    void OnFileModified(wxFileSystemWatcherEvent& event);
#endif

public:
    clFileSystemWatcher();
    virtual ~clFileSystemWatcher();

    void SetOwner(wxEvtHandler* owner) { this->m_owner = owner; }
    wxEvtHandler* GetOwner() { return m_owner; }

    /**
     * @brief add a file to watch
     */
    void SetFile(const wxFileName& filename);

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
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

#endif // CLFILESYSTEMWATCHER_H
