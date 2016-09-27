#ifndef CLFILESYSTEMWATCHER_H
#define CLFILESYSTEMWATCHER_H

#include "codelite_exports.h"
#include "clFileSystemEvent.h"
#include <map>
#include <wx/timer.h>

class WXDLLIMPEXP_CL clFileSystemWatcher : public wxEvtHandler
{
public:
    struct File {
        wxFileName filename;
        time_t lastModified;
        typedef std::map<wxString, File> Map_t;
    };
    
    clFileSystemWatcher::File::Map_t m_files;
    wxTimer* m_timer;
    wxEvtHandler* m_owner;

public:
    typedef wxSharedPtr<clFileSystemWatcher> Ptr_t;

protected:
    void OnTimer(wxTimerEvent& event);

public:
    clFileSystemWatcher();
    virtual ~clFileSystemWatcher();

    void SetOwner(wxEvtHandler* owner) { this->m_owner = owner; }
    wxEvtHandler* GetOwner() { return m_owner; }
    
    /**
     * @brief add a file to watch
     */
    void AddFile(const wxFileName& filename);
    
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
     * @brief clear the list of files to watch
     */
    void Clear();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_MODIFIED, clFileSystemEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_NOT_FOUND, clFileSystemEvent);

#endif // CLFILESYSTEMWATCHER_H
