#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include "wxcrafter.h"

class SettingsDlg : public SettingsDlgBase
{
public:
    SettingsDlg(wxWindow* parent);
    virtual ~SettingsDlg();

    wxColour GetTextColour() const { return m_colourPickerFG->GetColour(); }
    wxColour GetBgColour() const { return m_colourPickerBG->GetColour(); }
    wxFont GetTextFont() const { return m_fontPicker->GetSelectedFont(); }
};
#endif // SETTINGSDLG_H
