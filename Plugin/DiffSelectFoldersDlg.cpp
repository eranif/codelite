#include "DiffSelectFoldersDlg.h"
#include <wx/filename.h>

DiffSelectFoldersDlg::DiffSelectFoldersDlg(wxWindow* parent)
    : DiffSelectFoldersBaseDlg(parent)
{
    CentreOnParent();
}

DiffSelectFoldersDlg::~DiffSelectFoldersDlg() {}

void DiffSelectFoldersDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString left(m_dirPickerLeft->GetPath());
    wxString right(m_dirPickerRight->GetPath());

    event.Enable(left != right && wxFileName::DirExists(left) && wxFileName::DirExists(right));
}
