#pragma once

#include "gitui.h"

class GitReleaseNotesGenerationDlg : public GitReleaseNotesGenerationBaseDlg
{
public:
    GitReleaseNotesGenerationDlg(wxWindow* parent);
    ~GitReleaseNotesGenerationDlg() override;

protected:
    void OnChoicemodelsUpdateUi(wxUpdateUIEvent& event) override;
    void OnButtonokUpdateUi(wxUpdateUIEvent& event) override;
};
