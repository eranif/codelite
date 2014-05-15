#include "FilesModifiedDlg.h"

FilesModifiedDlg::FilesModifiedDlg(wxWindow* parent)
    : FilesModifiedDlgBase(parent)
{
}

FilesModifiedDlg::~FilesModifiedDlg()
{
}

void FilesModifiedDlg::OnChoose(wxCommandEvent& event)
{
    EndModal( ID_BUTTON_CHOOSE );
}

void FilesModifiedDlg::OnLoad(wxCommandEvent& event)
{
    EndModal( ID_BUTTON_LOAD );
}
