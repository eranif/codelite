#include "OpenFolderDlg.h"
#include "windowattrmanager.h"
OpenFolderDlg::OpenFolderDlg(wxWindow* parent)
    : OpenFolderDlgBase(parent)
{
    WindowAttrManager::Load(this, "OpenFolderDlg");
}

OpenFolderDlg::~OpenFolderDlg()
{
    WindowAttrManager::Save(this, "OpenFolderDlg");
}
