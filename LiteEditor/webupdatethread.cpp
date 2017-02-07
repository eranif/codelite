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

#include "precompiled_header.h"
#include "autoversion.h"
#include <wx/url.h>
#include <wx/tokenzr.h>
#include "webupdatethread.h"
#include "procutils.h"
#include "json_node.h"
#include "file_logger.h"

const wxEventType wxEVT_CMD_NEW_VERSION_AVAILABLE = wxNewEventType();
const wxEventType wxEVT_CMD_VERSION_UPTODATE = wxNewEventType();

static const size_t DLBUFSIZE = 4096;

struct CodeLiteVersion {
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
    : Job(parent)
    , m_userRequest(userRequest)
    , m_onlyRelease(onlyRelease)
{
}

WebUpdateJob::~WebUpdateJob() {}

void WebUpdateJob::Process(wxThread* thread)
{
#ifndef __WXMSW__
    wxFileName fn(wxT("/tmp/codelite-packages.json"));
    wxString command;
#ifdef __WXMAC__
    command << wxT("curl http://www.codelite.org/packages.json  --output ") << fn.GetFullPath()
            << wxT(" > /dev/null 2>&1");
#else
    command << "wget http://www.codelite.org/packages.json --output-file=/dev/null -O " << fn.GetFullPath()
            << wxT(" > /dev/null 2>&1");
#endif
    {
        wxLogNull noLog;
        ::wxRemoveFile(fn.GetFullPath());
    }

    wxArrayString outputArr;
    ProcUtils::SafeExecuteCommand(command, outputArr);

    if(fn.FileExists()) {

        wxFFile fp(fn.GetFullPath(), wxT("rb"));
        if(fp.IsOpened()) {

            m_dataRead.Clear();
            fp.ReadAll(&m_dataRead, wxConvUTF8);

            ParseFile();
        }
    }

#else
    wxURL url(wxT("http://www.codelite.org/packages.json"));
    if(url.GetError() == wxURL_NOERR) {

        wxInputStream* in_stream = url.GetInputStream();
        if(!in_stream) {
            return;
        }
        bool shutdownRequest(false);

        unsigned char buffer[DLBUFSIZE + 1];
        do {

            in_stream->Read(buffer, DLBUFSIZE);
            size_t bytes_read = in_stream->LastRead();
            if(bytes_read > 0) {

                buffer[bytes_read] = 0;
                wxString buffRead((const char*)buffer, wxConvUTF8);
                m_dataRead.Append(buffRead);
            }

            // Check termination request from time to time
            if(thread->TestDestroy()) {
                shutdownRequest = true;
                break;
            }

        } while(!in_stream->Eof());

        if(shutdownRequest == false) {
            delete in_stream;
            ParseFile();
        }
    }
#endif
}

size_t WebUpdateJob::WriteData(void* buffer, size_t size, size_t nmemb, void* obj)
{
    WebUpdateJob* job = reinterpret_cast<WebUpdateJob*>(obj);
    if(job) {
        char* data = new char[size * nmemb + 1];
        memcpy(data, buffer, size * nmemb);
        data[size * nmemb] = 0;

        job->m_dataRead.Append(_U(data));
        delete[] data;
        return size * nmemb;
    }
    return static_cast<size_t>(-1);
}

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
            event.SetClientData(new WebUpdateJobData(
                "https://codelite.org/support.php", v.GetUrl(), CODELITE_VERSION_STRING, "", false, true));
            m_parent->AddPendingEvent(event);
            return;
        }
    }

    if(m_userRequest) {
        // If we got here, then the version is up to date
        wxCommandEvent event(wxEVT_CMD_VERSION_UPTODATE);
        m_parent->AddPendingEvent(event);
    }
}

void WebUpdateJob::GetPlatformDetails(wxString& os, wxString& codename, wxString& arch) const
{
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
