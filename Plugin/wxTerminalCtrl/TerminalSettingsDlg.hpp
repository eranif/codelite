#pragma once

#include "codelite_exports.h"
#include "terminal_view.h"
#include "wxTerminalCtrlUI.hpp"

#include <vector>

struct WXDLLIMPEXP_SDK TerminalSettings {
    TerminalSettings();
    TerminalSettings(bool optimizedDrawings, const wxString& lastUsedTerminalCommand, int scrollBackLines);
    bool m_optimizedDrawings{!wxTerminalViewCtrl::IsOpenGLEnabled()};
    wxString m_defaultShell{wxEmptyString};
    int m_scrollBackLines{5000};

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
