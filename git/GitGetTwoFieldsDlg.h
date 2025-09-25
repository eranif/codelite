#ifndef GITGETTWOFIELDSDLG_H
#define GITGETTWOFIELDSDLG_H
#include "gitui.h"

class GitGetTwoFieldsDlg : public GitGetTwoCommitsBaseDlg
{
public:
    GitGetTwoFieldsDlg(wxWindow* parent);
    ~GitGetTwoFieldsDlg() override;

protected:
    void OnButtonokUpdateUi(wxUpdateUIEvent& event) override;
};
#endif // GITGETTWOFIELDSDLG_H
