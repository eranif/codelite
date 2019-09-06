//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : asyncprocess.cpp
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

class wxEvtHandler;
class IProcess;
#include <wx/string.h>
#include "macros.h"

#ifdef __WXMSW__
#include "winprocess_impl.h"
#else
#include "unixprocess_impl.h"
#endif

// Helper class for applying the environment before launching the process
class __Env
{
    const clEnvList_t* m_env = nullptr;
    wxStringMap_t m_oldEnv;

public:
    __Env(const clEnvList_t* env)
        : m_env(env)
    {
        if(m_env) {
            for(const auto& p : (*m_env)) {
                const wxString& name = p.first;
                const wxString& value = p.second;

                wxString oldValue;
                // If an environment variable with this name already exists, keep its old value
                // as we want to restore it later
                if(::wxGetEnv(name, &oldValue)) { m_oldEnv.insert({ name, oldValue }); }
                // set the new value
                ::wxSetEnv(name, value);
            }
        }
    }
    ~__Env()
    {
        if(m_env) {
            for(const auto& p : (*m_env)) {
                const wxString& name = p.first;
                if(m_oldEnv.count(name)) {
                    ::wxSetEnv(name, m_oldEnv[name]);
                } else {
                    ::wxUnsetEnv(name);
                }
            }
        }
        m_oldEnv.clear();
    }
};

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDir,
                             const clEnvList_t* env)
{
    __Env e(env);
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir);
#endif
}

IProcess* CreateAsyncProcessCB(wxEvtHandler* parent, IProcessCallback* cb, const wxString& cmd, size_t flags,
                               const wxString& workingDir, const clEnvList_t* env)
{
    __Env e(env);
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir, cb);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#endif
}

IProcess* CreateSyncProcess(const wxString& cmd, size_t flags, const wxString& workingDir, const clEnvList_t* env)
{
    __Env e(env);
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(NULL, cmd, errMsg, flags | IProcessCreateSync, workingDir);
#else
    return UnixProcessImpl::Execute(NULL, cmd, flags | IProcessCreateSync, workingDir);
#endif
}

// Static methods:
bool IProcess::GetProcessExitCode(int pid, int& exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);

    exitCode = 0;
    return true;
}

void IProcess::SetProcessExitCode(int pid, int exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);
}

void IProcess::WaitForTerminate(wxString& output)
{
    if(IsRedirect()) {
        wxString buff;
        wxString buffErr;
        while(Read(buff, buffErr)) {
            output << buff;
            if(!buff.IsEmpty() && !buffErr.IsEmpty()) { output << "\n"; }
            output << buffErr;
        }
    } else {
        // Just wait for the process to terminate in a busy loop
        while(IsAlive()) {
            wxThread::Sleep(10);
        }
    }
}
