#include "NewVirtualFolderDlg.h"

NewVirtualFolderDlg::NewVirtualFolderDlg(wxWindow* parent)
    : NewVirtualFolderDlgBase(parent)
{
}

NewVirtualFolderDlg::~NewVirtualFolderDlg()
{
}
void NewVirtualFolderDlg::OnCreateOnDiskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxCreateOnDisk->IsChecked());
}

void NewVirtualFolderDlg::OnNameUpdated(wxCommandEvent& event)
{

}
void NewVirtualFolderDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlName->IsEmpty() );
}
