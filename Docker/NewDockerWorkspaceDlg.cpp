#include "NewDockerWorkspaceDlg.h"

NewDockerWorkspaceDlg::NewDockerWorkspaceDlg(wxWindow* parent)
    : NewDockerWorkspaceDlgBase(parent)
{
}

NewDockerWorkspaceDlg::~NewDockerWorkspaceDlg() {}

void NewDockerWorkspaceDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->IsEmpty() && wxDirExists(m_dirPicker14->GetPath()));
}

wxFileName NewDockerWorkspaceDlg::GetWorkspaceFile() const
{
    wxFileName fn(m_dirPicker14->GetPath(), m_textCtrlName->GetValue());
    fn.SetExt("workspace");
    return fn;
}
