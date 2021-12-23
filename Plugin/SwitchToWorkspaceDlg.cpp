#include "SwitchToWorkspaceDlg.h"

#include "cl_config.h"
#include "globals.h"

#include <wx/dirdlg.h>
#include <wx/filedlg.h>

SwitchToWorkspaceDlg::SwitchToWorkspaceDlg(wxWindow* parent)
    : SwitchToWorkspaceBaseDlg(parent)
{
    auto recentWorkspaces = clConfig::Get().GetRecentWorkspaces();
    m_comboBoxFiles->Append(recentWorkspaces);
    CenterOnParent();
    GetSizer()->Fit(this);
}

SwitchToWorkspaceDlg::~SwitchToWorkspaceDlg() {}

wxString SwitchToWorkspaceDlg::GetPath() const { return m_comboBoxFiles->GetValue(); }
void SwitchToWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!m_comboBoxFiles->GetValue().IsEmpty()); }
void SwitchToWorkspaceDlg::OnBrowse(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(_("Open workspace"), wxEmptyString, wxEmptyString, "workspace",
                                     "CodeLite workspace file|*.cbp;*.workspace");
    if(path.empty()) {
        return;
    }
    m_comboBoxFiles->SetValue(path);
}
