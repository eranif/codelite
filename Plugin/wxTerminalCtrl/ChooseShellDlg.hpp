#pragma once

#include "wxTerminalCtrlUI.hpp"

/// Ask the user to choose a shell. The user can also ask us to remember the choice and not to ask again.
class ChooseShellDlg : public ChooseShellBaseDlg
{
public:
    ChooseShellDlg(wxWindow* parent, const wxArrayString& shells, int initialSelection);
    ~ChooseShellDlg() override;

    /// The index of the selected shell, or wxNOT_FOUND
    int GetSelection() const;
    /// True if the user checked "Remember my choice and don't ask me again"
    bool IsRemember() const;

protected:
    void OnShellDoubleClicked(wxCommandEvent& event) override;
};
