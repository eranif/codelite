#include "TerminalSettingsDlg.hpp"

#include "cl_config.h"
#include "terminal_view.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

namespace
{
/// A theme colour that the outline of the current tab can have.
struct TabOutlineColour {
    const char* key;      ///< The value in config.ini. It is never translated.
    const wxString label; ///< The name in the settings dialog.
    /// The theme colour. nullptr for the default entry: its colour is calculated from the background.
    wxColour wxTerminalTheme::* member;
};

constexpr const char* kDefaultTabOutlineKey = "default";

const std::vector<TabOutlineColour>& GetTabOutlineColours()
{
    static const std::vector<TabOutlineColour> colours = {
        {"fg", _("Text"), &wxTerminalTheme::fg},
        {"red", _("Red"), &wxTerminalTheme::red},
        {"green", _("Green"), &wxTerminalTheme::green},
        {"yellow", _("Yellow"), &wxTerminalTheme::yellow},
        {"blue", _("Blue"), &wxTerminalTheme::blue},
        {"magenta", _("Magenta"), &wxTerminalTheme::magenta},
        {"cyan", _("Cyan"), &wxTerminalTheme::cyan},
        {"white", _("White"), &wxTerminalTheme::white},
        {"brightred", _("Bright Red"), &wxTerminalTheme::brightRed},
        {"brightgreen", _("Bright Green"), &wxTerminalTheme::brightGreen},
        {"brightyellow", _("Bright Yellow"), &wxTerminalTheme::brightYellow},
        {"brightblue", _("Bright Blue"), &wxTerminalTheme::brightBlue},
        {"brightmagenta", _("Bright Magenta"), &wxTerminalTheme::brightMagenta},
        {"brightcyan", _("Bright Cyan"), &wxTerminalTheme::brightCyan},
        {"brightwhite", _("Bright White"), &wxTerminalTheme::brightWhite},
        {"selection", _("Selection"), &wxTerminalTheme::selectionBg},
        {"highlight", _("Search Match"), &wxTerminalTheme::highlightBg},
        {"cursor", _("Cursor"), &wxTerminalTheme::cursorColour},
        {"link", _("Link"), &wxTerminalTheme::linkColour},
        // Keep this one last: GetDefaultTabOutlineIndex() relies on it
        {kDefaultTabOutlineKey, _("Default"), nullptr},
    };
    return colours;
}

/// The index of the default entry (the last one) in the table.
int GetDefaultTabOutlineIndex() { return static_cast<int>(GetTabOutlineColours().size()) - 1; }

/// The index of `key` in the table, or wxNOT_FOUND when `key` is unknown.
int FindTabOutlineColour(const wxString& key)
{
    const auto& colours = GetTabOutlineColours();
    for (size_t i = 0; i < colours.size(); ++i) {
        if (key == colours[i].key) {
            return static_cast<int>(i);
        }
    }
    return wxNOT_FOUND;
}
} // namespace

wxColour GetTabOutlineColourOf(const wxTerminalTheme& theme, const wxString& key)
{
    // An unknown key is the same as the default
    int index = FindTabOutlineColour(key);
    if (index == wxNOT_FOUND) {
        index = GetDefaultTabOutlineIndex();
    }

    const auto member = GetTabOutlineColours()[index].member;
    const wxColour colour = member != nullptr ? theme.*member : theme.bg.ChangeLightness(theme.IsDark() ? 120 : 80);
    // Some theme colours have alpha; the outline is solid.
    return wxColour(colour.Red(), colour.Green(), colour.Blue());
}

TerminalSettings::TerminalSettings()
    : m_defaultShell(kTerminalCommand)
{
}

TerminalSettings::TerminalSettings(bool optimizedDrawings,
                                   const wxString& lastUsedTerminalCommand,
                                   int scrollBackLines,
                                   bool showProgressBar,
                                   bool notifyWhenDone,
                                   const wxString& tabOutlineColour)
    : m_optimizedDrawings(optimizedDrawings)
    , m_defaultShell(lastUsedTerminalCommand)
    , m_scrollBackLines(scrollBackLines)
    , m_showProgressBar(showProgressBar)
    , m_notifyWhenDone(notifyWhenDone)
    , m_tabOutlineColour(tabOutlineColour)
{
}

TerminalSettings TerminalSettings::Load()
{
    TerminalSettings settings{
        clConfig::Get().Read("terminal/optimized_drawings", false),
        clConfig::Get().Read("terminal/last_used_shell", wxString{}),
        clConfig::Get().Read("terminal/scroll_back_lines", 5000),
        clConfig::Get().Read("terminal/show_progress_bar", true),
        clConfig::Get().Read("terminal/notify_when_done", true),
        clConfig::Get().Read("terminal/tab_outline_colour", wxString{kDefaultTabOutlineKey}),
    };
    return settings;
}

void TerminalSettings::Save()
{
    clConfig::Get().Write("terminal/optimized_drawings", m_optimizedDrawings);
    clConfig::Get().Write("terminal/last_used_shell", m_defaultShell);
    clConfig::Get().Write("terminal/scroll_back_lines", m_scrollBackLines);
    clConfig::Get().Write("terminal/show_progress_bar", m_showProgressBar);
    clConfig::Get().Write("terminal/notify_when_done", m_notifyWhenDone);
    clConfig::Get().Write("terminal/tab_outline_colour", m_tabOutlineColour);
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

    for (const auto& entry : GetTabOutlineColours()) {
        m_choiceOutlineColour->Append(entry.label);
    }
    // An unknown key selects the default colour, the same as GetTabOutlineColourOf()
    int sel = FindTabOutlineColour(m_settings.m_tabOutlineColour);
    if (sel == wxNOT_FOUND) {
        sel = GetDefaultTabOutlineIndex();
    }
    m_choiceOutlineColour->SetSelection(sel);
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
    // Store the key, not the label
    const int outline = m_choiceOutlineColour->GetSelection();
    const auto& colours = GetTabOutlineColours();
    if (outline != wxNOT_FOUND && static_cast<size_t>(outline) < colours.size()) {
        settings.m_tabOutlineColour = colours[outline].key;
    } else {
        settings.m_tabOutlineColour = m_settings.m_tabOutlineColour;
    }
    settings.m_defaultShell = kTerminalCommand;
    int selection = m_choiceShell->GetSelection();
    if (selection != wxNOT_FOUND) {
        auto* clientData = static_cast<wxStringClientData*>(m_choiceShell->GetClientObject(selection));
        settings.m_defaultShell = clientData->GetData();
    }
    return settings;
}

void TerminalSettingsDlg::OnOutlineColourChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if (m_onOutlineColourChanged) {
        m_onOutlineColourChanged(GetSettings().m_tabOutlineColour);
    }
}
