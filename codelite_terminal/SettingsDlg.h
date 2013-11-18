#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H
#include "wxcrafter.h"

class SettingsDlg : public SettingsDlgBase
{
    wxFont m_font;
    wxColour m_bg_colour;
    wxColour m_fg_colour;

public:
    SettingsDlg(wxWindow* parent);
    virtual ~SettingsDlg();
    void SetBgColour(const wxColour& bg_colour) {
        this->m_bg_colour = bg_colour;
    }
    void SetFgColour(const wxColour& fg_colour) {
        this->m_fg_colour = fg_colour;
    }
    const wxColour& GetBgColour() const {
        return m_bg_colour;
    }
    const wxColour& GetFgColour() const {
        return m_fg_colour;
    }
    const wxFont& GetFont() const {
        return m_font;
    }
protected:
    virtual void OnBGColour(wxColourPickerEvent& event);
    virtual void OnFGColour(wxColourPickerEvent& event);
    virtual void OnFontSelected(wxFontPickerEvent& event);
};
#endif // SETTINGSDLG_H
