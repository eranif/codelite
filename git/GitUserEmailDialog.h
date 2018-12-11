#ifndef GITUSEREMAILDIALOG_H
#define GITUSEREMAILDIALOG_H
#include "gitui.h"

class GitUserEmailDialog : public GitUserEmailDialogBase
{
public:
    GitUserEmailDialog(wxWindow* parent);
    virtual ~GitUserEmailDialog();

    wxString GetEmail() const { return m_textCtrlEmail->GetValue(); }
    wxString GetUsername() const { return m_textCtrlName->GetValue(); }

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // GITUSEREMAILDIALOG_H
