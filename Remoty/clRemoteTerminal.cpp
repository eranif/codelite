#include "clRemoteTerminal.hpp"

#include "asyncprocess.h"
#include "environmentconfig.h"
#include "file_logger.h"
#include "globals.h"

#include <wx/tokenzr.h>

clRemoteTerminal::clRemoteTerminal(const SSHAccountInfo& account)
    : m_account(account)
{
    m_tty_file << "/tmp/remoty-" << ::clGetUserName() << "-tty";
}

clRemoteTerminal::~clRemoteTerminal() { wxDELETE(m_proc); }

bool clRemoteTerminal::Start()
{
    if(m_proc) {
        return true;
    }

    // wrap the command in ssh
    wxFileName ssh_exe;
    EnvSetter setter;
    if(!FileUtils::FindExe("ssh", ssh_exe)) {
        clERROR() << "Could not locate ssh executable in your PATH!" << endl;
        return false;
    }

    std::vector<wxString> command = { "ssh", "-o", "ServerAliveInterval=10", "-o", "StrictHostKeyChecking=no" };
    command.push_back(m_account.GetUsername() + "@" + m_account.GetHost());
    command.push_back("-t");
    command.push_back("-p");
    command.push_back(wxString() << m_account.GetPort());
    wxString ssh_cmd;
    ssh_cmd << "tty > " << m_tty_file << " 2>/dev/null && sleep 10000";
    command.push_back(ssh_cmd);

    m_proc = ::CreateAsyncProcess(nullptr, command, IProcessCreateConsole | IProcessNoRedirect | IProcessWrapInShell);
    return m_proc != nullptr;
}

void clRemoteTerminal::Stop()
{
    wxDELETE(m_proc);
    m_tty.clear();
    m_tty_file.clear();
}

const wxString& clRemoteTerminal::ReadTty()
{
    static wxString empty_string;
    // wrap the command in ssh
    wxFileName ssh_exe;
    EnvSetter setter;
    if(!FileUtils::FindExe("ssh", ssh_exe)) {
        clERROR() << "Could not locate ssh executable in your PATH!" << endl;
        return empty_string;
    }

    std::vector<wxString> command = { "cat", m_tty_file };
    IProcess::Ptr_t proc(::CreateAsyncProcess(this, command, IProcessCreateSSH | IProcessCreateSync, wxEmptyString,
                                              nullptr, m_account.GetAccountName()));

    wxString output;
    if(!proc) {
        m_tty.Clear();
        return m_tty;
    }
    proc->WaitForTerminate(output);

    m_tty.clear();
    auto arr = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(auto s : arr) {
        s.Trim().Trim(false);
        if(s.StartsWith("/dev")) {
            m_tty = s;
            break;
        }
    }
    return m_tty;
}
