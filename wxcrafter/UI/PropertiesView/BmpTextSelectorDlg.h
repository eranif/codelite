#ifndef BMPTEXTSELECTORDLG_H
#define BMPTEXTSELECTORDLG_H

#include "wxcrafter.h"
#include <vector>

typedef std::vector<std::pair<wxString, wxString> > BmpTextVec_t;

class BmpTextSelectorDlg : public BmpTextSelectorDlgBase
{
public:
    BmpTextSelectorDlg(wxWindow* parent, const wxString& initialValue);
    ~BmpTextSelectorDlg() override = default;

    static BmpTextVec_t FromString(const wxString& text);
    static wxString ToString(const BmpTextVec_t& vec);
    wxString GetValue();

protected:
    void OnItemActivated(wxDataViewEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;
    void OnEditUI(wxUpdateUIEvent& event) override;
    void OnDelete(wxCommandEvent& event) override;
    void OnEdit(wxCommandEvent& event) override;
    void OnNew(wxCommandEvent& event) override;
};
#endif // BMPTEXTSELECTORDLG_H
