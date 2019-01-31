#include "FreeTrialVersionDlg.h"
#include "wxc_settings.h"
#include <wx/stdpaths.h>

FreeTrialVersionDlg::FreeTrialVersionDlg(wxWindow* parent)
    : FreeTrialVersionDlgBase(parent)
    , m_answer(kClose)
{
    GetSizer()->Fit(this);
    CenterOnParent();
}

FreeTrialVersionDlg::~FreeTrialVersionDlg() {}

void FreeTrialVersionDlg::OnTrial(wxCommandEvent& event)
{
    m_answer = kTrial;
    EndModal(wxID_OK);
}

void FreeTrialVersionDlg::OnPurchase(wxCommandEvent& event)
{
    m_answer = kPurchase;
    EndModal(wxID_OK);
}
