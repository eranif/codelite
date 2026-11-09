#pragma once
#include "SmartCompletionsConfig.h"
#include "SmartCompletionsUI.hpp"

class SmartCompletionsSettingsDlg : public SmartCompletionsSettingsBaseDlg
{
    SmartCompletionsConfig& m_config;

public:
    SmartCompletionsSettingsDlg(wxWindow* parent, SmartCompletionsConfig& config);
    virtual ~SmartCompletionsSettingsDlg() = default;

protected:
    virtual void OnClearStats(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};
