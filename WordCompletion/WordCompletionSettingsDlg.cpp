#include "WordCompletionSettingsDlg.h"
#include "windowattrmanager.h"
#include "globals.h"

WordCompletionSettingsDlg::WordCompletionSettingsDlg(wxWindow* parent)
    : WordCompletionSettingsBaseDlg(parent)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
    WordCompletionSettings settings;
    m_pgPropComparisonMethod->SetChoiceSelection(settings.GetComparisonMethod());
    SetName("WordCompletionSettingsDlg");
    WindowAttrManager::Load(this);
}

WordCompletionSettingsDlg::~WordCompletionSettingsDlg() {  }

void WordCompletionSettingsDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    WordCompletionSettings settings;
    settings.Load();
    settings.SetComparisonMethod(m_pgPropComparisonMethod->GetChoiceSelection());
    settings.Save();
}
