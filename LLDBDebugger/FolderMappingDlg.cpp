#include "FolderMappingDlg.h"
#include "LLDBProtocol/LLDBSettings.h"

FolderMappingDlg::FolderMappingDlg(wxWindow* parent)
    : FolderMappingBaseDlg(parent)
{
    LLDBSettings settings;
    settings.Load();
    
    m_dirPickerLocal->SetPath( settings.GetLastLocalFolder() );
    m_textCtrlRemote->ChangeValue( settings.GetLastRemoteFolder() );
}

FolderMappingDlg::~FolderMappingDlg()
{
    LLDBSettings settings;
    settings.Load();
    settings.SetLastLocalFolder(m_dirPickerLocal->GetPath()  );
    settings.SetLastRemoteFolder(m_textCtrlRemote->GetValue()  );
    settings.Save();
}

void FolderMappingDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlRemote->IsEmpty() && !m_dirPickerLocal->GetPath().IsEmpty());
}

LLDBPivot FolderMappingDlg::GetPivot() const
{
    LLDBPivot pivot;
    pivot.SetLocalFolder( m_dirPickerLocal->GetPath() );
    pivot.SetRemoteFolder( m_textCtrlRemote->GetValue() );
    return pivot;
}
