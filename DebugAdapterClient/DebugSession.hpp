#ifndef DEBUG_SESSION_HPP
#define DEBUG_SESSION_HPP

#include "clDapSettingsStore.hpp"
#include "dap/dap.hpp"
#include "processreaderthread.h"
#include "ssh_account_info.h"

#include <vector>
#include <wx/string.h>

struct DebugSession {
    std::vector<wxString> command;
    wxString working_directory;
    clEnvList_t environment;
    bool need_to_set_breakpoints = false;
    bool debug_over_ssh = false;
    SSHAccountInfo ssh_acount;
    DapEntry dap_server;

    void Clear()
    {
        need_to_set_breakpoints = false;
        working_directory.clear();
        debug_over_ssh = false;
        ssh_acount = {};
        command.clear();
        environment.clear();
        dap_server = {};
    }
};

#endif // DEBUG_SESSION_HPP
