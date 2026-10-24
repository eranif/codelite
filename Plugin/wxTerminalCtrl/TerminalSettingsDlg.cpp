#include "TerminalSettingsDlg.hpp"

#include "cl_config.h"
#include "terminal_view.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

TerminalSettings::TerminalSettings()
    : m_defaultShell(kTerminalCommand)
{
}

TerminalSettings::TerminalSettings(bool optimizedDrawings,
                                   const wxString& lastUsedTerminalCommand,
                                   int scrollBackLines,
                                   bool showProgressBar,
                                   bool notifyWhenDone)
    : m_optimizedDrawings(optimizedDrawings)
    , m_defaultShell(lastUsedTerminalCommand)
    , m_scrollBackLines(scrollBackLines)
    , m_showProgressBar(showProgressBar)
    , m_notifyWhenDone(notifyWhenDone)
{
}

TerminalSettings TerminalSettings::Load()
{
    TerminalSettings settings{clConfig::Get().Read("terminal/optimized_drawings", false),
                              clConfig::Get().Read("terminal/last_used_shell", wxString{}),
                              clConfig::Get().Read("terminal/scroll_back_lines", 5000),
                              clConfig::Get().Read("terminal/show_progress_bar", true),
                              clConfig::Get().Read("terminal/notify_when_done", true)};
    return settings;
}

void TerminalSettings::Save()
{
    clConfig::Get().Write("terminal/optimized_drawings", m_optimizedDrawings);
    clConfig::Get().Write("terminal/last_used_shell", m_defaultShell);
    clConfig::Get().Write("terminal/scroll_back_lines", m_scrollBackLines);
    clConfig::Get().Write("terminal/show_progress_bar", m_showProgressBar);
    clConfig::Get().Write("terminal/notify_when_done", m_notifyWhenDone);
    clConfig::Get().Save();
}

TerminalSettingsDlg::TerminalSettingsDlg(wxWindow* parent, const std::vector<std::pair<wxString, wxString>>& shells)
    : TerminalSettingsBaseDlg(parent)
{
    m_settings = TerminalSettings::Load();
    // Not used when OpenGL is enabled
    m_checkBoxOptimizedDrawings->SetValue(m_settings.m_optimizedDrawings && !wxTerminalViewCtrl::IsOpenGLEnabled());
    for (const auto& [name, cmd] : shells) {
        m_choiceShell->Append(name, new wxStringClientData(cmd));
    }

    if (!m_settings.m_defaultShell.empty() && m_choiceShell->FindString(m_settings.m_defaultShell) != wxNOT_FOUND) {
        m_choiceShell->Select(m_choiceShell->FindString(m_settings.m_defaultShell));
    } else if (!m_choiceShell->IsEmpty()) {
        m_choiceShell->SetSelection(0);
    }

    m_spinCtrlScrollBackLines->SetValue(m_settings.m_scrollBackLines);
    m_checkBoxShowProgressBar->SetValue(m_settings.m_showProgressBar);
    m_checkBoxNotifyWhenDone->SetValue(m_settings.m_notifyWhenDone);
    GetSizer()->Fit(this);
    CenterOnParent();
}

TerminalSettingsDlg::~TerminalSettingsDlg() = default;

void TerminalSettingsDlg::OnOptimizedDrawingsUI(wxUpdateUIEvent& event)
{
#ifdef __WXGTK__
    // The OpenGL renderer does not need it
    if (wxTerminalViewCtrl::IsOpenGLEnabled()) {
        event.Check(false);
        event.Enable(false);
    } else {
        event.Enable(true);
    }
#else
    event.Check(false);
    event.Enable(false);
#endif
}

void TerminalSettingsDlg::OnOk(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto settings = GetSettings();
    settings.Save();
    CallAfter(&wxDialog::EndModal, wxID_OK);
}

TerminalSettings TerminalSettingsDlg::GetSettings() const
{
    TerminalSettings settings;
    settings.m_optimizedDrawings = m_checkBoxOptimizedDrawings->IsChecked() && !wxTerminalViewCtrl::IsOpenGLEnabled();
    settings.m_scrollBackLines = m_spinCtrlScrollBackLines->GetValue();
    settings.m_showProgressBar = m_checkBoxShowProgressBar->IsChecked();
    settings.m_notifyWhenDone = m_checkBoxNotifyWhenDone->IsChecked();
    settings.m_defaultShell = kTerminalCommand;
    int selection = m_choiceShell->GetSelection();
    if (selection != wxNOT_FOUND) {
        auto* clientData = static_cast<wxStringClientData*>(m_choiceShell->GetClientObject(selection));
        settings.m_defaultShell = clientData->GetData();
    }
    return settings;
}
