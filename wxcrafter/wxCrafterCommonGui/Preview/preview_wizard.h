#pragma once

#include "wizard_wrapper.h"

#include <wx/wizard.h>

class PreviewWizard : public wxWizard
{
    DECLARE_EVENT_TABLE()
    void OnClose(wxCloseEvent& e);
    void OnWizard(wxWizardEvent& e);

public:
    PreviewWizard(wxWindow* parent, const WizardWrapper& dw);
    ~PreviewWizard() override;
    void OnClosePreview(wxCommandEvent& e);

    void Run();
};
