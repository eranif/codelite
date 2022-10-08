#ifndef CODELITEREMOTEHELPER_HPP
#define CODELITEREMOTEHELPER_HPP

#include "JSON.h"
#include "clSFTPEvent.h"
#include "clWorkspaceEvent.hpp"
#include "codelite_exports.h"
#include "wxStringHash.h"

#include <unordered_map>
#include <wx/event.h>

class WXDLLIMPEXP_SDK CodeLiteRemoteHelper : public wxEvtHandler
{
    bool m_isRemoteLoaded = false;
    wxString m_workspacePath;
    wxString m_remoteAccount;
    wxString m_ssh_exe;
    std::unordered_map<wxString, JSON*> m_plugins_configs;

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
#if USE_SFTP
    void ProcessCodeLiteRemoteJSON(const wxString& filepath);
#endif
    void Clear();

public:
    CodeLiteRemoteHelper();
    virtual ~CodeLiteRemoteHelper();

    /**
     * @brief return the remote config for the plugin
     */
    JSON* GetPluginConfig(const wxString& plugin_name) const;

    /**
     * @brief return true if a remote workspce is loaded
     */
    bool IsRemoteWorkspaceOpened() const;

    /**
     * @brief return the remote workspace ssh account
     */
    const wxString& GetRemoteAccount() const { return m_remoteAccount; }

    /**
     * @brief wrap a command in ssh user@host "command" + add environment variables if available
     */
    bool BuildRemoteCommand(const wxString& command, const clEnvList_t& envlist, const wxString& remote_wd,
                            wxString* out_cmmand) const;
};

#endif // CODELITEREMOTEHELPER_HPP
