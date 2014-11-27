#include "WordCompletionSettingsDlg.h"
#include "windowattrmanager.h"

WordCompletionSettingsDlg::WordCompletionSettingsDlg(wxWindow* parent)
    : WordCompletionSettingsBaseDlg(parent)
{
#ifndef __WXOSX__
    m_pgMgr->GetGrid()->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);
#endif
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
