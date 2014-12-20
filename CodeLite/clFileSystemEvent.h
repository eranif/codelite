#ifndef CLFILESYSTEMEVENT_H
#define CLFILESYSTEMEVENT_H

#include "cl_command_event.h" // Base class: clCommandEvent

class WXDLLIMPEXP_CL clFileSystemEvent : public clCommandEvent
{
protected:
    wxString m_path;

public:
    clFileSystemEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clFileSystemEvent(const clFileSystemEvent& event);
    clFileSystemEvent& operator=(const clFileSystemEvent& src);

    virtual ~clFileSystemEvent();
    virtual wxEvent* Clone() const { return new clFileSystemEvent(*this); }
    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetPath() const { return m_path; }
};

typedef void (wxEvtHandler::*clFileSystemEventFunction)(clFileSystemEvent&);
#define clFileSystemEventHandler(func) wxEVENT_HANDLER_CAST(clFileSystemEventFunction, func)

#endif // CLFILESYSTEMEVENT_H
