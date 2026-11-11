#pragma once

#include "wxc_settings.h"
#include "wxcrafter.hpp"

class DefineCustomControlWizard : public DefineCustomControlWizardBaseClass
{
public:
    DefineCustomControlWizard(wxWindow* parent);
    ~DefineCustomControlWizard() override = default;
    CustomControlTemplate GetControl() const;

protected:
    void OnDeleteEvent(wxCommandEvent& event) override;
    void OnDeleteEventUI(wxUpdateUIEvent& event) override;
    void OnNewEvent(wxCommandEvent& event) override;
    void OnPageChanging(wxWizardEvent& event) override;
};
