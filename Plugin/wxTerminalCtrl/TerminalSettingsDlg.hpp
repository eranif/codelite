#pragma once

#include "codelite_exports.h"
#include "wxTerminalCtrlUI.hpp"

#include <functional>
#include <vector>

struct wxTerminalTheme;

struct WXDLLIMPEXP_SDK TerminalSettings {
    TerminalSettings();
    TerminalSettings(bool optimizedDrawings,
                     const wxString& lastUsedTerminalCommand,
                     int scrollBackLines,
                     bool showProgressBar,
                     bool notifyWhenDone,
                     const wxString& tabOutlineColour);
    bool m_optimizedDrawings{false};
    wxString m_defaultShell{wxEmptyString};
    int m_scrollBackLines{5000};
    /// Show the progress bar above the terminal
    bool m_showProgressBar{true};
    /// Tell the user when a program (an agent, for example) is done, or needs attention
    bool m_notifyWhenDone{true};
    /// The theme colour of the outline of the current tab (the key, see GetTabOutlineColourOf())
    wxString m_tabOutlineColour{"fg"};

    static TerminalSettings Load();
    void Save();
};

/// Resolve the outline colour `key` (see TerminalSettings::m_tabOutlineColour) in `theme`.
/// An unknown key gives the default colour. The result is always solid.
wxColour WXDLLIMPEXP_SDK GetTabOutlineColourOf(const wxTerminalTheme& theme, const wxString& key);

class WXDLLIMPEXP_SDK TerminalSettingsDlg : public TerminalSettingsBaseDlg
{
public:
    TerminalSettingsDlg(wxWindow* parent, const std::vector<std::pair<wxString, wxString>>& shells);
    ~TerminalSettingsDlg() override;
    TerminalSettings GetSettings() const;
    /// Called with the outline colour key every time the user selects another colour
    void SetOutlineColourPreview(std::function<void(const wxString&)> callback)
    {
        m_onOutlineColourChanged = std::move(callback);
    }

protected:
    void OnOutlineColourChanged(wxCommandEvent& event) override;
    void OnOk(wxCommandEvent& event) override;
    void OnOptimizedDrawingsUI(wxUpdateUIEvent& event) override;

private:
    TerminalSettings m_settings;
    std::function<void(const wxString&)> m_onOutlineColourChanged;
};
