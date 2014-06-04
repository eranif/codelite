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
    CallAfter( &FilesModifiedDlg::EndModal, ID_BUTTON_CHOOSE );
}

void FilesModifiedDlg::OnLoad(wxCommandEvent& event)
{
    CallAfter( &FilesModifiedDlg::EndModal, ID_BUTTON_LOAD );
}
