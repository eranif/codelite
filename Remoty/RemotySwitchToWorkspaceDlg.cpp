#include "RemotySwitchToWorkspaceDlg.h"
#include "RemotyConfig.hpp"
#include "SFTPBrowserDlg.h"
#include "cl_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include <wx/tokenzr.h>

RemotySwitchToWorkspaceDlg::RemotySwitchToWorkspaceDlg(wxWindow* parent)
    : RemotySwitchToWorkspaceDlgBase(parent)
{
    // populate the recent workspace options
    auto recentWorkspaces = clConfig::Get().GetRecentWorkspaces();
    m_comboBoxLocal->Append(recentWorkspaces);

    // load any recent remote workspaces loaded
    RemotyConfig config;
    auto recentRemoteWorkspaces = config.GetRecentWorkspaces();
    wxArrayString paths;
    paths.reserve(recentRemoteWorkspaces.size());
    for(const auto& recent_path : recentRemoteWorkspaces) {
        paths.Add(GetDisplayStringFromFullPath(recent_path));
        m_displayToRemotePath.insert({ paths.Last(), recent_path });
    }
    m_comboBoxRemote->Append(paths);
    GetSizer()->Fit(this);
    CenterOnParent();
}

RemotySwitchToWorkspaceDlg::~RemotySwitchToWorkspaceDlg()
{
    auto selection = m_comboBoxRemote->GetValue();
    selection.Trim().Trim(false);
    if(m_displayToRemotePath.count(selection) == 0) {
        return;
    }

    const wxString& full_path = m_displayToRemotePath[selection];
    RemotyConfig config;
    config.UpdateRecentWorkspaces(full_path);
}

void RemotySwitchToWorkspaceDlg::OnLocalBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path = ::wxFileSelector(_("Choose a file"), wxEmptyString, wxEmptyString, wxEmptyString,
                                     wxT("CodeLite Workspace files (*.workspace)|*.workspace"));
    if(path.empty()) {
        return;
    }
    m_comboBoxLocal->SetValue(path);
}

void RemotySwitchToWorkspaceDlg::OnRemoteBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto res = ::clRemoteFileSelector(_("Choose a file"), wxEmptyString, "*.workspace", this);
    if(res.first.empty()) {
        return;
    }

    const wxString& account_name = res.first;
    const wxString& path = res.second;

    // build the file's path
    auto account = SSHAccountInfo::LoadAccount(account_name);
    if(account.GetAccountName().empty()) {
        // should not happen
        return;
    }

    // build the path
    auto parts = ::wxStringTokenize(path, "/", wxTOKEN_STRTOK);
    wxString full_path;
    full_path << "ssh://" << account.GetUsername() << "@" << account.GetHost() << ":" << account.GetPort() << ":"
              << path;

    wxString display_path = GetDisplayStringFromFullPath(full_path);
    m_comboBoxRemote->SetValue(display_path);
    m_displayToRemotePath.insert({ display_path, full_path });
}

void RemotySwitchToWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!GetPath().IsEmpty()); }

wxString RemotySwitchToWorkspaceDlg::GetPath() const
{
    if(IsRemote()) {
        return m_displayToRemotePath.find(m_comboBoxRemote->GetValue())->second;
    } else {
        return m_comboBoxLocal->GetValue();
    }
}

wxString RemotySwitchToWorkspaceDlg::GetDisplayStringFromFullPath(const wxString& full_path) const
{
    wxString path, scheme, user_name, remote_server, port;
    FileUtils::ParseURI(full_path, path, scheme, user_name, remote_server, port);

    long nPort = 22;
    port.ToCLong(&nPort);

    auto accounts = SSHAccountInfo::Load([&](const SSHAccountInfo& acc) -> bool {
        return acc.GetUsername() == user_name && acc.GetPort() == nPort && acc.GetHost() == remote_server;
    });

    if(accounts.size() != 1) {
        return full_path;
    }
    const auto& account = accounts[0];

    wxString display_path;
    auto parts = ::wxStringTokenize(path, "/", wxTOKEN_STRTOK);
    if(!parts.empty()) {
        display_path = parts.Last() + "@" + account.GetAccountName();
    } else {
        display_path = full_path;
    }
    return display_path;
}
