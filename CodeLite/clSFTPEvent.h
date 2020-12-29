//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clSFTPEvent.h
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

#ifndef CLSFTPEVENT_H
#define CLSFTPEVENT_H

#include "cl_command_event.h" // Base class: clCommandEvent

class WXDLLIMPEXP_CL clSFTPEvent : public clCommandEvent
{
protected:
    wxString m_account;
    wxString m_localFile;
    wxString m_remoteFile;
    wxString m_newRemoteFile;
    int m_selectionStart = wxNOT_FOUND;
    int m_selectionEnd = wxNOT_FOUND;

public:
    clSFTPEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clSFTPEvent(const clSFTPEvent& event);
    clSFTPEvent& operator=(const clSFTPEvent& src);

    virtual ~clSFTPEvent();
    virtual wxEvent* Clone() const { return new clSFTPEvent(*this); }

    void SetAccount(const wxString& account) { this->m_account = account; }
    void SetLocalFile(const wxString& localFile) { this->m_localFile = localFile; }
    void SetRemoteFile(const wxString& remoteFile) { this->m_remoteFile = remoteFile; }
    const wxString& GetAccount() const { return m_account; }
    const wxString& GetLocalFile() const { return m_localFile; }
    const wxString& GetRemoteFile() const { return m_remoteFile; }
    void SetNewRemoteFile(const wxString& newRemoteFile) { this->m_newRemoteFile = newRemoteFile; }
    const wxString& GetNewRemoteFile() const { return m_newRemoteFile; }
    void SetSelectionEnd(int selectionEnd) { this->m_selectionEnd = selectionEnd; }
    void SetSelectionStart(int selectionStart) { this->m_selectionStart = selectionStart; }
    int GetSelectionEnd() const { return m_selectionEnd; }
    int GetSelectionStart() const { return m_selectionStart; }
};

typedef void (wxEvtHandler::*clSFTPEventFunction)(clSFTPEvent&);
#define clSFTPEventHandler(func) wxEVENT_HANDLER_CAST(clSFTPEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SFTP_SAVE_FILE, clSFTPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SFTP_RENAME_FILE, clSFTPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SFTP_DELETE_FILE, clSFTPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SFTP_OPEN_FILE, clSFTPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SFTP_SESSION_OPENED, clSFTPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SFTP_SESSION_CLOSED, clSFTPEvent);
#endif // CLSFTPEVENT_H
