#ifndef DUPLICATETLWDLG_H
#define DUPLICATETLWDLG_H
#include "wxcrafter.h"

class DuplicateTLWDlg : public DuplicateTLWBaseDlg
{
public:
    DuplicateTLWDlg(wxWindow* parent);
    ~DuplicateTLWDlg() override = default;

    wxString GetBaseName() const { return m_textBasename->GetValue(); }
    wxString GetInheritedName() const { return m_textInheritedName->GetValue(); }
    wxString GetFilename() const { return m_textFilename->GetValue(); }

protected:
    void OnFilenameFocus(wxFocusEvent& event) override;
    void OnInheritedNameFocus(wxFocusEvent& event) override;
};
#endif // DUPLICATETLWDLG_H
