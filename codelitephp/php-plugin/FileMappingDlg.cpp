#include "FileMappingDlg.h"
#include <windowattrmanager.h>

FileMappingDlg::FileMappingDlg(wxWindow* parent)
    : FileMappingDlgBase(parent)
{
    WindowAttrManager::Load(this, "FileMappingDlg");
}

FileMappingDlg::~FileMappingDlg()
{
    WindowAttrManager::Save(this, "FileMappingDlg");
}

void FileMappingDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dirPickerSource->GetPath().IsEmpty() && !m_dirPickerTarget->GetPath().IsEmpty() );
}
