#include "SettingsDlg.h"
#include "my_config.h"

SettingsDlg::SettingsDlg(wxWindow* parent)
    : SettingsDlgBase(parent)
{
    MyConfig config;
    m_bg_colour = config.GetBgColour();
    m_fg_colour = config.GetFgColour();
    m_font      = config.GetFont();
    
    m_fontPicker->SetFont( m_font );
    m_colourPickerBG->SetColour( m_bg_colour );
    m_colourPickerFG->SetColour( m_fg_colour );
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::OnFontSelected(wxFontPickerEvent& event)
{
    m_font = event.GetFont();
}

void SettingsDlg::OnBGColour(wxColourPickerEvent& event)
{
    m_bg_colour = event.GetColour();
}

void SettingsDlg::OnFGColour(wxColourPickerEvent& event)
{
    m_fg_colour = event.GetColour();
}
