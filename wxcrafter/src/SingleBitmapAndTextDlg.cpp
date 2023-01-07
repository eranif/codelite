#include "SingleBitmapAndTextDlg.h"
#include "BitmapSelectorDlg.h"
#include <windowattrmanager.h>

SingleBitmapAndTextDlg::SingleBitmapAndTextDlg(wxWindow* parent, const wxString& bmp, const wxString& text)
    : SingleBitmapAndTextDlgBase(parent)
{
    m_textCtrlBmp->ChangeValue(bmp);
    m_textCtrlText->ChangeValue(text);
    SetName("SingleBitmapAndTextDlg");
    WindowAttrManager::Load(this);
}

SingleBitmapAndTextDlg::~SingleBitmapAndTextDlg() {}

void SingleBitmapAndTextDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlBmp->GetValue().IsEmpty() && !m_textCtrlText->GetValue().IsEmpty());
}

void SingleBitmapAndTextDlg::OnSelectBitmap(wxCommandEvent& event)
{
    BitmapSelectorDlg dlg(this, m_textCtrlBmp->GetValue());
    if(dlg.ShowModal() == wxID_OK) { m_textCtrlBmp->ChangeValue(dlg.GetBitmapFile()); }
}
