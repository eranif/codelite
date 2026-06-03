#include "clFileSystemWatcher.h"

#include "fileutils.h"

#include <set>

wxDEFINE_EVENT(wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDEFINE_EVENT(wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

// In milliseconds
#define FILE_CHECK_INTERVAL 250

clLocalFileSystemWatcher::clLocalFileSystemWatcher()
{
    m_timer.SetOwner(this);
    Bind(wxEVT_TIMER, &clLocalFileSystemWatcher::OnTimer, this, m_timer.GetId());
}

clLocalFileSystemWatcher::~clLocalFileSystemWatcher()
{
    Stop();
    Unbind(wxEVT_TIMER, &clLocalFileSystemWatcher::OnTimer, this, m_timer.GetId());
}

void clLocalFileSystemWatcher::SetFile(const wxString& filename)
{
    m_files.clear();
    AddFile(filename);
}

void clLocalFileSystemWatcher::Start()
{
    Stop();
    m_timer.Start(FILE_CHECK_INTERVAL, true);
}

void clLocalFileSystemWatcher::Stop()
{
    if (m_timer.IsRunning()) {
        m_timer.Stop();
    }
}

void clLocalFileSystemWatcher::Clear()
{
    Stop();
    m_files.clear();
}

void clLocalFileSystemWatcher::OnTimer(wxTimerEvent& event)
{
    std::set<wxString> nonExistingFiles;
    for (auto& [_, f] : m_files) {
        const wxFileName& fn = f.filename;
        const wxString fullpath = fn.GetFullPath();
        if (!fn.Exists()) {

            // fire file not found event
            if (GetOwner()) {
                clFileSystemEvent evt(wxEVT_FILE_NOT_FOUND);
                evt.SetPath(fullpath);
                GetOwner()->AddPendingEvent(evt);
            }

            // add the missing file to a set
            nonExistingFiles.insert(fullpath);
        } else {
            auto old_modified_time = f.lastModified;
            auto old_size = f.file_size;
            auto curr_modified_time = FileUtils::GetFileModificationTime(fn);
            auto file_size = FileUtils::GetFileSize(fn);

            if (old_modified_time != curr_modified_time || old_size != file_size) {
                // Fire a modified event
                if (GetOwner()) {
                    clFileSystemEvent evt(wxEVT_FILE_MODIFIED);
                    evt.SetPath(fullpath);
                    GetOwner()->AddPendingEvent(evt);
                }
            }
            f.file_size = file_size;
            f.lastModified = curr_modified_time;
        }
    }

    // Remove the non existing files
    for (const wxString& fn : nonExistingFiles) {
        m_files.erase(fn);
    }
    m_timer.Start(FILE_CHECK_INTERVAL, true);
}

void clLocalFileSystemWatcher::RemoveFile(const wxFileName& filename)
{
    if (m_files.contains(filename.GetFullPath())) {
        m_files.erase(filename.GetFullPath());
    }
}

bool clLocalFileSystemWatcher::IsRunning() const { return m_timer.IsRunning(); }

void clLocalFileSystemWatcher::AddFile(const wxString& filename)
{
    if (!wxFileName::FileExists(filename)) {
        return;
    }

    clWatchedFile f;
    f.filename = filename;
    f.lastModified = FileUtils::GetFileModificationTime(filename);
    f.file_size = FileUtils::GetFileSize(filename);
    m_files.insert(std::make_pair(filename, f));
}
