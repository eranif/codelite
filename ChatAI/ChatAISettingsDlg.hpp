#pragma once

#include "UI.hpp"

class ChatAISettingsDlg : public AssistanceAISettingsBaseDlg
{
public:
    ChatAISettingsDlg(wxWindow* parent);
    virtual ~ChatAISettingsDlg();

protected:
    virtual void OnNewModel(wxCommandEvent& event);
};

