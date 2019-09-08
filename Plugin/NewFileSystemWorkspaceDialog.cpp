#include "NewFileSystemWorkspaceDialog.h"
#include <wx/filename.h>
#include "globals.h"
#include <wx/filename.h>

NewFileSystemWorkspaceDialog::NewFileSystemWorkspaceDialog(wxWindow* parent)
    : NewFileSystemWorkspaceDialogBase(parent)
{
    GetSizer()->Fit(this);
    CenterOnParent();
}

NewFileSystemWorkspaceDialog::~NewFileSystemWorkspaceDialog() {}

void NewFileSystemWorkspaceDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->GetValue().IsEmpty() && wxFileName::DirExists(m_dirPickerPath->GetPath()));
}

wxString NewFileSystemWorkspaceDialog::GetWorkspaceName() const { return m_textCtrlName->GetValue(); }

wxString NewFileSystemWorkspaceDialog::GetWorkspacePath() const { return m_dirPickerPath->GetPath(); }

void NewFileSystemWorkspaceDialog::OnDirSelected(wxFileDirPickerEvent& event)
{
    event.Skip();
    wxString path = event.GetPath();
    wxFileName fndir(path, "");
    if(fndir.GetDirCount() == 0) { return; }
    if(m_textCtrlName->IsEmpty()) { m_textCtrlName->ChangeValue(fndir.GetDirs().Last()); }
}
