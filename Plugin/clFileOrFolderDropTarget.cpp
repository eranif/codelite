#include "clFileOrFolderDropTarget.h"

#include "cl_command_event.h"
#include "codelite_events.h"

#include <wx/arrstr.h>
#include <wx/filename.h>

clFileOrFolderDropTarget::clFileOrFolderDropTarget(wxEvtHandler* eventSink)
    : m_sink(eventSink)
{
}

clFileOrFolderDropTarget::~clFileOrFolderDropTarget() {}

bool clFileOrFolderDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    wxUnusedVar(x);
    wxUnusedVar(y);

    // Split the list into 2: files and folders
    wxArrayString files, folders;
    for(size_t i = 0; i < filenames.size(); ++i) {
        if(wxFileName::DirExists(filenames.Item(i))) {
            folders.Add(filenames.Item(i));
        } else {
            files.Add(filenames.Item(i));
        }
    }

    if(m_sink) {
        // fire the events, folders before files
        if(!folders.IsEmpty()) {
            clCommandEvent eventFolders(wxEVT_DND_FOLDER_DROPPED);
            eventFolders.SetStrings(folders);
            m_sink->AddPendingEvent(eventFolders);
        }

        if(!files.IsEmpty()) {
            clCommandEvent eventFiles(wxEVT_DND_FILE_DROPPED);
            eventFiles.SetStrings(files);
            m_sink->AddPendingEvent(eventFiles);
        }
    }
    return true;
}
