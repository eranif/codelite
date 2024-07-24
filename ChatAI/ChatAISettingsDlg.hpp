#pragma once

#include "ChatAIConfig.hpp"
#include "UI.hpp"

class ChatAISettingsDlg : public AssistanceAISettingsBaseDlg
{
public:
    ChatAISettingsDlg(wxWindow* parent, ChatAIConfig& config);
    virtual ~ChatAISettingsDlg();

protected:
    void OnSearchModels(wxHyperlinkEvent& event) override;
    void OnDelete(wxCommandEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;
    void OnOK(wxCommandEvent& event) override;
    void OnNewModel(wxCommandEvent& event) override;
    void Save();

private:
    ChatAIConfig& m_config;
};
