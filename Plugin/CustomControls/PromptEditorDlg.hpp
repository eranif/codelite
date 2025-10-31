#pragma once

#include "CustomControls.hpp"
#include "ai/Config.hpp"
#include "codelite_exports.h"

#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK PromptEditorDlg : public PromptEditorBaseDlg
{
public:
    PromptEditorDlg(wxWindow* parent);
    ~PromptEditorDlg() override;

protected:
    void OnDefaults(wxCommandEvent& event) override;
    void OnPromptChanged(wxDataViewEvent& event) override;
    void OnSave(wxCommandEvent& event) override;
    void OnSaveUI(wxUpdateUIEvent& event) override;
    void LoadPrompts();
    void SelectRow(size_t row);
};
