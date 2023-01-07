#ifndef WXCSETTINGSDLG_H
#define WXCSETTINGSDLG_H
#include "gui.h"

class wxcSettingsDlg : public wxcSettingsDlgBase
{
    bool m_useTabModeStart;
    bool m_useTabModeEnd;

public:
    wxcSettingsDlg(wxWindow* parent);
    virtual ~wxcSettingsDlg();
    bool IsRestartRequired() const;

protected:
    virtual void OnMinimizeToTrayUI(wxUpdateUIEvent& event);
    virtual void OnUseAsTabUI(wxUpdateUIEvent& event);
    virtual void OnOk(wxCommandEvent& event);
};
#endif // WXCSETTINGSDLG_H
