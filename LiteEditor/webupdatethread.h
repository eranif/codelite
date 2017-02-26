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

#include "job.h"

extern const wxEventType wxEVT_CMD_NEW_VERSION_AVAILABLE;
extern const wxEventType wxEVT_CMD_VERSION_UPTODATE;

class WebUpdateJobData
{
    wxString m_url;
    wxString m_curVersion;
    wxString m_newVersion;
    bool m_upToDate;
    wxString m_releaseNotes;
    bool m_showMessage;

public:
    WebUpdateJobData(const wxString& url,
                     const wxString& releaseNotes,
                     wxString curVersion,
                     wxString newVersion,
                     bool upToDate,
                     bool showMessage)
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

class WebUpdateJob : public Job
{
    wxString m_dataRead;
    bool m_userRequest;
    bool m_onlyRelease;
    
protected:
    /**
     * @brief get the current platform details
     */
    void GetPlatformDetails(wxString& os, wxString& codename, wxString& arch) const;

public:
    WebUpdateJob(wxEvtHandler* parent, bool userRequest, bool onlyRelease);
    virtual ~WebUpdateJob();
    void ParseFile();

    static size_t WriteData(void* buffer, size_t size, size_t nmemb, void* obj);

public:
    virtual void Process(wxThread* thread);
};
#endif // __webupdatethread__
