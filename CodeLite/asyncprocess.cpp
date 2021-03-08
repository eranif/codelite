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

//#include "asyncprocess.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "file_logger.h"
#include "ssh_account_info.h"
#include <wx/arrstr.h>
#include <wx/string.h>

#ifdef __WXMSW__
#include "winprocess_impl.h"
#else
#include "unixprocess_impl.h"
#endif

static wxArrayString __WrapInShell(const wxArrayString& args)
{
    wxString cmd = wxJoin(args, ' ', 0);
    wxArrayString command;

#ifdef __WXMSW__
    wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if(!shell) {
        shell = (wxChar*)wxT("CMD.EXE");
    }
    command.Add(shell);
    command.Add("/C");
    command.Add(cmd);
#else
    command.Add("/bin/sh");
    command.Add("-c");
    command.Add(cmd);
#endif
    return command;
}

static wxArrayString __AddSshCommand(const wxArrayString& args, const wxString& wd, const wxString& sshAccountName)
{
    // we accept both SSHAccountInfo* & wxString* with the account name
    if(sshAccountName.empty()) {
        clERROR() << "CreateAsyncProcess: no ssh account name provided" << endl;
        return args;
    }
    wxArrayString a;

    auto accountInfo = SSHAccountInfo::LoadAccount(sshAccountName);
    if(accountInfo.GetAccountName().empty()) {
        clERROR() << "CreateAsyncProcess: could not locate ssh account:" << sshAccountName << endl;
        return args;
    }

    a.Add("ssh");
    a.Add(accountInfo.GetUsername() + "@" + accountInfo.GetHost());
    a.Add("-p");
    a.Add(wxString() << accountInfo.GetPort());
    wxString sshEnv;
    if(::wxGetEnv("SSH_OPTIONS", &sshEnv)) {
        wxArrayString sshOptionsArr = StringUtils::BuildArgv(sshEnv);
        a.insert(a.end(), sshOptionsArr.begin(), sshOptionsArr.end());
    }

    // add the command as a single string
    wxArrayString* p_args = const_cast<wxArrayString*>(&args);
    wxArrayString tmpargs;
    if(!wd.empty()) {
        tmpargs.Add("cd");
        tmpargs.Add(wd);
        tmpargs.Add("&&");
        tmpargs.insert(tmpargs.end(), args.begin(), args.end());
        p_args = &tmpargs;
    }

    wxString oneLiner = wxJoin(*p_args, ' ', 0);
#ifdef __WXMSW__
    oneLiner.Prepend("\"").Append("\"");
#else
    oneLiner.Replace("\"", "\\\""); // escape any double quotes
#endif
    a.Add(oneLiner);
    return a;
}

static void __StripArgs(wxArrayString& args)
{
#if defined(__WXOSX__) || defined(__WXGTK__)
    for(wxString& arg : args) {
        arg.Trim().Trim(false);
        if(arg.StartsWith("'") || arg.StartsWith("\"")) {
            arg.Remove(0, 1);
        }

        if(arg.EndsWith("'") || arg.EndsWith("\"")) {
            arg.RemoveLast();
        }
    }
#endif
}

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxArrayString& args, size_t flags, const wxString& workingDir,
                             const clEnvList_t* env, const wxString& sshAccountName)
{
    clEnvironment e(env);
    wxArrayString c = args;

    if(flags & IProcessWrapInShell) {
        // wrap the command in OS specific terminal
        c = __WrapInShell(c);
    }

    if(flags & IProcessCreateSSH) {
        c = __AddSshCommand(c, workingDir, sshAccountName);
    }

    // needed on linux where fork does not require the extra quoting
    __StripArgs(c);

    clDEBUG() << "CreateAsyncProcess called with:" << c << endl;

#ifdef __WXMSW__
    return WinProcessImpl::Execute(parent, c, flags, workingDir);
#else
    return UnixProcessImpl::Execute(parent, c, flags, workingDir);
#endif
}

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDir,
                             const clEnvList_t* env, const wxString& sshAccountName)
{
    return CreateAsyncProcess(parent, StringUtils::BuildArgv(cmd), flags, workingDir, env, sshAccountName);
}

IProcess* CreateAsyncProcessCB(wxEvtHandler* parent, IProcessCallback* cb, const wxString& cmd, size_t flags,
                               const wxString& workingDir, const clEnvList_t* env)
{
    clEnvironment e(env);
#ifdef __WXMSW__
    return WinProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#endif
}

IProcess* CreateSyncProcess(const wxString& cmd, size_t flags, const wxString& workingDir, const clEnvList_t* env)
{
    clEnvironment e(env);
#ifdef __WXMSW__
    return WinProcessImpl::Execute(NULL, cmd, flags | IProcessCreateSync, workingDir);
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
            if(!buff.IsEmpty() && !buffErr.IsEmpty()) {
                output << "\n";
            }
            output << buffErr;
        }
    } else {
        // Just wait for the process to terminate in a busy loop
        while(IsAlive()) {
            wxThread::Sleep(10);
        }
    }
}
