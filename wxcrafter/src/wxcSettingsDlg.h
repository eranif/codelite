#ifndef WXCSETTINGSDLG_H
#define WXCSETTINGSDLG_H
#include "gui.h"

class wxcSettingsDlg : public wxcSettingsDlgBase
{
    bool m_useTabModeStart;
    bool m_useTabModeEnd;

public:
    wxcSettingsDlg(wxWindow* parent);
    ~wxcSettingsDlg() override = default;
    bool IsRestartRequired() const;

protected:
    void OnMinimizeToTrayUI(wxUpdateUIEvent& event) override;
    virtual void OnUseAsTabUI(wxUpdateUIEvent& event);
    void OnOk(wxCommandEvent& event) override;
};
#endif // WXCSETTINGSDLG_H
