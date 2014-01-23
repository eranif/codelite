#include "gitCloneDlg.h"
#include "windowattrmanager.h"
#include <wx/uri.h>

gitCloneDlg::gitCloneDlg(wxWindow* parent)
    : gitCloneDlgBaseClass(parent)
{
    m_textCtrlURL->SetFocus();
    WindowAttrManager::Load(this, "gitCloneDlg", NULL);
}

gitCloneDlg::~gitCloneDlg()
{
    WindowAttrManager::Save(this, "gitCloneDlg", NULL);
}

void gitCloneDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlURL->IsEmpty() && !m_dirPickerTargetDir->GetPath().IsEmpty() );
}

wxString gitCloneDlg::GetCloneURL() const
{
    wxString urlString = m_textCtrlURL->GetValue();
    return urlString;
}
