//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : webupdatethread.cpp
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

#include "autoversion.h"
#include "file_logger.h"
#include "json_node.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "webupdatethread.h"
#include <wx/tokenzr.h>
#include <wx/url.h>

wxDEFINE_EVENT(wxEVT_CMD_NEW_VERSION_AVAILABLE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_CMD_VERSION_UPTODATE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_CMD_VERSION_CHECK_ERROR, wxCommandEvent);

static const size_t DLBUFSIZE = 4096;

struct CodeLiteVersion
{
    wxString m_os;
    wxString m_codename;
    wxString m_arch;
    wxString m_url;
    int m_version;
    bool m_isReleaseVersion;
    CodeLiteVersion(const JSONElement& json)
        : m_version(wxNOT_FOUND)
        , m_isReleaseVersion(false)
    {
        m_os = json.namedObject("os").toString();
        m_codename = json.namedObject("codename").toString();
        m_arch = json.namedObject("arch").toString();
        m_url = json.namedObject("url").toString();
        m_version = json.namedObject("version").toInt();
        m_isReleaseVersion = json.namedObject("isRelease").toBool(m_isReleaseVersion);
    }

    void Print() { clDEBUG() << "--->" << m_os << "," << m_codename << "," << m_arch << "," << m_version << clEndl; }

    /**
     * @brief return true of this codelite version object is newer than the provided input
     */
    bool IsNewer(const wxString& os, const wxString& codename, const wxString& arch) const
    {
        wxString strVersionNumer = CODELITE_VERSION_STRING;
        strVersionNumer.Replace(".", "");
        long nVersionNumber = -1;
        strVersionNumer.ToCLong(&nVersionNumber);

        if((m_os == os) && (m_arch == arch) && (m_codename == codename)) {
            bool res = (m_version > nVersionNumber);
            if(res) {
                clDEBUG() << "Found new version!" << clEndl;
            }
            return res;
        }
        return false;
    }

    bool IsReleaseVersion() const { return m_isReleaseVersion; }
    const wxString& GetArch() const { return m_arch; }
    const wxString& GetCodename() const { return m_codename; }
    const wxString& GetOs() const { return m_os; }
    const wxString& GetUrl() const { return m_url; }
    int GetVersion() const { return m_version; }
};

WebUpdateJob::WebUpdateJob(wxEvtHandler* parent, bool userRequest, bool onlyRelease)
    : m_parent(parent)
    , m_socket(this)
    , m_userRequest(userRequest)
    , m_onlyRelease(onlyRelease)
    , m_eventsConnected(true)
    , m_testSocket(this)
    , m_testingConnection(false)
{
    Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &WebUpdateJob::OnConnected, this);
    Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &WebUpdateJob::OnConnectionLost, this);
    Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &WebUpdateJob::OnConnectionError, this);
    Bind(wxEVT_ASYNC_SOCKET_ERROR, &WebUpdateJob::OnSocketError, this);
    Bind(wxEVT_ASYNC_SOCKET_INPUT, &WebUpdateJob::OnSocketInput, this);
}

WebUpdateJob::~WebUpdateJob() { Clear(); }

void WebUpdateJob::ParseFile()
{
    wxString os, arch, codename;
    GetPlatformDetails(os, codename, arch);

    clDEBUG() << "Current platform details:" << os << "," << codename << "," << arch << "," << CODELITE_VERSION_STRING
              << clEndl;
    JSONRoot root(m_dataRead);
    JSONElement platforms = root.toElement().namedObject("platforms");

    int count = platforms.arraySize();
    for(int i = 0; i < count; ++i) {
        CodeLiteVersion v(platforms.arrayItem(i));
        v.Print();
        if(!v.IsReleaseVersion() && m_onlyRelease) {
            // User wishes to be prompted for new releases only
            // skip weekly builds
            continue;
        }

        if(v.IsNewer(os, codename, arch)) {
            clDEBUG() << "A new version of CodeLite found" << clEndl;
            wxCommandEvent event(wxEVT_CMD_NEW_VERSION_AVAILABLE);
            event.SetClientData(new WebUpdateJobData("https://codelite.org/support.php", v.GetUrl(),
                                                     CODELITE_VERSION_STRING, "", false, true));
            m_parent->AddPendingEvent(event);
            return;
        }
    }

    // If we got here, then the version is up to date
    wxCommandEvent event(wxEVT_CMD_VERSION_UPTODATE);
    m_parent->AddPendingEvent(event);
}

void WebUpdateJob::GetPlatformDetails(wxString& os, wxString& codename, wxString& arch) const
{
    static wxStringMap_t linuxVariants;

#ifdef __WXMSW__
    os = "msw";
    codename = "Windows";
#ifndef NDEBUG
    os << "-dbg";
#endif

#ifdef _WIN64
    arch = "x86_64";
#else
    arch = "i386";
#endif
#elif defined(__WXOSX__)
    os = "osx";
    arch = "x86_64";
    codename = "10.8";
#else
    os = "linux";
    wxFFile fp("/etc/issue", "rb");
    wxString content;
    if(fp.IsOpened()) {
        fp.ReadAll(&content, wxConvUTF8);
        fp.Close();
    }
    // Test for common code names that we support on Linux
    if(content.Contains("Ubuntu 14.04")) {
        codename = "Ubuntu 14.04";
    } else if(content.Contains("Ubuntu 16.04")) {
        codename = "Ubuntu 16.04";
    } else if(content.Contains("Ubuntu 18.04")) {
        codename = "Ubuntu 18.04";
    } else if(content.Contains("Debian GNU/Linux 8")) {
        codename = "Debian GNU/Linux 8";
    } else {
        codename = "others";
    }
#if __LP64__
    arch = "x86_64";
#else
    arch = "i386";
#endif
#endif
}

void WebUpdateJob::OnConnected(clCommandEvent& e)
{
    if(m_testingConnection) {
        // Now that we confirmed that we have connectivity, run the check
        m_testSocket.Disconnect();
        m_testingConnection = false;
        // Now do the real check
        RealCheck();

    } else {
        wxString message;
        message << "GET /packages.json HTTP/1.1\r\n"
                << "Host: www.codelite.org\r\n"
                << "\r\n";
        m_socket.Send(message);
    }
}

void WebUpdateJob::OnConnectionLost(clCommandEvent& e)
{
    clDEBUG() << "WebUpdateJob: Connection lost:" << e.GetString() << clEndl;
    m_socket.Disconnect();
    m_testSocket.Disconnect();
    m_testingConnection = false;
    NotifyError("Connection lost:" + e.GetString());
}

void WebUpdateJob::OnConnectionError(clCommandEvent& e)
{
    clDEBUG() << "WebUpdateJob: Connection error:" << e.GetString() << clEndl;
    m_socket.Disconnect();
    m_testSocket.Disconnect();
    m_testingConnection = false;
    NotifyError("Connection error:" + e.GetString());
}

void WebUpdateJob::OnSocketError(clCommandEvent& e)
{
    clDEBUG() << "WebUpdateJob: socket error:" << e.GetString() << clEndl;
    m_socket.Disconnect();
    m_testSocket.Disconnect();
    m_testingConnection = false;
    NotifyError("Socker error:" + e.GetString());
}

void WebUpdateJob::OnSocketInput(clCommandEvent& e)
{
    m_dataRead << e.GetString();
    int where = m_dataRead.Find("\r\n\r\n");
    if(where != wxNOT_FOUND) {
        wxString headers = m_dataRead.Mid(0, where);
        m_dataRead = m_dataRead.Mid(where + 4);
        ParseFile();
        Clear();
    }
}

void WebUpdateJob::Check() { CheckConnectivity(); }

void WebUpdateJob::Clear()
{
    if(m_eventsConnected) {
        Unbind(wxEVT_ASYNC_SOCKET_CONNECTED, &WebUpdateJob::OnConnected, this);
        Unbind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &WebUpdateJob::OnConnectionLost, this);
        Unbind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &WebUpdateJob::OnConnectionError, this);
        Unbind(wxEVT_ASYNC_SOCKET_ERROR, &WebUpdateJob::OnSocketError, this);
        Unbind(wxEVT_ASYNC_SOCKET_INPUT, &WebUpdateJob::OnSocketInput, this);
        m_eventsConnected = false;
    }
    m_socket.Disconnect();
    m_testSocket.Disconnect();
}

void WebUpdateJob::CheckConnectivity()
{
    m_testingConnection = true;
    m_testSocket.ConnectNonBlocking("tcp://79.143.189.67:80");
}

void WebUpdateJob::RealCheck() { m_socket.ConnectNonBlocking("tcp://79.143.189.67:80"); }

void WebUpdateJob::NotifyError(const wxString& errmsg)
{
    wxCommandEvent event(wxEVT_CMD_VERSION_CHECK_ERROR);
    event.SetString(errmsg);
    m_parent->AddPendingEvent(event);
}
