#include "SmartCompletionsSettingsDlg.h"
#include "SmartCompletionsConfig.h"
#include "windowattrmanager.h"

SmartCompletionsSettingsDlg::SmartCompletionsSettingsDlg(wxWindow* parent, SmartCompletionsConfig& config)
    : SmartCompletionsSettingsBaseDlg(parent)
    , m_config(config)
{
    m_checkBoxEnabled->SetValue(m_config.IsEnabled());
    WindowAttrManager::Load(this);
}

SmartCompletionsSettingsDlg::~SmartCompletionsSettingsDlg() {}

void SmartCompletionsSettingsDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    m_config.Load();
    m_config.SetEnabled(m_checkBoxEnabled->IsChecked());
    m_config.Save();
}

void SmartCompletionsSettingsDlg::OnClearStats(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_config.GetUsageDb().Clear();
}
