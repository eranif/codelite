//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clFileSystemEvent.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLFILESYSTEMEVENT_H
#define CLFILESYSTEMEVENT_H

#include "cl_command_event.h" // Base class: clCommandEvent

class WXDLLIMPEXP_CL clFileSystemEvent : public clCommandEvent
{
    enum clFileSystemEventFlags {
        IS_REMOTE = (1 << 0),
    };

protected:
    wxString m_path;
    wxString m_newpath;
    wxArrayString m_paths;
    size_t m_flags = 0;

protected:
    void SetBit(bool b, size_t bit)
    {
        if(b) {
            m_flags |= bit;
        } else {
            m_flags &= ~bit;
        }
    }

    bool HasBit(size_t bit) const { return m_flags & bit; }

public:
    clFileSystemEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clFileSystemEvent(const clFileSystemEvent& event);
    clFileSystemEvent& operator=(const clFileSystemEvent& src);

    virtual ~clFileSystemEvent();
    virtual wxEvent* Clone() const { return new clFileSystemEvent(*this); }
    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetPath() const { return m_path; }
    void SetNewpath(const wxString& newpath) { this->m_newpath = newpath; }
    const wxString& GetNewpath() const { return m_newpath; }
    void SetPaths(const wxArrayString& paths) { this->m_paths = paths; }
    const wxArrayString& GetPaths() const { return m_paths; }
    wxArrayString& GetPaths() { return m_paths; }

    void SetIsRemoteFile(bool remote) { SetBit(IS_REMOTE, remote); };
    bool IsRemoteFile() const { return HasBit(IS_REMOTE); }
};

typedef void (wxEvtHandler::*clFileSystemEventFunction)(clFileSystemEvent&);
#define clFileSystemEventHandler(func) wxEVENT_HANDLER_CAST(clFileSystemEventFunction, func)

#endif // CLFILESYSTEMEVENT_H
