#include "wxcSettingsDlg.h"

#include "wxc_settings.h"

wxcSettingsDlg::wxcSettingsDlg(wxWindow* parent, bool standAlone)
    : wxcSettingsDlgBase(parent)
{
    m_useTabModeStart = m_useTabModeEnd = wxcSettings::Get().HasFlag(wxcSettings::USE_TABBED_MODE);
    m_checkBoxFormatInheritedFiles->SetValue(wxcSettings::Get().HasFlag(wxcSettings::FORMAT_INHERITED_FILES));
    m_checkBoxKeepAllPossibleNames->SetValue(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES));
    m_checkBoxKeepAllUsersetNames->SetValue(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES));
    m_checkBoxCopyEventhandlerToo->SetValue(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO));
    m_checkBoxUseTRay->SetValue(wxcSettings::Get().HasFlag(wxcSettings::EXIT_MINIMIZE_TO_TRAY));

    if (!standAlone) {
        m_checkBoxUseTRay->Set3StateValue(wxCheckBoxState::wxCHK_UNCHECKED);
        m_checkBoxUseTRay->Hide();
        staticBoxSizerGeneral->Show(false);
    }
}

void wxcSettingsDlg::OnOk(wxCommandEvent& event)
{
    wxcSettings::Get().EnableFlag(wxcSettings::FORMAT_INHERITED_FILES, m_checkBoxFormatInheritedFiles->IsChecked());
    wxcSettings::Get().EnableFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES, m_checkBoxKeepAllPossibleNames->IsChecked());
    wxcSettings::Get().EnableFlag(
        wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES, m_checkBoxKeepAllUsersetNames->IsChecked());
    wxcSettings::Get().EnableFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO, m_checkBoxCopyEventhandlerToo->IsChecked());
    wxcSettings::Get().EnableFlag(wxcSettings::EXIT_MINIMIZE_TO_TRAY, m_checkBoxUseTRay->IsChecked());
    wxcSettings::Get().Save();

    m_useTabModeEnd = wxcSettings::Get().HasFlag(wxcSettings::USE_TABBED_MODE);
    EndModal(wxID_OK);
}

bool wxcSettingsDlg::IsRestartRequired() const { return m_useTabModeStart != m_useTabModeEnd; }
