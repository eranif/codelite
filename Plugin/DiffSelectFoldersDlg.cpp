#include "DiffSelectFoldersDlg.h"
#include <wx/filename.h>

DiffSelectFoldersDlg::DiffSelectFoldersDlg(wxWindow* parent, const wxString& left, const wxString& right)
    : DiffSelectFoldersBaseDlg(parent)
{
    wxSize parentSize = GetParent()->GetSize();
    parentSize.SetWidth(parentSize.GetWidth() / 2);
    SetSize(parentSize.GetWidth(), -1);
    m_dirPickerLeft->SetPath(left);
    m_dirPickerRight->SetPath(right);
    CentreOnParent();
}

DiffSelectFoldersDlg::~DiffSelectFoldersDlg() {}

void DiffSelectFoldersDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString left(m_dirPickerLeft->GetPath());
    wxString right(m_dirPickerRight->GetPath());

    event.Enable(left != right && wxFileName::DirExists(left) && wxFileName::DirExists(right));
}
