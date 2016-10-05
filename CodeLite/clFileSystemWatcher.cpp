#include "clFileSystemWatcher.h"
#include <algorithm>
#include <set>
#include "fileutils.h"

wxDEFINE_EVENT(wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDEFINE_EVENT(wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

// In milliseconds
#define FILE_CHECK_INTERVAL 500

clFileSystemWatcher::clFileSystemWatcher()
    : m_owner(NULL)
#if CL_FSW_USE_TIMER
    , m_timer(NULL)
#endif
{
#if CL_FSW_USE_TIMER
    Bind(wxEVT_TIMER, &clFileSystemWatcher::OnTimer, this);
#else
    m_watcher.SetOwner(this);
    Bind(wxEVT_FSWATCHER, &clFileSystemWatcher::OnFileModified, this);
#endif
}

clFileSystemWatcher::~clFileSystemWatcher()
{
#if CL_FSW_USE_TIMER
    Stop();
    Unbind(wxEVT_TIMER, &clFileSystemWatcher::OnTimer, this);
#else
    m_watcher.RemoveAll();
    Unbind(wxEVT_FSWATCHER, &clFileSystemWatcher::OnFileModified, this);
#endif
}

void clFileSystemWatcher::SetFile(const wxFileName& filename)
{
#if CL_FSW_USE_TIMER
    if(filename.Exists()) {
        m_files.clear();
        File f;
        f.filename = filename;
        f.lastModified = FileUtils::GetFileModificationTime(filename);
        f.file_size = FileUtils::GetFileSize(filename);
        m_files.insert(std::make_pair(filename.GetFullPath(), f));
    }
#else
    m_watcher.RemoveAll();
    wxFileName dironly = filename;
    dironly.SetFullName(""); // must be directory only
    m_watcher.Add(dironly);
    m_watchedFile = filename; // but we keep the file name for later usage
#endif
}

void clFileSystemWatcher::Start()
{
#if CL_FSW_USE_TIMER
    Stop();

    m_timer = new wxTimer(this);
    m_timer->Start(FILE_CHECK_INTERVAL, true);
#else
#endif
}

void clFileSystemWatcher::Stop()
{
#if CL_FSW_USE_TIMER
    if(m_timer) {
        m_timer->Stop();
    }
    wxDELETE(m_timer);
#else
    m_watcher.RemoveAll();
#endif
}

void clFileSystemWatcher::Clear()
{
#if CL_FSW_USE_TIMER
    Stop();
    m_files.clear();
#else
    m_watcher.RemoveAll();
#endif
}

#if CL_FSW_USE_TIMER
void clFileSystemWatcher::OnTimer(wxTimerEvent& event)
{
    std::set<wxString> nonExistingFiles;
    std::for_each(m_files.begin(), m_files.end(), [&](const std::pair<wxString, clFileSystemWatcher::File>& p) {
        const File& f = p.second;
        const wxFileName& fn = f.filename;
        if(!fn.Exists()) {

            // fire file not found event
            if(GetOwner()) {
                clFileSystemEvent evt(wxEVT_FILE_NOT_FOUND);
                evt.SetPath(fn.GetFullPath());
                GetOwner()->AddPendingEvent(evt);
            }

            // add the missing file to a set
            nonExistingFiles.insert(fn.GetFullPath());
        } else {
            
#ifdef __WXMSW__
            size_t prev_value = f.file_size;
            size_t curr_value = FileUtils::GetFileSize(fn);
#else
            time_t prev_value = f.lastModified;
            time_t curr_value = FileUtils::GetFileModificationTime(fn);
#endif

            if(prev_value != curr_value) {
                // Fire a modified event
                if(GetOwner()) {
                    clFileSystemEvent evt(wxEVT_FILE_MODIFIED);
                    evt.SetPath(fn.GetFullPath());
                    GetOwner()->AddPendingEvent(evt);
                }
            }
#ifdef __WXMSW__
            File updatdFile = f;
            updatdFile.file_size = curr_value;
#else
            // Always update the last modified timestamp
            File updatdFile = f;
            updatdFile.lastModified = curr_value;
#endif
            m_files[fn.GetFullPath()] = updatdFile;
        }
    });

    // Remove the non existing files
    std::for_each(nonExistingFiles.begin(), nonExistingFiles.end(), [&](const wxString& fn) { m_files.erase(fn); });

    if(m_timer) {
        m_timer->Start(FILE_CHECK_INTERVAL, true);
    }
}
#endif

#if !CL_FSW_USE_TIMER
void clFileSystemWatcher::OnFileModified(wxFileSystemWatcherEvent& event)
{
    if(event.GetChangeType() == wxFSW_EVENT_MODIFY) {
        const wxFileName& modpath = event.GetPath();
        if(modpath == m_watchedFile) {
            if(GetOwner()) {
                clFileSystemEvent evt(wxEVT_FILE_MODIFIED);
                evt.SetPath(modpath.GetFullPath());
                GetOwner()->AddPendingEvent(evt);
            }
        }
    }
}
#endif

void clFileSystemWatcher::RemoveFile(const wxFileName& filename)
{
#if CL_FSW_USE_TIMER
    if(m_files.count(filename.GetFullPath())) {
        m_files.erase(filename.GetFullPath());
    }
#endif
}

bool clFileSystemWatcher::IsRunning() const
{
#if CL_FSW_USE_TIMER
    return m_timer;
#else
    return m_watcher.GetWatchedPathsCount();
#endif
}
