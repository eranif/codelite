#pragma once

#include "ai_UI.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK SystemPromptDialog : public SystemPromptDialogBase
{
public:
    SystemPromptDialog(wxWindow* parent, const wxString& content = wxEmptyString);
    ~SystemPromptDialog() override;
    wxString GetValue() const { return m_stcPrompt->GetValue(); }

protected:
    void OnClose(wxCommandEvent& event) override;
};
