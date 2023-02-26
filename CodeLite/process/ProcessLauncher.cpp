#include "ProcessLauncher.hpp"

#include "StringUtils.h"
#include "clTempFile.hpp"
#include "file_logger.h"
#include "fileutils.h"
#include "ssh/ssh_account_info.h"

#include <wx/process.h>

#ifdef __WXMSW__
static bool shell_is_cmd = true;
#else
static bool shell_is_cmd = false;
#endif

namespace
{

class NonInteractiveProcess : public wxProcess, public IProcess
{
    
};

///
/// Helper methods
///
void fix_args(wxArrayString& args)
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

wxArrayString add_ssh_command(const wxArrayString& args, const wxString& wd, const wxString& sshAccountName,
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

void wrap_spaces_for_shell(wxString& str, size_t flags)
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

/// wrap the given command `args` in the OS terminal emulator
wxArrayString wrap_in_shell(const wxArrayString& args, size_t flags)
{
    wxArrayString tmparr = args;
    for(wxString& arg : tmparr) {
        wrap_spaces_for_shell(arg, flags);
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

} // namespace

IProcess* ProcessLauncher::LaunchNonInteractive(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                                                const wxString& workingDir, const clEnvList_t* env,
                                                const wxString& sshAccountName)
{
    auto cmdarr = StringUtils::BuildArgv(cmd);
    return LaunchNonInteractive(parent, cmdarr, flags, workingDir, env, sshAccountName);
}

IProcess* ProcessLauncher::LaunchNonInteractive(wxEvtHandler* parent, const wxArrayString& args, size_t flags,
                                                const wxString& workingDir, const clEnvList_t* env,
                                                const wxString& sshAccountName)
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
        c = wrap_in_shell(c, flags);
    }

    clTempFile tmpfile; // needed for putty clients
    tmpfile.Persist();  // do not delete this file on destruct
    if(flags & IProcessCreateSSH) {
        c = add_ssh_command(c, workingDir, sshAccountName, tmpfile, env);
    }

    // needed on linux where fork does not require the extra quoting
    fix_args(c);

    ::wxExecute()
}
