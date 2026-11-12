#pragma once

#include "LanguageServerEntry.h"
#include "lsp_UI.hpp"

#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK LanguageServerPage;
class WXDLLIMPEXP_SDK NewLanguageServerDlg : public NewLanguageServerDlgBase
{
    LanguageServerPage* m_page = nullptr;

public:
    NewLanguageServerDlg(wxWindow* parent);
    virtual ~NewLanguageServerDlg() = default;

    LanguageServerEntry GetData() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
