#ifndef BMPTEXTSELECTORDLG_H
#define BMPTEXTSELECTORDLG_H

#include "wxcrafter.h"
#include <vector>

typedef std::vector<std::pair<wxString, wxString> > BmpTextVec_t;

class BmpTextSelectorDlg : public BmpTextSelectorDlgBase
{
public:
    BmpTextSelectorDlg(wxWindow* parent, const wxString& initialValue);
    virtual ~BmpTextSelectorDlg();

    static BmpTextVec_t FromString(const wxString& text);
    static wxString ToString(const BmpTextVec_t& vec);
    wxString GetValue();

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
    virtual void OnEditUI(wxUpdateUIEvent& event);
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnEdit(wxCommandEvent& event);
    virtual void OnNew(wxCommandEvent& event);
};
#endif // BMPTEXTSELECTORDLG_H
