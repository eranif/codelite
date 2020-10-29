#include "SwitchToWorkspaceDlg.h"
#include "cl_config.h"
#include "globals.h"

SwitchToWorkspaceDlg::SwitchToWorkspaceDlg(wxWindow* parent)
    : SwitchToWorkspaceBaseDlg(parent)
{
    auto recentWorkspaces = clConfig::Get().GetRecentWorkspaces();
    for(const auto& wspfile : recentWorkspaces) {
        wxVector<wxVariant> cols;
        wxFileName fn(wspfile);
        cols.push_back(fn.GetFullName());
        cols.push_back(fn.GetFullPath());
        m_dvListCtrl->AppendItem(cols);
    }
    ::clSetDialogBestSizeAndPosition(this);
}

SwitchToWorkspaceDlg::~SwitchToWorkspaceDlg() {}

void SwitchToWorkspaceDlg::OnItemActivated(wxDataViewEvent& event)
{
    m_filePicker->SetPath(m_dvListCtrl->GetItemText(event.GetItem(), 1));
    // Close the dialog
    EndModal(wxID_OK);
}

void SwitchToWorkspaceDlg::OnSelectionChanged(wxDataViewEvent& event)
{
    m_filePicker->SetPath(m_dvListCtrl->GetItemText(event.GetItem(), 1));
}

wxString SwitchToWorkspaceDlg::GetPath() const { return m_filePicker->GetPath(); }
