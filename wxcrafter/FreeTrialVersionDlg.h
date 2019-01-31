#ifndef FREETRIALVERSIONDLG_H
#define FREETRIALVERSIONDLG_H
#include "gui.h"

class FreeTrialVersionDlg : public FreeTrialVersionDlgBase
{
    int m_answer;

public:
    FreeTrialVersionDlg(wxWindow* parent);
    virtual ~FreeTrialVersionDlg();

public:
    enum {
        kPurchase,
        kTrial,
        kClose,
    };
    int GetAnswer() const { return m_answer; }

protected:
    virtual void OnTrial(wxCommandEvent& event);
    virtual void OnPurchase(wxCommandEvent& event);
};
#endif // FREETRIALVERSIONDLG_H
