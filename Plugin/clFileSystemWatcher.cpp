#include "clFileSystemWatcher.h"

#include "file_logger.h"
#include "fileutils.h"

#include <set>

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

wxDEFINE_EVENT(wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDEFINE_EVENT(wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

// In milliseconds
constexpr int FILE_CHECK_INTERVAL = 250;

clFileSystemWatcher::clFileSystemWatcher()
{
    m_timer.SetOwner(this);
    Bind(wxEVT_TIMER, &clFileSystemWatcher::OnTimer, this, m_timer.GetId());
}

clFileSystemWatcher::~clFileSystemWatcher()
{
    Stop();
    Unbind(wxEVT_TIMER, &clFileSystemWatcher::OnTimer, this, m_timer.GetId());
}

void clFileSystemWatcher::SetFile(clWatchedFile&& file)
{
    if (!file.IsOk()) {
        return;
    }
    m_files.clear();
    AddFile(std::move(file));
}

void clFileSystemWatcher::Start()
{
    Stop();
    m_timer.StartOnce(FILE_CHECK_INTERVAL);
}

void clFileSystemWatcher::Stop()
{
    if (m_timer.IsRunning()) {
        m_timer.Stop();
    }
}

void clFileSystemWatcher::Clear()
{
    Stop();
    m_files.clear();
}

void clFileSystemWatcher::HandleLocalFiles()
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
}

void clFileSystemWatcher::HandleRemoteFiles()
{
#if USE_SFTP
    while (!m_remoteFiles.empty()) {
        // since operations are SLOW on remote, we remove the files from the
        // monitored files map, this will make sure that we won't check the same
        // file twice.
        auto it = m_remoteFiles.begin();
        auto file = it->second;
        m_remoteFiles.erase(it);

        clSFTPManager::Get().GetFileAttributes(
            file.m_filename,
            file.m_remoteAccount,
            [file = std::move(file), this](clStatusOr<clSFTPManager::Attribute> result) mutable {
                // Main thread here
                if (!result.ok()) {
                    if (StatusIsNotFound(result.status())) {
                        clFileSystemEvent evt{wxEVT_FILE_NOT_FOUND};
                        evt.SetPath(file.m_filename);
                        file.m_owner->AddPendingEvent(evt);
                    } else {
                        clWARNING() << result.error_message() << endl;
                    }
                    return;
                }

                // Change if the file was modified.
                auto old_modified_time = file.m_lastModified;
                auto old_size = file.m_fileSize;
                auto curr_modified_time = result.value().attr->GetLastModified();
                auto file_size = result.value().attr->GetSize();

                if (file.m_owner && (old_modified_time != curr_modified_time || old_size != file_size)) {
                    clFileSystemEvent evt{wxEVT_FILE_MODIFIED};
                    evt.SetPath(file.m_filename);
                    file.m_owner->AddPendingEvent(evt);
                }

                file.m_fileSize = file_size;
                file.m_lastModified = curr_modified_time;
                // Re-add the file
                m_remoteFiles.erase(file.m_filename);
                m_remoteFiles.insert(std::make_pair(file.m_filename, std::move(file)));
            });
    }
#endif
}

void clFileSystemWatcher::OnTimer(wxTimerEvent& event)
{
    wxUnusedVar(event);
    HandleLocalFiles();
    HandleRemoteFiles();
    m_timer.StartOnce(FILE_CHECK_INTERVAL);
}

void clFileSystemWatcher::RemoveFile(const wxString& filename)
{
    if (m_files.contains(filename)) {
        clDEBUG() << "Removing file:" << filename << "from watched list" << endl;
        m_files.erase(filename);
    }
}

bool clFileSystemWatcher::IsRunning() const { return m_timer.IsRunning(); }

void clFileSystemWatcher::AddFile(clWatchedFile&& file)
{
    if (!file.IsOk()) {
        return;
    }

    if (!file.IsRemote()) {
        // Local file
        clDEBUG() << "Add file:" << file.m_filename << "to the watched list" << endl;

        file.m_fileSize = FileUtils::GetFileSize(file.m_filename);
        file.m_lastModified = FileUtils::GetFileModificationTime(file.m_filename);
        m_files.erase(file.m_filename);
        m_files.insert(std::make_pair(file.m_filename, std::move(file)));
    } else {
        // Remote file
#if USE_SFTP
        clSFTPManager::Get().GetFileAttributes(
            file.m_filename,
            file.m_remoteAccount,
            [file = std::move(file), this](clStatusOr<clSFTPManager::Attribute> result) mutable {
                if (!result.ok()) {
                    clWARNING() << result.error_message() << endl;
                    return;
                }
                file.m_fileSize = result.value().attr->GetSize();
                file.m_lastModified = result.value().attr->GetLastModified();
                m_files.erase(file.m_filename);
                m_files.insert(std::make_pair(file.m_filename, std::move(file)));
            });
#endif
    }
}

clFileSystemWatcher& clFileSystemWatcher::Get()
{
    static clFileSystemWatcher watcher;
    return watcher;
}