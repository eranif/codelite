#include "new_file_dlg.h"
#include <windowattrmanager.h>

NewFileDlg::NewFileDlg( wxWindow* parent, const wxString &path )
    : NewFileDlgBase( parent )
{
    m_textCtrlName->SetFocus();
    m_dirPickerPath->SetPath(path);
    WindowAttrManager::Load(this, wxT("NewFileDlg"), NULL);
}

NewFileDlg::~NewFileDlg()
{
    WindowAttrManager::Save(this, wxT("NewFileDlg"), NULL);
}

wxFileName NewFileDlg::GetPath() const
{
    wxFileName filename(m_dirPickerPath->GetPath(), m_textCtrlName->GetValue());
    return filename;
}
