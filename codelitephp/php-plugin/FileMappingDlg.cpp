#include "FileMappingDlg.h"

#include "windowattrmanager.h"

FileMappingDlg::FileMappingDlg(wxWindow* parent)
    : FileMappingDlgBase(parent)
{
    SetName("FileMappingDlg");
    WindowAttrManager::Load(this);
}

FileMappingDlg::~FileMappingDlg() {}

void FileMappingDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_dirPickerSource->GetPath().IsEmpty() && !m_textCtrlRemote->IsEmpty());
}
