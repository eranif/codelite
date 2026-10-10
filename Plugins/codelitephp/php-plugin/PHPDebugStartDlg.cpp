#include "PHPDebugStartDlg.h"

#include "imanager.h"
#include "windowattrmanager.h"

PHPDebugStartDlg::PHPDebugStartDlg(wxWindow* parent, PHPProjectSettingsData& settings, IManager* manager)
    : PHPDebugStartDlgBase(parent)
    , m_settings(settings)
    , m_manager(manager)
{
#ifdef __WXOSX__
    // On OSX, remove the 'show effect' or we will see a noticeable delay
    // when showing this dialog
    m_simpleBook->SetEffect(wxSHOW_EFFECT_NONE);
#endif

    if (settings.GetRunAs() == PHPProjectSettingsData::kRunAsWebsite) {
        m_choice->Select(0);
        m_simpleBook->SetSelection(0);
    } else {
        m_choice->Select(1);
        m_simpleBook->SetSelection(1);
    }
    GetComboBoxURL()->Append(settings.GetProjectURL());
    if (GetComboBoxURL()->GetCount()) {
        GetComboBoxURL()->SetSelection(0);
    }

    GetCheckBoxDebugActiveEditor()->SetValue(settings.HasFlag(PHPProjectSettingsData::kOpt_RunCurrentEditor));

    wxString fileToRun;
    if (m_manager->GetActiveEditor() && GetCheckBoxDebugActiveEditor()->IsChecked()) {
        fileToRun = m_manager->GetActiveEditor()->GetFileName().GetFullPath();
    } else {
        fileToRun = settings.GetIndexFile();
    }
    GetTextCtrlScriptToDebug()->ChangeValue(fileToRun);
    SetName("PHPDebugStartDlg");
    WindowAttrManager::Load(this);
}

PHPDebugStartDlg::~PHPDebugStartDlg()
{
    // Store the settings, the caller saves them
    m_settings.EnableFlag(PHPProjectSettingsData::kOpt_RunCurrentEditor, GetCheckBoxDebugActiveEditor()->IsChecked());
    m_settings.SetIndexFile(GetTextCtrlScriptToDebug()->GetValue());
    m_settings.SetProjectURL(GetComboBoxURL()->GetValue());
    m_settings.SetRunAs(m_choice->GetSelection() == 0 ? PHPProjectSettingsData::kRunAsWebsite
                                                      : PHPProjectSettingsData::kRunAsCLI);
}

void PHPDebugStartDlg::OnDebugMethodChanged(wxCommandEvent& event)
{
    event.Skip();
    if (event.GetSelection() == 0) {
        // Debug URL
        CallAfter(&PHPDebugStartDlg::SetBookSelection, 0);
        m_settings.SetRunAs(PHPProjectSettingsData::kRunAsWebsite);
    } else {
        // Command line script
        CallAfter(&PHPDebugStartDlg::SetBookSelection, 1);
        m_settings.SetRunAs(PHPProjectSettingsData::kRunAsCLI);
    }
}

void PHPDebugStartDlg::OnScriptToDebugUI(wxUpdateUIEvent& event)
{
    event.Enable(!GetCheckBoxDebugActiveEditor()->IsChecked());
}

void PHPDebugStartDlg::OnUseActiveEditor(wxCommandEvent& event)
{
    if (m_manager->GetActiveEditor()) {
        GetTextCtrlScriptToDebug()->ChangeValue(m_manager->GetActiveEditor()->GetFileName().GetFullPath());
    }
}

void PHPDebugStartDlg::OnOkUI(wxUpdateUIEvent& event) { event.Enable(!GetPath().IsEmpty()); }

wxString PHPDebugStartDlg::GetPath() const
{
    if (m_choice->GetSelection() == 0) {
        // URL
        return m_comboBoxURL->GetValue();
    } else {
        return m_textCtrlScriptToDebug->GetValue();
    }
}
