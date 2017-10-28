#ifndef SMARTCOMPLETIONSSETTINGSDLG_H
#define SMARTCOMPLETIONSSETTINGSDLG_H
#include "SmartCompletionsUI.h"
#include "SmartCompletionsConfig.h"

class SmartCompletionsSettingsDlg : public SmartCompletionsSettingsBaseDlg
{
    SmartCompletionsConfig& m_config;

public:
    SmartCompletionsSettingsDlg(wxWindow* parent, SmartCompletionsConfig& config);
    virtual ~SmartCompletionsSettingsDlg();

protected:
    virtual void OnClearStats(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};
#endif // SMARTCOMPLETIONSSETTINGSDLG_H
