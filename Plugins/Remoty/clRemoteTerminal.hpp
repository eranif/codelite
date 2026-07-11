#ifndef CLREMOTETERMINAL_HPP
#define CLREMOTETERMINAL_HPP

#include "cl_command_event.h"
#include "ssh/ssh_account_info.h"

#include <memory>
#include <wx/event.h>

class clRemoteTerminal : public wxEvtHandler
{
    IProcess* m_proc = nullptr;
    wxString m_tty;
    wxString m_tty_file;
    SSHAccountInfo m_account;

public:
    using ptr_t = std::unique_ptr<clRemoteTerminal>;

public:
    clRemoteTerminal(const SSHAccountInfo& account);
    ~clRemoteTerminal();

    bool Start();
    void Stop();

    const wxString& ReadTty();
};

#endif // CLREMOTETERMINAL_HPP
