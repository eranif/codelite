#ifndef FONTPICKERDLG_H
#define FONTPICKERDLG_H
#include "wxcrafter.h"

class FontPickerDlg : public FontPickerDlgBaseClass
{
    wxString m_fontname;

protected:
    virtual void OnUseCustomFont(wxCommandEvent& event);
    virtual void OnUseCustomFontUI(wxUpdateUIEvent& event);
    virtual void OnUsePreDefinedFontUI(wxUpdateUIEvent& event);
    virtual void OnSystemFontSelected(wxCommandEvent& event);
    virtual void OnFontSelected(wxFontPickerEvent& event);
    virtual void OnUsePreDefinedFont(wxCommandEvent& event);

protected:
    void DoUpdateSelectionToPreDefinedFont();
    void DoUpdateSelectionToCustomFont();

public:
    FontPickerDlg(wxWindow* parent, const wxString& font);
    virtual ~FontPickerDlg();

    const wxString& GetFontName() const
    {
        static wxString EMPTY_STRING;
        if(m_checkBoxCustomFont->IsChecked() == false && m_checkBoxPreDefinedFont->IsChecked() == false)
            return EMPTY_STRING;

        return m_fontname;
    }
};
#endif // FONTPICKERDLG_H
