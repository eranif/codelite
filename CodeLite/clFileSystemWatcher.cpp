#include "clFileSystemWatcher.h"

#include "file_logger.h"
#include "fileutils.h"

#include <set>

wxDEFINE_EVENT(wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDEFINE_EVENT(wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

// In milliseconds
constexpr int FILE_CHECK_INTERVAL = 250;

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

void clLocalFileSystemWatcher::SetFile(clWatchedFile&& file)
{
    if (!file.IsOk()) {
        return;
    }
    m_files.clear();
    AddFile(std::move(file));
}

void clLocalFileSystemWatcher::Start()
{
    Stop();
    m_timer.StartOnce(FILE_CHECK_INTERVAL);
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
        const wxFileName fn = f.m_filename;
        const wxString fullpath = fn.GetFullPath();
        if (!fn.Exists()) {

            // fire file not found event
            if (f.m_owner != nullptr) {
                clFileSystemEvent evt(wxEVT_FILE_NOT_FOUND);
                evt.SetPath(fullpath);
                f.m_owner->AddPendingEvent(evt);
            } else {
                clERROR() << "(NotFound) null find handler for file:" << fullpath << endl;
            }
            // add the missing file to a set
            nonExistingFiles.insert(fullpath);

        } else {
            auto old_modified_time = f.m_lastModified;
            auto old_size = f.m_fileSize;
            auto curr_modified_time = FileUtils::GetFileModificationTime(fn);
            auto file_size = FileUtils::GetFileSize(fn);

            if (old_modified_time != curr_modified_time || old_size != file_size) {
                // Fire a modified event
                if (f.m_owner != nullptr) {
                    clFileSystemEvent evt(wxEVT_FILE_MODIFIED);
                    evt.SetPath(fullpath);
                    f.m_owner->AddPendingEvent(evt);
                } else {
                    clERROR() << "(Modified) null find handler for file:" << fullpath << endl;
                }
            }
            f.m_fileSize = file_size;
            f.m_lastModified = curr_modified_time;
        }
    }

    // Remove the non existing files
    for (const wxString& fn : nonExistingFiles) {
        m_files.erase(fn);
    }
    m_timer.StartOnce(FILE_CHECK_INTERVAL);
}

void clLocalFileSystemWatcher::RemoveFile(const wxString& filename)
{
    if (m_files.contains(filename)) {
        clDEBUG() << "Removing file:" << filename << "from watched list" << endl;
        m_files.erase(filename);
    }
}

bool clLocalFileSystemWatcher::IsRunning() const { return m_timer.IsRunning(); }

void clLocalFileSystemWatcher::AddFile(clWatchedFile&& file)
{
    if (!file.IsOk()) {
        return;
    }

    clDEBUG() << "Add file:" << file.m_filename << "to the watched list" << endl;

    file.m_fileSize = FileUtils::GetFileSize(file.m_filename);
    file.m_lastModified = FileUtils::GetFileModificationTime(file.m_filename);
    m_files.erase(file.m_filename);
    m_files.insert(std::make_pair(file.m_filename, std::move(file)));
}

clLocalFileSystemWatcher& clLocalFileSystemWatcher::Get()
{
    static clLocalFileSystemWatcher watcher;
    return watcher;
}