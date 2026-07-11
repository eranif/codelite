#pragma once

#include "gitui.hpp"

class GitReleaseNotesGenerationDlg : public GitReleaseNotesGenerationBaseDlg
{
public:
    GitReleaseNotesGenerationDlg(wxWindow* parent);
    ~GitReleaseNotesGenerationDlg() override;

protected:
    void OnChoiceProviders(wxCommandEvent& event) override;
    void OnChoiceProvidersUpdateUI(wxUpdateUIEvent& event) override;
    void OnButtonokUpdateUi(wxUpdateUIEvent& event) override;

    void UpdateEndpoints();
};
