#ifndef ADDFUNCTIONSIMPDLG_H
#define ADDFUNCTIONSIMPDLG_H
#include "wxcrafter.h"
#include "entry.h"

class AddFunctionsImpDlg : public AddFunctionsImplBaseDlg
{
    TagEntryPtrVector_t m_tags;
public:
    AddFunctionsImpDlg(wxWindow* parent, const TagEntryPtrVector_t &tags, const wxString &targetFile);
    virtual ~AddFunctionsImpDlg();

    wxString GetText() const;
    wxString GetFileName() const;
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnCheckAll(wxCommandEvent& event);
    virtual void OnUncheckAll(wxCommandEvent& event);
};

#endif // ADDFUNCTIONSIMPDLG_H
