#include "WordCompletionSettingsDlg.h"
#include "windowattrmanager.h"
#include "globals.h"

WordCompletionSettingsDlg::WordCompletionSettingsDlg(wxWindow* parent)
    : WordCompletionSettingsBaseDlg(parent)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
    WordCompletionSettings settings;
    size_t completeTypes = settings.Load().GetCompleteTypes();
    m_pgPropTypes->SetValueFromInt(completeTypes);
    m_pgPropComparisonMethod->SetChoiceSelection(settings.GetComparisonMethod());
    
    WindowAttrManager::Load(this, "WordCompletionSettingsDlg");
}

WordCompletionSettingsDlg::~WordCompletionSettingsDlg() { WindowAttrManager::Save(this, "WordCompletionSettingsDlg"); }

void WordCompletionSettingsDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    WordCompletionSettings settings;
    settings.Load();
    settings.SetCompleteTypes(m_pgPropTypes->GetValue().GetInteger());
    settings.SetComparisonMethod(m_pgPropComparisonMethod->GetChoiceSelection());
    settings.Save();
}
