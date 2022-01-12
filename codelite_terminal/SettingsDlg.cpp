#include "SettingsDlg.h"

#include "wxTerminalOptions.h"

#include <drawingutils.h>

SettingsDlg::SettingsDlg(wxWindow* parent)
    : SettingsDlgBase(parent)
{
    Load();
}

SettingsDlg::~SettingsDlg() {}

void SettingsDlg::Load()
{
    m_fontPicker->SetSelectedFont(wxTerminalOptions::Get().GetFont());
    m_colourPickerBG->SetColour(wxTerminalOptions::Get().GetBgColour());
    m_colourPickerFG->SetColour(wxTerminalOptions::Get().GetTextColour());
}

void SettingsDlg::Save()
{
    wxTerminalOptions::Get().SetFont(GetFontPicker()->GetSelectedFont());
    wxTerminalOptions::Get().SetBgColour(GetColourPickerBG()->GetColour());
    wxTerminalOptions::Get().SetTextColour(GetColourPickerFG()->GetColour());
}
