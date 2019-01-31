#include "FontPickerDlg.h"
#include "wxgui_helpers.h"
#include <windowattrmanager.h>

FontPickerDlg::FontPickerDlg(wxWindow* parent, const wxString& font)
    : FontPickerDlgBaseClass(parent)
    , m_fontname(font)
{
    wxFont f = wxCrafter::StringToFont(m_fontname);
    if(wxCrafter::IsSystemFont(m_fontname)) {
        wxString name = m_fontname.BeforeFirst(wxT(','));
        int where = m_choiceSystemFonts->FindString(name);
        if(where != wxNOT_FOUND) { m_choiceSystemFonts->SetSelection(where); }

        m_checkBoxBold->SetValue(f.GetWeight() == wxFONTWEIGHT_BOLD);
        m_checkBoxItalic->SetValue(f.GetStyle() == wxFONTSTYLE_ITALIC);
        m_checkBoxUnderlined->SetValue(f.GetUnderlined());
        m_checkBoxPreDefinedFont->SetValue(true);
        DoUpdateSelectionToPreDefinedFont();

    } else if(f.IsOk()) {
        m_fontPicker->SetSelectedFont(f);
        m_checkBoxCustomFont->SetValue(true);
        DoUpdateSelectionToCustomFont();
    }

    if(f.IsOk()) {
        m_staticTextSample->SetFont(f);
        m_staticTextSample->SetLabel(wxT("Sample Text"));
    }
    SetName("FontPickerDlg");
    WindowAttrManager::Load(this);
}

FontPickerDlg::~FontPickerDlg() {}

void FontPickerDlg::OnUseCustomFont(wxCommandEvent& event)
{
    m_checkBoxPreDefinedFont->SetValue(false);
    wxFont font = m_fontPicker->GetSelectedFont();
    m_fontname = wxCrafter::FontToString(font);
    DoUpdateSelectionToCustomFont();
}

void FontPickerDlg::OnUseCustomFontUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxCustomFont->IsChecked()); }

void FontPickerDlg::OnUsePreDefinedFontUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxPreDefinedFont->IsChecked());
}

void FontPickerDlg::OnSystemFontSelected(wxCommandEvent& event) { DoUpdateSelectionToPreDefinedFont(); }

void FontPickerDlg::OnFontSelected(wxFontPickerEvent& event) { DoUpdateSelectionToCustomFont(); }

void FontPickerDlg::OnUsePreDefinedFont(wxCommandEvent& event)
{
    m_checkBoxCustomFont->SetValue(false);
    DoUpdateSelectionToPreDefinedFont();
}

void FontPickerDlg::DoUpdateSelectionToPreDefinedFont()
{
    // Update the preview
    wxFont font = wxCrafter::StringToFont(m_choiceSystemFonts->GetStringSelection());
    if(font.IsOk()) {

        if(wxCrafter::IsSystemFont(m_choiceSystemFonts->GetStringSelection())) {
            m_fontname = m_choiceSystemFonts->GetStringSelection();
            if(m_checkBoxItalic->IsChecked()) {
                m_fontname << wxT(",italic");
                font.SetStyle(wxFONTSTYLE_ITALIC);
            } else
                m_fontname << wxT(",normal");

            if(m_checkBoxBold->IsChecked()) {
                m_fontname << wxT(",bold");
                font.SetWeight(wxFONTWEIGHT_BOLD);
            } else
                m_fontname << wxT(",normal");

            if(m_checkBoxUnderlined->IsChecked()) {
                m_fontname << wxT(",underlined");
                font.SetUnderlined(true);
            } else
                m_fontname << wxT(",normal");

            m_staticTextSample->SetFont(font);
            m_staticTextSample->SetLabel(wxT("Sample Text"));

        } else {
            m_fontname = wxCrafter::FontToString(font);
        }
    }
}
void FontPickerDlg::DoUpdateSelectionToCustomFont()
{
    wxFont font = m_fontPicker->GetSelectedFont();
    m_fontname = wxCrafter::FontToString(font);

    m_staticTextSample->SetFont(font);
    m_staticTextSample->SetLabel(wxT("Sample Text"));
}
