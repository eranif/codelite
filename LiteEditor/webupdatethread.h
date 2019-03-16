//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : webupdatethread.h
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

#ifndef __webupdatethread__
#define __webupdatethread__

#include "SocketAPI/clSocketClientAsync.h"
#include "cl_command_event.h"

wxDECLARE_EVENT(wxEVT_CMD_NEW_VERSION_AVAILABLE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_CMD_VERSION_UPTODATE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_CMD_VERSION_CHECK_ERROR, wxCommandEvent);

class WebUpdateJobData
{
    wxString m_url;
    wxString m_curVersion;
    wxString m_newVersion;
    bool m_upToDate;
    wxString m_releaseNotes;
    bool m_showMessage;

public:
    WebUpdateJobData(const wxString& url, const wxString& releaseNotes, wxString curVersion, wxString newVersion,
                     bool upToDate, bool showMessage)
        : m_url(url.c_str())
        , m_curVersion(curVersion)
        , m_newVersion(newVersion)
        , m_upToDate(upToDate)
        , m_releaseNotes(releaseNotes)
        , m_showMessage(showMessage)
    {
    }

    ~WebUpdateJobData() {}

    const wxString& GetCurVersion() const { return m_curVersion; }
    const wxString& GetNewVersion() const { return m_newVersion; }
    const wxString& GetUrl() const { return m_url; }

    bool IsUpToDate() const { return m_upToDate; }

    void SetReleaseNotes(const wxString& releaseNotes) { this->m_releaseNotes = releaseNotes; }
    const wxString& GetReleaseNotes() const { return m_releaseNotes; }
    bool GetShowMessage() const { return m_showMessage; }
};

class WebUpdateJob : public wxEvtHandler
{
    wxEvtHandler* m_parent;
    clAsyncSocket::Ptr_t m_socket;
    wxString m_dataRead;
    bool m_userRequest;
    bool m_onlyRelease;

protected:
    /**
     * @brief get the current platform details
     */
    void GetPlatformDetails(wxString& os, wxString& codename, wxString& arch) const;
    void OnConnected(clCommandEvent& e);
    void OnConnectionLost(clCommandEvent& e);
    void OnConnectionError(clCommandEvent& e);
    void OnSocketError(clCommandEvent& e);
    void OnSocketInput(clCommandEvent& e);

    void Clear();
    void NotifyError(const wxString& errmsg);

public:
    WebUpdateJob(wxEvtHandler* parent, bool userRequest, bool onlyRelease);
    virtual ~WebUpdateJob();
    void ParseFile();
    bool IsUserRequest() const { return m_userRequest; }
    void Check();
};
#endif // __webupdatethread__
