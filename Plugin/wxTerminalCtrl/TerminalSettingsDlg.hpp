#pragma once

#include "codelite_exports.h"
#include "wxTerminalCtrlUI.hpp"

#include <vector>

struct WXDLLIMPEXP_SDK TerminalSettings {
    TerminalSettings();
    TerminalSettings(bool optimizedDrawings,
                     const wxString& lastUsedTerminalCommand,
                     int scrollBackLines,
                     bool showProgressBar,
                     bool notifyWhenDone);
    bool m_optimizedDrawings{false};
    wxString m_defaultShell{wxEmptyString};
    int m_scrollBackLines{5000};
    /// Show the progress bar above the terminal
    bool m_showProgressBar{true};
    /// Tell the user when a program (an agent, for example) is done, or needs attention
    bool m_notifyWhenDone{true};

    static TerminalSettings Load();
    void Save();
};

class WXDLLIMPEXP_SDK TerminalSettingsDlg : public TerminalSettingsBaseDlg
{
public:
    TerminalSettingsDlg(wxWindow* parent, const std::vector<std::pair<wxString, wxString>>& shells);
    ~TerminalSettingsDlg() override;
    TerminalSettings GetSettings() const;

protected:
    void OnOk(wxCommandEvent& event) override;
    void OnOptimizedDrawingsUI(wxUpdateUIEvent& event) override;

private:
    TerminalSettings m_settings;
};
