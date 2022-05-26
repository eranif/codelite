#include "RemotySwitchToWorkspaceDlg.h"

#include "RemotyConfig.hpp"
#include "SFTPBrowserDlg.h"
#include "cl_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"

#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/tokenzr.h>

namespace
{
template <typename T> void UpdateSelection(T* ctrl, const wxString& str)
{
    if((int)ctrl->FindString(str) == wxNOT_FOUND) {
        ctrl->Append(str);
    }
    ctrl->SetStringSelection(str);
}
} // namespace

RemotySwitchToWorkspaceDlg::RemotySwitchToWorkspaceDlg(wxWindow* parent)
    : RemotySwitchToWorkspaceDlgBase(parent)
{
    // populate the recent workspace options
    RemotyConfig config;
    bool workspace_type_local = config.IsOpenWorkspaceTypeLocal();
    m_choiceWorkspaceType->SetStringSelection(workspace_type_local ? "Local" : "Remote");

    InitialiseDialog();
    GetSizer()->Fit(this);
    CenterOnParent();
}

RemotySwitchToWorkspaceDlg::~RemotySwitchToWorkspaceDlg()
{
    RemotyConfig config;
    if(IsRemote()) {
        RemoteWorkspaceInfo wi{ m_choiceAccount->GetStringSelection(), m_comboBoxPath->GetStringSelection() };
        config.UpdateRecentWorkspaces(wi);
    }
    config.SetOpenWorkspaceTypeLocal(m_choiceWorkspaceType->GetStringSelection() == "Local");
}

void RemotySwitchToWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!m_comboBoxPath->GetValue().IsEmpty()); }

void RemotySwitchToWorkspaceDlg::OnBrowse(wxCommandEvent& event)
{
    if(IsRemote()) {
        auto res = ::clRemoteFileSelector(_("Choose a file"), wxEmptyString, "*.workspace", this);
        if(res.first.empty()) {
            return;
        }
        const wxString& account_name = res.first;
        const wxString& path = res.second;
        // update the account drop box
        UpdateSelection(m_choiceAccount, account_name);
        // update the path
        UpdateSelection(m_comboBoxPath, path);
    } else {
        wxString path = ::wxFileSelector(_("Choose a file"), wxEmptyString, wxEmptyString, wxEmptyString,
                                         "CodeLite Workspace files (*.workspace)|*.workspace");
        if(path.empty()) {
            return;
        }
        UpdateSelection(m_comboBoxPath, path);
    }
}

void RemotySwitchToWorkspaceDlg::OnRemoteUI(wxUpdateUIEvent& event) { event.Enable(IsRemote()); }

void RemotySwitchToWorkspaceDlg::OnChoiceWorkspaceType(wxCommandEvent& event)
{
    wxUnusedVar(event);
    InitialiseDialog();
}

bool RemotySwitchToWorkspaceDlg::IsRemote() const { return m_choiceWorkspaceType->GetStringSelection() == "Remote"; }

void RemotySwitchToWorkspaceDlg::SyncPathToAccount()
{
    int path_index = static_cast<int>(m_comboBoxPath->GetSelection());
    if(wxNOT_FOUND == path_index || path_index >= static_cast<int>(m_remoteWorkspaces.size())) {
        return;
    }

    m_choiceAccount->SetStringSelection(m_remoteWorkspaces[path_index].account);
}

wxString RemotySwitchToWorkspaceDlg::GetPath() const { return m_comboBoxPath->GetValue(); }

wxString RemotySwitchToWorkspaceDlg::GetAccount() { return m_choiceAccount->GetStringSelection(); }

void RemotySwitchToWorkspaceDlg::InitialiseDialog()
{
    RemotyConfig config;
    bool workspace_type_local = m_choiceWorkspaceType->GetStringSelection() == "Local";

    m_remoteWorkspaces = config.GetRecentWorkspaces();
    m_comboBoxPath->Clear();
    m_choiceAccount->Clear();

    if(workspace_type_local) {
        auto local_paths = clConfig::Get().GetRecentWorkspaces();
        m_comboBoxPath->Append(local_paths);
        if(!local_paths.empty()) {
            m_comboBoxPath->SetSelection(0);
        }
    } else {
        // remote workspaces
        std::set<wxString> S;
        if(m_remoteWorkspaces.empty()) {
            return;
        }
        for(size_t i = 0; i < m_remoteWorkspaces.size(); ++i) {
            m_comboBoxPath->Append(m_remoteWorkspaces[i].path);
            S.insert(m_remoteWorkspaces[i].account);
        }

        for(const auto& account : S) {
            m_choiceAccount->Append(account);
        }
        m_comboBoxPath->SetSelection(0);
        SyncPathToAccount();
    }
}

void RemotySwitchToWorkspaceDlg::OnPathChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(!IsRemote()) {
        return;
    }
    int sel = m_comboBoxPath->GetSelection();
    if(sel == wxNOT_FOUND || sel >= (int)m_remoteWorkspaces.size()) {
        return;
    }
    m_choiceAccount->SetStringSelection(m_remoteWorkspaces[sel].account);
}
