#include "clFileSystemWatcher.h"
#include <algorithm>
#include <set>

wxDEFINE_EVENT(wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDEFINE_EVENT(wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

// In milliseconds
#define FILE_CHECK_INTERVAL 500

clFileSystemWatcher::clFileSystemWatcher()
    : m_timer(NULL)
    , m_owner(NULL)
{
    Bind(wxEVT_TIMER, &clFileSystemWatcher::OnTimer, this);
}

clFileSystemWatcher::~clFileSystemWatcher()
{
    Stop();
    Unbind(wxEVT_TIMER, &clFileSystemWatcher::OnTimer, this);
}

void clFileSystemWatcher::AddFile(const wxFileName& filename)
{
    if(filename.Exists() && (m_files.count(filename.GetFullPath()) == 0)) {
        File f;
        f.filename = filename;
        f.lastModified = filename.GetModificationTime().GetTicks();
        m_files.insert(std::make_pair(filename.GetFullPath(), f));
    }
}

void clFileSystemWatcher::Start()
{
    if(m_timer) {
        return;
    }
    
    m_timer = new wxTimer(this);
    m_timer->Start(FILE_CHECK_INTERVAL, true);
}

void clFileSystemWatcher::Stop()
{
    if(m_timer) {
        m_timer->Stop();
    }
    wxDELETE(m_timer);
}

void clFileSystemWatcher::Clear() { m_files.clear(); }

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

            time_t lastSampled = f.lastModified;
            time_t modTime = fn.GetModificationTime().GetTicks();

            if(modTime != lastSampled) {
                // Fire a modified event
                if(GetOwner()) {
                    clFileSystemEvent evt(wxEVT_FILE_MODIFIED);
                    evt.SetPath(fn.GetFullPath());
                    GetOwner()->AddPendingEvent(evt);
                }
            }
            // Always update the last modified timestamp
            File updatdFile = f;
            updatdFile.lastModified = modTime;
            m_files[fn.GetFullPath()] = updatdFile;
        }
    });

    // Remove the non existing files
    std::for_each(nonExistingFiles.begin(), nonExistingFiles.end(), [&](const wxString& fn) { m_files.erase(fn); });

    if(m_timer) {
        m_timer->Start(FILE_CHECK_INTERVAL, true);
    }
}

void clFileSystemWatcher::RemoveFile(const wxFileName& filename)
{
    if(m_files.count(filename.GetFullPath())) {
        m_files.erase(filename.GetFullPath());
    }
}
