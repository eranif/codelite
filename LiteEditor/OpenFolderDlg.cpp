#include "OpenFolderDlg.h"
#include "windowattrmanager.h"
OpenFolderDlg::OpenFolderDlg(wxWindow* parent)
    : OpenFolderDlgBase(parent)
{
    SetName("OpenFolderDlg");
    WindowAttrManager::Load(this);
}
