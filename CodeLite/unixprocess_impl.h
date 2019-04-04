//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : unixprocess_impl.h
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

#ifndef __unixprocessimpl__
#define __unixprocessimpl__

#if defined(__WXMAC__) || defined(__WXGTK__)
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "codelite_exports.h"

class wxTerminal;
class WXDLLIMPEXP_CL UnixProcessImpl : public IProcess
{
    int m_readHandle;
    int m_stderrHandle = wxNOT_FOUND;
    int m_writeHandle;
    ProcessReaderThread* m_thr = nullptr;
    wxString m_tty;
    friend class wxTerminal;
private:
    void StartReaderThread();
    bool ReadFromFd(int fd, fd_set& rset, wxString& output);

public:
    UnixProcessImpl(wxEvtHandler* parent);
    virtual ~UnixProcessImpl();

    static IProcess* Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                             const wxString& workingDirectory = wxEmptyString, IProcessCallback* cb = NULL);

    void SetReadHandle(const int& readHandle) { this->m_readHandle = readHandle; }
    void SetWriteHandler(const int& writeHandler) { this->m_writeHandle = writeHandler; }
    int GetReadHandle() const { return m_readHandle; }
    int GetStderrHandle() const { return m_stderrHandle; }
    int GetWriteHandle() const { return m_writeHandle; }
    void SetStderrHandle(int stderrHandle) { this->m_stderrHandle = stderrHandle; }

    void SetTty(const wxString& tty) { this->m_tty = tty; }
    const wxString& GetTty() const { return m_tty; }

public:
    virtual void Cleanup();
    virtual bool IsAlive();
    virtual bool Read(wxString& buff, wxString& buffErr);
    virtual bool Write(const wxString& buff);
    virtual bool Write(const std::string& buff);
    virtual bool WriteRaw(const wxString& buff);
    virtual bool WriteRaw(const std::string& buff);
    virtual void Terminate();
    virtual bool WriteToConsole(const wxString& buff);
    virtual void Detach();
};
#endif //#if defined(__WXMAC )||defined(__WXGTK__)
#endif // __unixprocessimpl__
