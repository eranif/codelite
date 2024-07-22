#pragma once

#include "ChatAIConfig.hpp"
#include "UI.hpp"

class ChatAISettingsDlg : public AssistanceAISettingsBaseDlg
{
public:
    ChatAISettingsDlg(wxWindow* parent, ChatAIConfig& config);
    virtual ~ChatAISettingsDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnNewModel(wxCommandEvent& event);

private:
    ChatAIConfig& m_config;
};
