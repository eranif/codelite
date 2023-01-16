#ifndef NEWLANGUAGESERVERDLG_H
#define NEWLANGUAGESERVERDLG_H

#include "LanguageServerEntry.h"
#include "UI.h"

#include <wx/arrstr.h>

class LanguageServerPage;
class NewLanguageServerDlg : public NewLanguageServerDlgBase
{
    LanguageServerPage* m_page = nullptr;

public:
    NewLanguageServerDlg(wxWindow* parent);
    virtual ~NewLanguageServerDlg();

    LanguageServerEntry GetData() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWLANGUAGESERVERDLG_H
