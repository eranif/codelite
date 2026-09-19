#ifndef GITUSEREMAILDIALOG_H
#define GITUSEREMAILDIALOG_H
#include "gitui.hpp"

class GitUserEmailDialog : public GitUserEmailDialogBase
{
public:
    GitUserEmailDialog(wxWindow* parent);
    ~GitUserEmailDialog() override = default;

    wxString GetEmail() const { return m_textCtrlEmail->GetValue(); }
    wxString GetUsername() const { return m_textCtrlName->GetValue(); }

protected:
    void OnOKUI(wxUpdateUIEvent& event) override;
};
#endif // GITUSEREMAILDIALOG_H
