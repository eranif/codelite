#pragma once

#include "AgentHostUI.hpp"

class LaunchAIAgentDlg : public LaunchAIAgentDlgBase
{
public:
    LaunchAIAgentDlg(wxWindow* parent,
                     const wxArrayString& choices,
                     int selection = wxNOT_FOUND,
                     const wxString& workingDir = wxEmptyString);
    ~LaunchAIAgentDlg() override = default;

    int GetAgent() const;
    wxString GetWorkingDirectory() const;

protected:
    void OnActivated(wxCommandEvent& event) override;
    void OnOkUI(wxUpdateUIEvent& event) override;
};
