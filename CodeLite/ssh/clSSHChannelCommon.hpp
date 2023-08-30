#ifndef CLSSHCHANNELCOMMON_HPP
#define CLSSHCHANNELCOMMON_HPP

#if USE_SFTP

#include "clEnvironment.hpp"
#include "clResult.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "ssh/cl_ssh.h"

#include <vector>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_WRITE_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_READ_STDERR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);

namespace ssh
{
enum class read_result {
    SSH_TIMEOUT = -2,
    SSH_CONN_CLOSED = -1,
    SSH_IO_ERROR = 0,
    SSH_SUCCESS = 1,
};

/// return true if the read_result is not an error
inline bool result_ok(read_result res) { return res == read_result::SSH_SUCCESS || res == read_result::SSH_TIMEOUT; }

/// read from the channel
/// return ssh::read_result::* (see above)
/// this function fire events of type: `wxEVT_SSH_CHANNEL_*`
WXDLLIMPEXP_CL read_result channel_read(SSHChannel_t channel, wxEvtHandler* handler, bool isStderr, bool wantStderr);

/// read from the channel and return the `output`
/// return ssh::read_result::* (see above)
WXDLLIMPEXP_CL read_result channel_read(SSHChannel_t channel, std::string* output, bool isStderr, bool wantStderr);

/// read everything from the channel until the channel is closed or an error occurs
/// return 0 if the process exited with success, 1 if it exited with an error
WXDLLIMPEXP_CL int channel_read_all(SSHChannel_t channel, std::string* output, bool isStderr);

/// build a oneliner command to execute on the remote host
WXDLLIMPEXP_CL wxString build_command(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env);

/// Place the command + env into a script string
WXDLLIMPEXP_CL wxString build_script_content(const std::vector<wxString>& command, const wxString& wd,
                                             const clEnvList_t& env);

WXDLLIMPEXP_CL clResultBool write_remote_file_content(clSSH::Ptr_t ssh, const wxString& remote_path,
                                                      const wxString& content);

} // namespace ssh
#endif

#endif // CLSSHCHANNEL_H
