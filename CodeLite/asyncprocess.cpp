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
#include "asyncprocess.h"

#include "StringUtils.h"
#include "clTempFile.hpp"
#include "cl_command_event.h"
#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"
#include "ssh_account_info.h"

#include <wx/arrstr.h>
#include <wx/string.h>

#ifdef __WXMSW__
static bool shell_is_cmd = true;
#include "winprocess_impl.h"
#else
static bool shell_is_cmd = false;
#include "unixprocess_impl.h"
#endif
class __AsyncCallback : public wxEvtHandler
{
    std::function<void(const wxString&)> m_cb;
    wxString m_output;

public:
    __AsyncCallback(std::function<void(const wxString&)> cb)
        : m_cb(std::move(cb))
    {
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &__AsyncCallback::OnProcessTerminated, this);
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &__AsyncCallback::OnProcessOutput, this);
    }
    ~__AsyncCallback()
    {
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &__AsyncCallback::OnProcessTerminated, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &__AsyncCallback::OnProcessOutput, this);
    }

    void OnProcessOutput(clProcessEvent& event)
    {
        size_t new_len = m_output.length() + event.GetOutput().length();
        if(new_len > m_output.length()) {
            m_output.reserve(new_len);
            m_output << event.GetOutput();
        }
    }

    void OnProcessTerminated(clProcessEvent& event)
    {
        if(!event.GetOutput().empty()) {
            m_output << event.GetOutput();
        }
        // all the user callback
        m_cb(m_output);
        delete event.GetProcess();
        delete this; // we are no longer needed...
    }
};

static void __WrapSpacesForShell(wxString& str, size_t flags)
{
    str.Trim().Trim(false);
    auto tmpArgs = StringUtils::BuildArgv(str);
    if(tmpArgs.size() > 1) {
        if(!shell_is_cmd || (flags & IProcessCreateSSH)) {
            // escape any occurances of "
            str.Replace("\"", "\\\"");
        }
        str.Append("\"").Prepend("\"");
    }
}

static wxArrayString __WrapInShell(const wxArrayString& args, size_t flags)
{
    wxArrayString tmparr = args;
    for(wxString& arg : tmparr) {
        __WrapSpacesForShell(arg, flags);
    }

    wxString cmd = wxJoin(tmparr, ' ', 0);
    wxArrayString command;

    bool is_ssh = flags & IProcessCreateSSH;
    if(shell_is_cmd && !is_ssh) {
        wxString shell = wxGetenv("COMSPEC");
        if(shell.IsEmpty()) {
            shell = "CMD.EXE";
        }
        command.Add(shell);
        command.Add("/C");
        command.Add("\"" + cmd + "\"");

    } else {
        command.Add("/bin/sh");
        command.Add("-c");
        command.Add("'" + cmd + "'");
    }
    return command;
}

static wxArrayString __AddSshCommand(const wxArrayString& args, const wxString& wd, const wxString& sshAccountName,
                                     clTempFile& tmpfile, const clEnvList_t* env_list)
{
#ifndef __WXMSW__
    wxUnusedVar(tmpfile);
#endif

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

    // determine the ssh client we have
    wxString ssh_client = "ssh";

    //----------------------------------------------------------
    // prepare the main command ("args") as a one liner string
    //----------------------------------------------------------
    wxArrayString* p_args = const_cast<wxArrayString*>(&args);
    wxArrayString tmpargs;

    // add any environment variables provided by the caller
    if(env_list) {
        for(const std::pair<wxString, wxString>& env_var : *env_list) {
            tmpargs.Add(env_var.first + "=" + env_var.second);
        }
    }

    if(!wd.empty()) {
        // add working directory
        tmpargs.Add("cd");
        tmpargs.Add(wd);
        tmpargs.Add("&&");
    }

    // add the command arguments
    tmpargs.insert(tmpargs.end(), args.begin(), args.end());
    p_args = &tmpargs;

    wxString oneLiner = wxJoin(*p_args, ' ', 0);
#ifdef __WXMSW__
    oneLiner.Prepend("\"").Append("\"");
#else
    oneLiner.Replace("\"", "\\\""); // escape any double quotes
#endif

    //----------------------------------------------------------
    // build the executing command
    //----------------------------------------------------------

    // the following are common to both ssh / putty clients
    a.Add(ssh_client);
    a.Add(accountInfo.GetUsername() + "@" + accountInfo.GetHost());
    a.Add("-t");                                // force tty
    a.Add("-p");                                // port
    a.Add(wxString() << accountInfo.GetPort()); // the port number

    //----------------------------------------------------------
    // we support extra args in the environment variable
    // CL_SSH_OPTIONS
    //----------------------------------------------------------
    wxString sshEnv;
    if(::wxGetEnv("CL_SSH_OPTIONS", &sshEnv)) {
        wxArrayString sshOptionsArr = StringUtils::BuildArgv(sshEnv);
        a.insert(a.end(), sshOptionsArr.begin(), sshOptionsArr.end());
    }

    a.Add(oneLiner);
    return a;
}

static void __FixArgs(wxArrayString& args)
{
    for(wxString& arg : args) {
        // escape LF/CR
        arg.Replace("\n", "");
        arg.Replace("\r", "");
#if defined(__WXOSX__) || defined(__WXGTK__)
        arg.Trim().Trim(false);
        if(arg.length() > 1) {
            if(arg.StartsWith("'") && arg.EndsWith("'")) {
                arg.Remove(0, 1);
                arg.RemoveLast();
            } else if(arg.StartsWith("\"") && arg.EndsWith("\"")) {
                arg.Remove(0, 1);
                arg.RemoveLast();
            }
        }
#endif
    }
}

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const std::vector<wxString>& args, size_t flags,
                             const wxString& workingDir, const clEnvList_t* env, const wxString& sshAccountName)
{
    wxArrayString wxargs;
    wxargs.reserve(args.size());
    for(const wxString& s : args) {
        wxargs.Add(s);
    }
    return CreateAsyncProcess(parent, wxargs, flags, workingDir, env, sshAccountName);
}

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxArrayString& args, size_t flags, const wxString& workingDir,
                             const clEnvList_t* env, const wxString& sshAccountName)
{
    clEnvironment e(env);
    wxArrayString c = args;

    // sanity:
    // Check that we have SSH executable
    wxFileName sshpath;
    if((flags & IProcessCreateSSH) && !FileUtils::FindExe("ssh", sshpath)) {
        return nullptr;
    }

    // clDEBUG1() << "1: CreateAsyncProcess called with:" << c << endl;

    if(flags & IProcessWrapInShell) {
        // wrap the command in OS specific terminal
        c = __WrapInShell(c, flags);
    }

    clTempFile tmpfile; // needed for putty clients
    tmpfile.Persist();  // do not delete this file on destruct
    if(flags & IProcessCreateSSH) {
        c = __AddSshCommand(c, workingDir, sshAccountName, tmpfile, env);
    }

    // needed on linux where fork does not require the extra quoting
    __FixArgs(c);
    // clDEBUG1() << "2: CreateAsyncProcess called with:" << c << endl;

#ifdef __WXMSW__
    return WinProcessImpl::Execute(parent, c, flags, workingDir);
#else
    return UnixProcessImpl::Execute(parent, c, flags, workingDir);
#endif
}

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDir,
                             const clEnvList_t* env, const wxString& sshAccountName)
{
    auto args = StringUtils::BuildArgv(cmd);
    return CreateAsyncProcess(parent, args, flags, workingDir, env, sshAccountName);
}

void CreateAsyncProcessCB(const wxString& cmd, std::function<void(const wxString&)> cb, size_t flags,
                          const wxString& workingDir, const clEnvList_t* env)
{
    clEnvironment e(env);
    CreateAsyncProcess(new __AsyncCallback(move(cb)), cmd, flags, workingDir, env, wxEmptyString);
}

IProcess* CreateSyncProcess(const wxString& cmd, size_t flags, const wxString& workingDir, const clEnvList_t* env)
{
    return CreateAsyncProcess(nullptr, StringUtils::BuildArgv(cmd), flags | IProcessCreateSync, workingDir, env,
                              wxEmptyString);
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

void IProcess::SuspendAsyncReads()
{
    if(m_thr) {
        clDEBUG1() << "Suspending process reader thread..." << endl;
        m_thr->Suspend();
        clDEBUG1() << "Suspending process reader thread...done" << endl;
    }
}

void IProcess::ResumeAsyncReads()
{
    if(m_thr) {
        clDEBUG1() << "Resuming process reader thread..." << endl;
        m_thr->Resume();
        clDEBUG1() << "Resuming process reader thread..." << endl;
    }
}
