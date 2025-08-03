#include "WordCompletionSettingsDlg.h"
#include "windowattrmanager.h"
#include "globals.h"

WordCompletionSettingsDlg::WordCompletionSettingsDlg(wxWindow* parent)
    : WordCompletionSettingsBaseDlg(parent)
    , m_modified(false)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
    WordCompletionSettings settings;
    settings.Load();
    m_pgPropComparisonMethod->SetChoiceSelection(settings.GetComparisonMethod());
    m_pgPropEnabled->SetValue(settings.IsEnabled());
    SetName("WordCompletionSettingsDlg");
    WindowAttrManager::Load(this);
}

void WordCompletionSettingsDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_modified = true;
}

void WordCompletionSettingsDlg::OnOk(wxCommandEvent& event)
{
    event.Skip();
    WordCompletionSettings settings;
    settings.Load();
    settings.SetComparisonMethod(m_pgPropComparisonMethod->GetChoiceSelection());
    settings.SetEnabled(m_pgPropEnabled->GetValue().GetBool());
    settings.Save();
    EndModal(wxID_OK);
}

void WordCompletionSettingsDlg::OnOkUI(wxUpdateUIEvent& event) { event.Enable(m_modified); }
