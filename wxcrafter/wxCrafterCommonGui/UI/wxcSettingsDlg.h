#ifndef WXCSETTINGSDLG_H
#define WXCSETTINGSDLG_H
#include "gui.hpp"

class wxcSettingsDlg : public wxcSettingsDlgBase
{
public:
    wxcSettingsDlg(wxWindow* parent, bool standAlone);
    ~wxcSettingsDlg() override = default;
    bool IsRestartRequired() const;

protected:
    void OnOk(wxCommandEvent& event) override;

private:
    bool m_useTabModeStart = false;
    bool m_useTabModeEnd = false;
};
#endif // WXCSETTINGSDLG_H
