#include "RemotySwitchToWorkspaceDlg.h"
#include "SFTPBrowserDlg.h"
#include "cl_config.h"
#include "globals.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"

RemotySwitchToWorkspaceDlg::RemotySwitchToWorkspaceDlg(wxWindow* parent)
    : RemotySwitchToWorkspaceDlgBase(parent)
{
    // populate the recent workspace options
    auto recentWorkspaces = clConfig::Get().GetRecentWorkspaces();
    m_comboBoxLocal->Append(recentWorkspaces);

    // load any recent remote workspaces loaded
    auto recentRemoteWorkspaces = clConfig::Get().Read("remoty/recent_workspaces", wxArrayString());
    m_comboBoxRemote->Append(recentRemoteWorkspaces);
    GetSizer()->Fit(this);
}

RemotySwitchToWorkspaceDlg::~RemotySwitchToWorkspaceDlg()
{
    auto recentRemoteWorkspaces = clConfig::Get().Read("remoty/recent_workspaces", wxArrayString());
    auto selection = m_comboBoxRemote->GetValue();
    selection.Trim().Trim(false);
    if(!selection.empty() && (recentRemoteWorkspaces.Index(selection) == wxNOT_FOUND)) {
        recentRemoteWorkspaces.Add(selection);
        recentRemoteWorkspaces.Sort();
        clConfig::Get().Write("remoty/recent_workspaces", recentRemoteWorkspaces);
    }
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
    wxString remotePath;
    remotePath << "ssh://" << account.GetUsername() << "@" << account.GetHost() << ":" << account.GetPort() << ":"
               << path;
    m_comboBoxRemote->SetValue(remotePath);
}

void RemotySwitchToWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!GetPath().IsEmpty()); }
