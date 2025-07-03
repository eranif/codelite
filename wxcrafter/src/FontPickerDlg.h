#ifndef FONTPICKERDLG_H
#define FONTPICKERDLG_H
#include "wxcrafter.h"

class FontPickerDlg : public FontPickerDlgBaseClass
{
    wxString m_fontname;

protected:
    void OnUseCustomFont(wxCommandEvent& event) override;
    void OnUseCustomFontUI(wxUpdateUIEvent& event) override;
    void OnUsePreDefinedFontUI(wxUpdateUIEvent& event) override;
    void OnSystemFontSelected(wxCommandEvent& event) override;
    void OnFontSelected(wxFontPickerEvent& event) override;
    void OnUsePreDefinedFont(wxCommandEvent& event) override;

protected:
    void DoUpdateSelectionToPreDefinedFont();
    void DoUpdateSelectionToCustomFont();

public:
    FontPickerDlg(wxWindow* parent, const wxString& font);
    ~FontPickerDlg() override = default;

    const wxString& GetFontName() const
    {
        static wxString EMPTY_STRING;
        if(m_checkBoxCustomFont->IsChecked() == false && m_checkBoxPreDefinedFont->IsChecked() == false)
            return EMPTY_STRING;

        return m_fontname;
    }
};
#endif // FONTPICKERDLG_H
